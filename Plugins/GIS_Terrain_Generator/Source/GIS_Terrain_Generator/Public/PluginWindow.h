// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Brushes/SlateDynamicImageBrush.h"
#include "DesktopPlatformModule.h"
#include "IPythonScriptPlugin.h"
#include "GISLandscapeGeneration.h"

#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Widgets/Layout/SScrollBox.h"

#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Views/STileView.h"
#include "Misc/Paths.h"
#include "Engine/Texture2D.h"

#include "ImageUtils.h"
#include "ImageCore.h"
#include "ImageCoreUtils.h"

#include "Slate/DeferredCleanupSlateBrush.h"

#include "Styling/StyleColors.h"

#include "Brushes/SlateImageBrush.h"

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

	TArray<TSharedPtr<FDeferredCleanupSlateBrush>> RasterBrush;
	TArray<UTexture2D*> RasterTextures;

	TSharedPtr<SWidgetSwitcher> WidgetSwitcher;
	int8 ActiveWidgetIndex{ 0 };

	const FSlateImageBrush* temp;

	const FSlateBrush* DefaultBrush;
	const FSlateBrush* GetMyBrush() const;
	const FSlateBrush* GetMyRasterBrush( int ) const;

	TArray<FString> FoundFiles;

	FReply SelectFile();
	FReply ConfirmFile();
	FReply ClearBrush();
	FReply GoToPreviewPage();

	FString SelectedFilePath;
	
	UTexture2D* BytesToTexture(const TArray<uint8>& ImageBytes);
	UTexture2D* PreviewTexture{ nullptr };
	UTexture2D* GeneratePreview(FString&);
	UTexture2D* OptimizeImage(FImage&);

	IPythonScriptPlugin* PythonPlugin;
	
	FPythonCommandEx Ex;

	void LoadPythonFile();
	void CopyFile(FString&);
	void GenerateRaster(FString&);
	void LoadRasterImages();

	bool EnableConfirm{ false };
	bool IsFileSelected() const;

	bool PollRasterGeneration();
	bool RasterDone{ false };

	UINT8 RasterCount{ 0 };
	bool RasterCountDone{ false };

	float RasterScaleX{ 64.0f };
	float RasterScaleY{ 64.0f };

	void SetRasterCount(int);
	TSharedPtr<SBox> SizeBox;

	TSharedPtr<SUniformGridPanel> RasterGridPanel;
	void BuildRasterGrid();
	void ClearRasterTextures();

	bool RunPythonCommand(FPythonCommandEx&, FString&, bool);
};
