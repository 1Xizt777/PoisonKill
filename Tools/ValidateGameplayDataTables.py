import unreal


TABLE_PATH = "/Game/PoisonKill/Data/{}"

TABLES = {
    "DT_Poisons": (
        "DisplayName",
        "SingleDose",
        "LethalThreshold",
        "IncubationTime",
        "VisualSignificance",
        "AllowedCarrierTypes",
    ),
    "DT_Carriers": (
        "DisplayName",
        "CarrierType",
        "bConcealsVisuals",
        "DoseCoefficient",
        "DefaultResidueHits",
    ),
    "DT_NPCs": (
        "DisplayName",
        "ThresholdCoefficient",
        "WarningThresholdRatio",
        "WarningMoveSpeedScale",
    ),
    "DT_Recipes": (
        "DisplayName",
        "InputItemId",
        "RequiredInputCount",
        "OutputPoisonId",
        "ProcessingDuration",
    ),
}


for table_name, columns in TABLES.items():
    table = unreal.load_asset(TABLE_PATH.format(table_name))
    if not table:
        raise RuntimeError("Could not load {}".format(table_name))

    unreal.log("TABLE {} rows={}".format(table_name, list(table.get_row_names())))
    for column in columns:
        values = unreal.DataTableFunctionLibrary.get_data_table_column_as_string(
            table, column
        )
        unreal.log("  {}={}".format(column, list(values)))

unreal.log("Gameplay DataTable values dumped successfully.")
