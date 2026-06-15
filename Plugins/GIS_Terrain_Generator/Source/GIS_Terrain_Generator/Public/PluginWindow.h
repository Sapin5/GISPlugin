// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Brushes/SlateDynamicImageBrush.h"
#include "DesktopPlatformModule.h"
#include "IPythonScriptPlugin.h"
/**
 *
 */
class GIS_TERRAIN_GENERATOR_API SPluginWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPluginWindow) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
private:
	TSharedPtr<FSlateImageBrush> CustomBrush;
	TSharedPtr<FSlateDynamicImageBrush> DynamicBrush;
	FReply OnButtonClicked();
	FString SelectedFilePath;

	IPythonScriptPlugin* PythonPlugin;
	FPythonCommandEx Ex;

	void LoadPythonFile();
	void CopyFile(FString&);
	void GenerateRaster();
	void GeneratePreview(FString&);
	void ErrorCheck(FPythonCommandEx&);
};
