// Copyright Epic Games, Inc. All Rights Reserved.

#include "GIS_Terrain_GeneratorCommands.h"

#define LOCTEXT_NAMESPACE "FGIS_Terrain_GeneratorModule"

void FGIS_Terrain_GeneratorCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "GIS_Terrain_Generator", "Bring up GIS_Terrain_Generator window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
