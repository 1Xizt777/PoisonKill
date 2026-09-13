import os

import unreal


TABLE_SPECS = (
    (
        "DT_Poisons",
        "DT_Poisons.csv",
        "/Script/PoisonKill.PKPoisonDefinition",
    ),
    (
        "DT_Carriers",
        "DT_Carriers.csv",
        "/Script/PoisonKill.PKCarrierDefinition",
    ),
    (
        "DT_NPCs",
        "DT_NPCs.csv",
        "/Script/PoisonKill.PKNpcDefinition",
    ),
    (
        "DT_Recipes",
        "DT_Recipes.csv",
        "/Script/PoisonKill.PKRecipeDefinition",
    ),
)

DESTINATION_PATH = "/Game/PoisonKill/Data"


def import_data_table(asset_name, source_file, row_struct_path):
    row_struct = unreal.load_object(None, row_struct_path)
    if not row_struct:
        raise RuntimeError(
            "Could not load row struct {}. Compile the PoisonKill C++ module first.".format(
                row_struct_path
            )
        )

    if not os.path.isfile(source_file):
        raise FileNotFoundError("Source CSV not found: {}".format(source_file))

    factory = unreal.CSVImportFactory()
    settings = factory.automated_import_settings
    settings.import_type = unreal.CSVImportType.ECSV_DATA_TABLE
    settings.import_row_struct = row_struct

    task = unreal.AssetImportTask()
    task.filename = source_file
    task.destination_path = DESTINATION_PATH
    task.destination_name = asset_name
    task.factory = factory
    task.automated = True
    task.replace_existing = True
    task.save = True

    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    imported_paths = list(task.get_editor_property("imported_object_paths"))
    if not imported_paths:
        raise RuntimeError("Import produced no asset for {}".format(source_file))

    unreal.log(
        "Imported {} into {} ({} rows)".format(
            source_file,
            imported_paths[0],
            len(unreal.load_asset(imported_paths[0]).get_row_names()),
        )
    )


project_dir = unreal.Paths.project_dir()
source_dir = os.path.join(project_dir, "DataTables")

for asset_name, file_name, row_struct_path in TABLE_SPECS:
    import_data_table(
        asset_name,
        os.path.join(source_dir, file_name),
        row_struct_path,
    )

unreal.EditorAssetLibrary.save_directory(DESTINATION_PATH, only_if_is_dirty=True, recursive=True)
unreal.log("Gameplay DataTables imported and saved.")
