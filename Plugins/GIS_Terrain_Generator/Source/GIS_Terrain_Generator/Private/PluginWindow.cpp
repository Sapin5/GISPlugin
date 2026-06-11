// Fill out your copyright notice in the Description page of Project Settings.


#include "PluginWindow.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"
#include "Misc/Paths.h"


void SPluginWindow::Construct(const FArguments& InArgs)
{
	FString ImagePath = FPaths::ProjectPluginsDir() / TEXT("GIS_Terrain_Generator/Resources/TiledMap/PlaceHolder.png");

	// Fallback to avoid crashes
	const FSlateBrush* BrushToDisplay = FAppStyle::Get().GetBrush("Productivity.Info");

	// FIX: Removed the local declaration line that was here!

	if (FPaths::FileExists(ImagePath)) {
		// Uses the header file variable, keeping it alive beyond this function
		DynamicBrush = MakeShareable(new FSlateDynamicImageBrush(FName(*ImagePath), FVector2D(400.0f, 320.0f)));
		BrushToDisplay = DynamicBrush.Get();
		UE_LOG(LogTemp, Log, TEXT("Successfully loaded custom image from: %s"), *ImagePath);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Custom Brush not found at %s. Falling back on default!"), *ImagePath);
	}

	ChildSlot
		[
			SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight().Padding(10.0f)
				[
					SNew(STextBlock).Text(INVTEXT("This will work eventually :D"))
						.Font(FCoreStyle::Get().GetFontStyle("HeadingMain"))
				]

				+ SVerticalBox::Slot().AutoHeight().Padding(10.0f)
				[
					SNew(SImage)
						.Image(BrushToDisplay).DesiredSizeOverride(FVector2D(400.0f, 320.0f))
				]

				+ SVerticalBox::Slot().AutoHeight().Padding(10.0f)
				[
					SNew(SButton)
						.Text(INVTEXT("I Will do something soon"))
						.OnClicked(this, &SPluginWindow::OnButtonClicked)
				]
		];
};


FReply SPluginWindow::OnButtonClicked() {
	void* NativeWinHandle = nullptr;
	TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
	SPluginWindow::execute();

	// To Do: break this into more functions or into a seperate file
	if (ParentWindow.IsValid()) {
		TSharedPtr<FGenericWindow> NativeWindow = ParentWindow->GetNativeWindow();

		if (NativeWindow.IsValid()) {
			NativeWinHandle = NativeWindow->GetOSWindowHandle();
			IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

			if (DesktopPlatform) {
				TArray<FString> OutNames;
				bool BOpened = DesktopPlatform->OpenFileDialog(NativeWinHandle, TEXT("Open folder"), "d:\\", FString(""),
					TEXT("Image file | *.png; |GeoTIFF |*.tiff; *.tif"), 0, OutNames);

				if (BOpened && OutNames.Num() > 0) {
					/*
					SelectedFilePath = OutNames[0];
					UE_LOG(LogTemp, Log, TEXT("Selected File: %s"), *SelectedFilePath)

					FString DestFolder = FPaths::ProjectDir() / TEXT("Content/GIS/GeoTiff");
					FString FileName = FPaths::GetCleanFilename(SelectedFilePath);
					FString DestPath = DestFolder / FileName;

					IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

					if (PlatformFile.CopyFile(*DestPath, *SelectedFilePath)) {
						UE_LOG(LogTemp, Log, TEXT("File copied to: %s"), *DestPath);
					} else {
						UE_LOG(LogTemp, Warning, TEXT("Failed to copy file to: %s"), *DestPath);
					}
					*/
				}
			}
		}
	}
	return FReply::Handled();
}

void SPluginWindow::execute() {
	IPythonScriptPlugin* PythonPlugin = IPythonScriptPlugin::Get();
	FString FilePath = FPaths::ProjectPluginsDir() / TEXT("GIS_Terrain_Generator/Content/Python/GIS_Data_Processor.py");

	if (!PythonPlugin || !PythonPlugin->IsPythonAvailable()) {
		UE_LOG(LogTemp, Error, TEXT("Python Script Plugin is not available!"));
		return;
	}

	FPythonCommandEx Import;
	Import.ExecutionMode = EPythonCommandExecutionMode::ExecuteStatement;
	Import.Command = TEXT("import importlib, GIS_Data_Processor; importlib.reload(GIS_Data_Processor)");
	PythonPlugin->ExecPythonCommandEx(Import);

	FPythonCommandEx Ex;
	Ex.ExecutionMode = EPythonCommandExecutionMode::ExecuteStatement;
	Ex.Command = TEXT("GIS_Data_Processor.hello()");

	if (!PythonPlugin->ExecPythonCommandEx(Ex))
	{
		UE_LOG(LogTemp, Error, TEXT("Python call failed: %s"), *Ex.CommandResult);
	}
}