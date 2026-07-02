// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class GIS_TERRAIN_GENERATOR_API SPreviewWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPreviewWindow) {}
	SLATE_END_ARGS()

	SPreviewWindow();
	~SPreviewWindow();

	FReply ConfirmFile();

private:

};
