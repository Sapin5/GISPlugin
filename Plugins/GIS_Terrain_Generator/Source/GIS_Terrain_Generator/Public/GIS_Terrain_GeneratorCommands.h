// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Framework/Commands/Commands.h"
#include "GIS_Terrain_GeneratorStyle.h"

class FGIS_Terrain_GeneratorCommands : public TCommands<FGIS_Terrain_GeneratorCommands>
{
public:

	FGIS_Terrain_GeneratorCommands()
		: TCommands<FGIS_Terrain_GeneratorCommands>(TEXT("GIS_Terrain_Generator"), NSLOCTEXT("Contexts", "GIS_Terrain_Generator", "GIS_Terrain_Generator Plugin"), NAME_None, FGIS_Terrain_GeneratorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};