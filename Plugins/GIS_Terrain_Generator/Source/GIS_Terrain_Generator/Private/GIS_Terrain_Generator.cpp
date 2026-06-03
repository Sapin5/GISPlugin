// Copyright Epic Games, Inc. All Rights Reserved.

#include "GIS_Terrain_Generator.h"
#include "GIS_Terrain_GeneratorStyle.h"
#include "GIS_Terrain_GeneratorCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "Tester.h"

static const FName GIS_Terrain_GeneratorTabName("GIS_Terrain_Generator");

#define LOCTEXT_NAMESPACE "FGIS_Terrain_GeneratorModule"

void FGIS_Terrain_GeneratorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FGIS_Terrain_GeneratorStyle::Initialize();
	FGIS_Terrain_GeneratorStyle::ReloadTextures();

	FGIS_Terrain_GeneratorCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FGIS_Terrain_GeneratorCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FGIS_Terrain_GeneratorModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FGIS_Terrain_GeneratorModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(GIS_Terrain_GeneratorTabName, FOnSpawnTab::CreateRaw(this, &FGIS_Terrain_GeneratorModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FGIS_Terrain_GeneratorTabTitle", "GIS_Terrain_Generator"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FGIS_Terrain_GeneratorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FGIS_Terrain_GeneratorStyle::Shutdown();

	FGIS_Terrain_GeneratorCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(GIS_Terrain_GeneratorTabName);
}

TSharedRef<SDockTab> FGIS_Terrain_GeneratorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FGIS_Terrain_GeneratorModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("GIS_Terrain_Generator.cpp"))
		);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(WidgetText)
			]
		];
}

void FGIS_Terrain_GeneratorModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(GIS_Terrain_GeneratorTabName);
}

void FGIS_Terrain_GeneratorModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FGIS_Terrain_GeneratorCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FGIS_Terrain_GeneratorCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGIS_Terrain_GeneratorModule, GIS_Terrain_Generator)