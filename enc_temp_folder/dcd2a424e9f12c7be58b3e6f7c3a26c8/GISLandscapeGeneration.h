// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LandscapeComponent.h"
#include "Widgets/SCompoundWidget.h"
#include "Brushes/SlateDynamicImageBrush.h"

#include "Widgets/Input/SButton.h"

/**
 * 
 */
class GIS_TERRAIN_GENERATOR_API GISLandscapeGeneration: public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(GISLandscapeGeneration) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual ~GISLandscapeGeneration();
private:

	// ULandscapeComponent Test;

	void GenerateLandscape();
	FReply ConfirmLandscapeGeneration();
};
