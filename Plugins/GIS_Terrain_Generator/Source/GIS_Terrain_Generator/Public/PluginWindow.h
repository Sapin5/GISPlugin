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
	
	TSharedPtr<FSlateDynamicImageBrush> DynamicBrush;
	const FSlateBrush* DefaultBrush;
	FReply SelectFile();
	FReply ConfirmFile();
	FString SelectedFilePath;
	
	UTexture2D* BytesToTexture(const TArray<uint8>& ImageBytes);
	UTexture2D* PreviewTexture = nullptr;
	UTexture2D* GeneratePreview(FString&);

	IPythonScriptPlugin* PythonPlugin;
	
	const FSlateBrush* GetMyBrush() const;
	
	FPythonCommandEx Ex;

	void LoadPythonFile();
	void CopyFile(FString&);
	void GenerateRaster();
	bool ErrorCheck(FPythonCommandEx&);

	bool EnableConfirm;

public:
	virtual ~SPluginWindow();
};
