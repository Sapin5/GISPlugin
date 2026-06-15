// Fill out your copyright notice in the Description page of Project Settings.


#include "PluginWindow.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"
#include "Misc/Paths.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Engine/Texture2D.h"

void SPluginWindow::Construct(const FArguments& InArgs)
{
	SPluginWindow::LoadPythonFile();
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
				// Heading slot
				+ SVerticalBox::Slot().AutoHeight().Padding(10.0f)
				[
					SNew(STextBlock)
						.Text(INVTEXT("Terrain Generation"))
						.Font(FCoreStyle::Get().GetFontStyle("HeadingMain"))
				]

				// Section with a border detail
				+ SVerticalBox::Slot().AutoHeight().Padding(2.0f)
				[
					SNew(SBorder)
						.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
						.Padding(4.0f)
						[
							SNew(SVerticalBox)
								+ SVerticalBox::Slot().AutoHeight().Padding(3.0f, 1.0f)
								[
									SNew(STextBlock)
										.Text(INVTEXT("GeoTIFF preview"))
										.Font(FCoreStyle::Get().GetFontStyle("HeadingMain"))
								]
								+ SVerticalBox::Slot().AutoHeight().Padding(3.0f, 1.0f)
								[
									SNew(SHorizontalBox)
										+ SHorizontalBox::Slot().AutoWidth().Padding(2.0f).VAlign(VAlign_Top)
										[
											SNew(SButton)
												.Text(INVTEXT("Select File"))
												.OnClicked(this, &SPluginWindow::OnButtonClicked)
										]
										+ SHorizontalBox::Slot().AutoWidth().Padding(10.0f)
										[
											SNew(SImage)
												.Image(BrushToDisplay).DesiredSizeOverride(FVector2D(400.0f, 320.0f))
										]
								]
						]
				]
		];


	// Commented out while I work out how to better use slate
	/*
	SNew(SComboButton)
	.ButtonContent()
	[
		SNew(STextBlock)
			.Text(INVTEXT("Create Blocking Volume"))
			.Font(FCoreStyle::Get().GetFontStyle("NormalText"))
	]
	.MenuContent()
	[
		SNew(STextBlock)
			.Text(INVTEXT("Menu items go here"))
	]
	*/
};


FReply SPluginWindow::OnButtonClicked() {
	void* NativeWinHandle = nullptr;
	TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
	// To Do: break this into more functions or into a seperate file
	if (ParentWindow.IsValid()) {
		TSharedPtr<FGenericWindow> NativeWindow = ParentWindow->GetNativeWindow();

		if (NativeWindow.IsValid()) {
			NativeWinHandle = NativeWindow->GetOSWindowHandle();
			IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

			if (DesktopPlatform) {
				TArray<FString> OutNames;
				bool BOpened = DesktopPlatform->OpenFileDialog(NativeWinHandle, TEXT("Open folder"), "d:\\", FString(""),
					TEXT("GeoTIFF |*.tiff; *.tif"), 0, OutNames);
					// Image File | *.png;| <- png files, dropping for now June 15th
				if (BOpened && OutNames.Num() > 0) {
					
					SelectedFilePath = OutNames[0];
					SPluginWindow::GeneratePreview(SelectedFilePath);
					// SPluginWindow::GenerateRaster();
					UE_LOG(LogTemp, Log, TEXT("Selected File: %s"), *SelectedFilePath)
				}
			}
		}
	}
	return FReply::Handled();
}


void SPluginWindow::CopyFile(FString& FilePath) {
	/*
	*  I can make copies of selected files using this.
	*  but its probably not a good idea since some files are upwards of 20gb
	*  Importing to unreal yould require additional work
	*/

	// Get file path name, file name
	FString DestFolder = FPaths::ProjectDir() / TEXT("Content/GIS/GeoTiff");
	FString FileName = FPaths::GetCleanFilename(FilePath);
	FString DestPath = DestFolder / FileName;

	// gets active platform file (Windows, Linux, etc) reference
	// Also abstracts over OS and pak layers
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	if (PlatformFile.CopyFile(*DestPath, *FilePath)) {
		UE_LOG(LogTemp, Log, TEXT("File copied to: %s"), *DestPath);
	} else {
		UE_LOG(LogTemp, Warning, TEXT("Failed to copy file to: %s"), *DestPath);
	}
}

void SPluginWindow::LoadPythonFile() {
	PythonPlugin = IPythonScriptPlugin::Get();
	FString FilePath = FPaths::ProjectPluginsDir() / TEXT("GIS_Terrain_Generator/Content/Python/GIS_Data_Processor.py");

	if (!PythonPlugin || !PythonPlugin->IsPythonAvailable()) {
		UE_LOG(LogTemp, Error, TEXT("Python Script Plugin is not available!"));
		return;
	}

	FPythonCommandEx Import;
	Import.ExecutionMode = EPythonCommandExecutionMode::ExecuteStatement;
	Import.Command = TEXT("import importlib, GIS_Data_Processor; importlib.reload(GIS_Data_Processor)");
	PythonPlugin->ExecPythonCommandEx(Import);

	Ex.ExecutionMode = EPythonCommandExecutionMode::ExecuteStatement;
}

void SPluginWindow::GenerateRaster() {
	Ex.Command = TEXT("GIS_Data_Processor.hello()");

	SPluginWindow::ErrorCheck(Ex);
}

void SPluginWindow::GeneratePreview(FString& FilePath) {
	Ex.Command = FString::Printf(TEXT("GIS_Data_Processor.lowResolutionPreview('%s')"), *FilePath);
	PythonPlugin->ExecPythonCommandEx(Ex);

	FString OutputStr;
	for (const FPythonLogOutputEntry& Entry : Ex.LogOutput)
	{
		OutputStr += Entry.Output + TEXT("\n");
	}
	UE_LOG(LogTemp, Log, TEXT("Heee HEEEE %s"), *OutputStr);
}

void SPluginWindow::ErrorCheck(FPythonCommandEx& command) {

	if (!PythonPlugin->ExecPythonCommandEx(command))
	{
		UE_LOG(LogTemp, Error, TEXT("Python call failed: %s"), *command.CommandResult);
	}
}

UTexture2D* BytesToTexture(const TArray<uint8>& ImageBytes) {
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
	
	if (!ImageWrapper.IsValid() || !ImageWrapper->SetCompressed(ImageBytes.GetData(), ImageBytes.Num()))
	{
		return nullptr;
	}

	TArray<uint8> UncompressedData;
	if (!ImageWrapper->GetRaw(ERGBFormat::Gray, 8, UncompressedData))
	{
		return nullptr;
	}

	UTexture2D* Texture = UTexture2D::CreateTransient(
		ImageWrapper->GetWidth(),
		ImageWrapper->GetHeight(),
		PF_G8
	);

	if (!Texture) return nullptr;

	void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, UncompressedData.GetData(), UncompressedData.Num());
	Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
	Texture->UpdateResource();

	return Texture;
}