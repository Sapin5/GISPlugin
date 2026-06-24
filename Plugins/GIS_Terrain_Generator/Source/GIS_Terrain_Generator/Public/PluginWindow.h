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

	virtual ~SPluginWindow();
private:

	TSharedRef<SWidget> PreviewPage();
	TSharedRef<SWidget> GISMapPage();
	
	TSharedPtr<FSlateDynamicImageBrush> DynamicBrush;

	TSharedPtr<SWidgetSwitcher> WidgetSwitcher;
	int8 ActiveWidgetIndex{ 0 };

	const FSlateBrush* DefaultBrush;
	const FSlateBrush* GetMyBrush() const;

	FReply SelectFile();
	FReply ConfirmFile();
	FReply ClearBrush();
	FReply GoToPreviewPage();

	FString SelectedFilePath;
	
	UTexture2D* BytesToTexture(const TArray<uint8>& ImageBytes);
	UTexture2D* PreviewTexture{ nullptr };
	UTexture2D* GeneratePreview(FString&);

	IPythonScriptPlugin* PythonPlugin;
	
	FPythonCommandEx Ex;

	void LoadPythonFile();
	void CopyFile(FString&);
	void GenerateRaster(FString&);

	bool ErrorCheck(FPythonCommandEx&);
	bool EnableConfirm{ false };
	bool IsFileSelected() const;
};
