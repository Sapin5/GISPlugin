// Copyright Epic Games, Inc. All Rights Reserved.

#include "GIS_Terrain_GeneratorStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FGIS_Terrain_GeneratorStyle::StyleInstance = nullptr;

void FGIS_Terrain_GeneratorStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FGIS_Terrain_GeneratorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FGIS_Terrain_GeneratorStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("GIS_Terrain_GeneratorStyle"));
	return StyleSetName;
}

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FGIS_Terrain_GeneratorStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("GIS_Terrain_GeneratorStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("GIS_Terrain_Generator")->GetBaseDir() / TEXT("Resources"));

	Style->Set("GIS_Terrain_Generator.OpenPluginWindow", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));

	return Style;
}

void FGIS_Terrain_GeneratorStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FGIS_Terrain_GeneratorStyle::Get()
{
	return *StyleInstance;
}
