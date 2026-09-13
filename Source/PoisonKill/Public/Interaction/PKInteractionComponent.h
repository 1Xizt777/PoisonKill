#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PKInteractionTypes.h"
#include "PKInteractionComponent.generated.h"

class AActor;
class APawn;
struct FPKInteractionRequest;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPKInteractionTargetChangedSignature , AActor*,Target , FText,PromptText);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPKInteractionProgressChangedSignature , AActor*,Target , float,NormalizedProgress);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPKInteractionFinishedSignature , AActor*,Target);


UCLASS(ClassGroup = (PK), meta = (BlueprintSpawnableComponent))
class POISONKILL_API UPKInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPKInteractionComponent();

	UPROPERTY(BlueprintAssignable, Category = "PK|Interaction")
	FPKInteractionTargetChangedSignature OnInteractionTargetChanged;	//目标变化：显示或隐藏 E 采集原料 提示

	UPROPERTY(BlueprintAssignable, Category = "PK|Interaction")
	FPKInteractionProgressChangedSignature OnInteractionProgressChanged;	//进度变化：更新进度条

	UPROPERTY(BlueprintAssignable, Category = "PK|Interaction")
	FPKInteractionFinishedSignature OnInteractionCompleted;		//完成：播放完成音效或刷新提示

	UPROPERTY(BlueprintAssignable, Category = "PK|Interaction")
	FPKInteractionFinishedSignature OnInteractionCanceled;		//取消：隐藏进度并恢复普通输入

	
	UFUNCTION(BlueprintCallable, Category = "PK|Interaction")
	bool BeginInteraction();		//开始交互

	UFUNCTION(BlueprintCallable, Category = "PK|Interaction")
	void EndInteraction(bool bForceCancel = false);		//取消交互：处理松手和Npc看到强制取消
	
	UFUNCTION(BlueprintCallable, Category = "PK|Interaction")
	void CancelInteraction();		//NPC 看见玩家导致施用中断

	
	//----Begin----//
	UFUNCTION(BlueprintPure, Category = "PK|Interaction")
	bool IsInteractionLocked() const;		

	UFUNCTION(BlueprintPure, Category = "PK|Interaction")
	bool IsInteractionPressed() const;

	UFUNCTION(BlueprintPure, Category = "PK|Interaction")
	AActor* GetCurrentTarget() const;

	UFUNCTION(BlueprintPure, Category = "PK|Interaction")
	float GetNormalizedProgress() const;

	const FPKInteractionRequest& GetCurrentRequest() const;
	//----End----//
	
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime,ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PK|Interaction", meta = (ClampMin = "1.0", Units = "cm"))
	float InteractionRange = 150.0f;		//搜索范围

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PK|Interaction", meta = (ClampMin = "0.01", Units = "s"))
	float TargetRefreshInterval = 0.1f;		//重新搜索交互间隔

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "PK|Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AActor> CurrentTarget;		//当前检测到、可以被交互的 Actor

	FPKInteractionRequest CurrentRequest;		//该 Actor 返回的提示文字、长按时间和交互规则
	
	float ElapsedHoldTime = 0.0f;		//当前已经按住的时间
	float TargetRefreshElapsed = 0.0f;		//距离上次搜索交互目标过去了多久
	bool bInteractionPressed = false;		//玩家当前是否按住交互键
	bool bInteractionLocked = false;		//交互期间是否需要锁定移动、视角和跳跃

	TMap<TWeakObjectPtr<AActor>, float> RetainedProgress;	//保留的进度

	void RefreshCurrentTarget();		//重新搜索交互Target函数
	void SetCurrentTarget(AActor* NewTarget, const FPKInteractionRequest& NewRequest);
	void CompleteInteraction();
	void BroadcastProgress();	//广播进度函数

	friend struct FPKInteractionComponentTestAccessor;
};
