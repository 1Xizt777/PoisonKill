#include "Interaction/PKInteractionComponent.h"

#include "GameFramework/Pawn.h"
#include "Interaction/PKInteractable.h"
#include "Kismet/GameplayStatics.h"

UPKInteractionComponent::UPKInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPKInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	RefreshCurrentTarget();
}

void UPKInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (bInteractionPressed)
	{
		EndInteraction(true);
	}

	Super::EndPlay(EndPlayReason);
}

void UPKInteractionComponent::TickComponent(const float DeltaTime , const ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TargetRefreshElapsed += DeltaTime;
	if (TargetRefreshElapsed >= TargetRefreshInterval)
	{
		TargetRefreshElapsed = 0.0f;
		RefreshCurrentTarget();
	}

	if (!bInteractionPressed || !CurrentTarget)
	{
		return;
	}

	ElapsedHoldTime += DeltaTime;
	BroadcastProgress();

	APawn* Interactor = Cast<APawn>(GetOwner());
	IPKInteractable::Execute_OnInteractionProgressUpdated(CurrentTarget , Interactor , GetNormalizedProgress());

	if (ElapsedHoldTime >= CurrentRequest.HoldDuration)
	{
		CompleteInteraction();
	}
}

bool UPKInteractionComponent::BeginInteraction()
{
	if (bInteractionPressed)	//如果已经按住，直接返回
	{
		return true;
	}

	RefreshCurrentTarget();		//重新确认当前目标
	if (!CurrentTarget)
	{
		return false;
	}

	if (CurrentRequest.HoldDuration <= 0.0f)	//持续时间小于等于 0 则立即完成
	{
		ElapsedHoldTime = CurrentRequest.HoldDuration;
		CompleteInteraction();
		return true;
	}

	bInteractionPressed = true;		//设置 pressed = true
	bInteractionLocked = CurrentRequest.bLockPlayerInput;		//是否锁input
	ElapsedHoldTime = CurrentRequest.bRetainProgressOnRelease ? RetainedProgress.FindRef(CurrentTarget) : 0.0f;  //是否保留进度
	ElapsedHoldTime = FMath::Clamp(ElapsedHoldTime, 0.0f, CurrentRequest.HoldDuration);	//更新当前按住的时间

	APawn* Interactor = Cast<APawn>(GetOwner());
	IPKInteractable::Execute_OnInteractionStarted(CurrentTarget, Interactor);	//执行开始交互函数
	
	BroadcastProgress();	//广播进度（ElapsedHoldTime / CurrentRequest.HoldDuration）当前按住的时间/所需时间
	
	return true;		
}

void UPKInteractionComponent::EndInteraction(const bool bForceCancel)
{
	if (!bInteractionPressed)	//如果没按，直接返回
	{
		return;
	}

	bInteractionPressed = false;	//pressed = false
	bInteractionLocked = false;		//锁input = false

	if (CurrentTarget)
	{
		APawn* Interactor = Cast<APawn>(GetOwner());
		IPKInteractable::Execute_OnInteractionCanceled(CurrentTarget, Interactor);	//调用目标的 OnInteractionCanceled()

		if (CurrentRequest.bRetainProgressOnRelease && !bForceCancel)	//保留中断进度 && 不强行取消
		{
			RetainedProgress.Add(CurrentTarget, ElapsedHoldTime);	//保留进度
		}
		else
		{
			RetainedProgress.Remove(CurrentTarget);	//移去进度并清零
			ElapsedHoldTime = 0.0f;
		}
	}

	OnInteractionCanceled.Broadcast(CurrentTarget);	//广播取消
	BroadcastProgress();	//广播进度
}

void UPKInteractionComponent::CancelInteraction()
{
	EndInteraction(true); //Npc看见强制取消
}

bool UPKInteractionComponent::IsInteractionLocked() const
{
	return bInteractionLocked;
}

bool UPKInteractionComponent::IsInteractionPressed() const
{
	return bInteractionPressed;
}

AActor* UPKInteractionComponent::GetCurrentTarget() const
{
	return CurrentTarget;
}

float UPKInteractionComponent::GetNormalizedProgress() const
{
	if (CurrentRequest.HoldDuration <= 0.0f)
	{
		return ElapsedHoldTime > 0.0f ? 1.0f : 0.0f;
	}

	return FMath::Clamp(ElapsedHoldTime / CurrentRequest.HoldDuration, 0.0f, 1.0f);
}

const FPKInteractionRequest& UPKInteractionComponent::GetCurrentRequest() const
{
	return CurrentRequest;
}

void UPKInteractionComponent::RefreshCurrentTarget()
{
	UWorld* World = GetWorld();
	APawn* Interactor = Cast<APawn>(GetOwner()); //交互者
	if (!World || !Interactor)
	{
		return;
	}

	TArray<AActor*> InteractableActors;
	
	//通过接口获取所有可交互Actors
	UGameplayStatics::GetAllActorsWithInterface(this,UPKInteractable::StaticClass(),InteractableActors);

	AActor* BestTarget = nullptr;
	FPKInteractionRequest BestRequest;
	int32 BestPriority = MIN_int32;
	float BestDistanceSquared = TNumericLimits<float>::Max();
	const FVector InteractorLocation = Interactor->GetActorLocation();
	const float RangeSquared = FMath::Square(InteractionRange);

	for (AActor* Candidate : InteractableActors)
	{
		if (!IsValid(Candidate) || Candidate == Interactor)		//候选者安全判定
		{
			continue;
		}

		const float DistanceSquared = FVector::DistSquared(		//两点距离平方
			InteractorLocation,
			Candidate->GetActorLocation()
		);
		
		if (DistanceSquared > RangeSquared)		//判断范围
		{
			continue;
		}

		FPKInteractionRequest CandidateRequest;
		if (!IPKInteractable::Execute_CanInteract(Candidate,Interactor,CandidateRequest))		//是否CanInteract
		{
			continue;
		}

		//优先级判断
		const bool bHigherPriority = CandidateRequest.Priority > BestPriority;
		
		//相同优先级但更近
		const bool bSamePriorityAndCloser =CandidateRequest.Priority == BestPriority && DistanceSquared < BestDistanceSquared;

		if (bHigherPriority || bSamePriorityAndCloser)
		{
			BestTarget = Candidate;		//选定最佳候选者
			BestRequest = CandidateRequest;
			BestPriority = CandidateRequest.Priority;
			BestDistanceSquared = DistanceSquared;
		}
	}

	if (bInteractionPressed && BestTarget != CurrentTarget)
	{
		EndInteraction(true);
	}

	SetCurrentTarget(BestTarget, BestRequest);
}

void UPKInteractionComponent::SetCurrentTarget(AActor* NewTarget,const FPKInteractionRequest& NewRequest)
{
	const bool bTargetChanged = CurrentTarget != NewTarget;
	const bool bPromptChanged = !CurrentRequest.PromptText.EqualTo(NewRequest.PromptText);

	CurrentTarget = NewTarget;
	CurrentRequest = NewRequest;

	if (bTargetChanged || bPromptChanged)	//Target变化 || Prompt变化
	{
		OnInteractionTargetChanged.Broadcast(CurrentTarget, CurrentRequest.PromptText);	//	广播
	}
}

void UPKInteractionComponent::CompleteInteraction()
{
	if (!bInteractionPressed && !CurrentTarget)
	{
		return;
	}

	AActor* CompletedTarget = CurrentTarget;
	APawn* Interactor = Cast<APawn>(GetOwner());

	bInteractionPressed = false;  //pressed = false
	bInteractionLocked = false;		//锁input = false
	RetainedProgress.Remove(CompletedTarget);	//从保留进度Map中移去
	ElapsedHoldTime = 0.0f;		//按压时间清零

	if (CompletedTarget)
	{
		IPKInteractable::Execute_OnInteractionCompleted(CompletedTarget, Interactor);	//调用目标的 OnInteractionCompleted()
	}

	OnInteractionCompleted.Broadcast(CompletedTarget);		//广播已完成
	OnInteractionProgressChanged.Broadcast(CompletedTarget, 1.0f);		//广播进度变化
}

void UPKInteractionComponent::BroadcastProgress()
{
	OnInteractionProgressChanged.Broadcast(CurrentTarget, GetNormalizedProgress());	//广播进度变化
}
