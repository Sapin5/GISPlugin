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
												.Image(this, &SPluginWindow::GetMyBrush)
												.DesiredSizeOverride(FVector2D(400.0f, 400.0f))
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

SPluginWindow::~SPluginWindow()
{
	if (PreviewTexture)
	{
		PreviewTexture->RemoveFromRoot();
	}
}

const FSlateBrush* SPluginWindow::GetMyBrush() const{

	// Fallback to avoid crashes
	if (DynamicBrush.IsValid()) {
		UE_LOG(LogTemp, Log, TEXT("Successfully loaded custom image"));
		return DynamicBrush.Get();
	}

	UE_LOG(LogTemp, Warning, TEXT("No brush loaded. Falling back on default!"));
	return FAppStyle::Get().GetBrush("Productivity.Info");
}


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

					
					if (PreviewTexture) {
						PreviewTexture->RemoveFromRoot();
						PreviewTexture = nullptr;
					}
					
					PreviewTexture = SPluginWindow::GeneratePreview(SelectedFilePath);
					
					if (PreviewTexture) {
						PreviewTexture->AddToRoot(); 
						DynamicBrush = MakeShared<FSlateDynamicImageBrush>(
							PreviewTexture,
							FVector2D(400.0f, 320.0f),
							FName(*SelectedFilePath)
						);
					}
					
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
}


void SPluginWindow::GenerateRaster() {

	Ex.ExecutionMode = EPythonCommandExecutionMode::ExecuteStatement;
	Ex.Command = TEXT("GIS_Data_Processor.hello()");

	if (!SPluginWindow::ErrorCheck(Ex)) return;
}


UTexture2D* SPluginWindow::GeneratePreview(FString& FilePath) {

	FString SafePath = FilePath.Replace(TEXT("\\"), TEXT("/"));

	Ex = FPythonCommandEx();
	Ex.ExecutionMode = EPythonCommandExecutionMode::EvaluateStatement;
	Ex.Command = FString::Format(TEXT("GIS_Data_Processor.lowResolutionPreview('{0}')"), { SafePath });
	
	if(!SPluginWindow::ErrorCheck(Ex)) return nullptr;

	FString TempPath = Ex.CommandResult.TrimStartAndEnd();
	TempPath = TempPath.Replace(TEXT("'"), TEXT("")).Replace(TEXT("\""), TEXT(""));
	UE_LOG(LogTemp, Log, TEXT("Python returned: '%s'"), *Ex.CommandResult);
	UE_LOG(LogTemp, Log, TEXT("Parsed TempPath: '%s'"), *TempPath);
	TArray<uint8> ImageBytes;
	if (!FFileHelper::LoadFileToArray(ImageBytes, *TempPath)) {
		UE_LOG(LogTemp, Error, TEXT("Failed to load temp file: %s"), *TempPath);
		return nullptr;
	}

	IFileManager::Get().Delete(*TempPath);

	UE_LOG(LogTemp, Warning, TEXT("Preview has been created"));
	return SPluginWindow::BytesToTexture(ImageBytes);
}


bool SPluginWindow::ErrorCheck(FPythonCommandEx& command) {
	/*
	* This is what executes commands. Probably will rename to better fit
	* its use
	*/
	if (!PythonPlugin->ExecPythonCommandEx(command))
	{
		UE_LOG(LogTemp, Error, TEXT("Python call failed: %s"), *command.CommandResult);
		return false;
	}
	return true;
}


UTexture2D* SPluginWindow::BytesToTexture(const TArray<uint8>& ImageBytes) {
	// Add this first:
	UE_LOG(LogTemp, Log, TEXT("BytesToTexture received %d bytes"), ImageBytes.Num());

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);

	if (!ImageWrapper.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create ImageWrapper"));
		return nullptr;
	}

	if (!ImageWrapper->SetCompressed(ImageBytes.GetData(), ImageBytes.Num()))
	{
		UE_LOG(LogTemp, Error, TEXT("SetCompressed failed - data may not be valid JPEG"));
		return nullptr;
	}

	TArray<uint8> UncompressedData;
	if (!ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedData))
	{
		UE_LOG(LogTemp, Error, TEXT("GetRaw failed"));
		return nullptr;
	}

	UE_LOG(LogTemp, Log, TEXT("Image size: %d x %d"), ImageWrapper->GetWidth(), ImageWrapper->GetHeight());

	UTexture2D* Texture = UTexture2D::CreateTransient(
		ImageWrapper->GetWidth(),
		ImageWrapper->GetHeight(),
		PF_B8G8R8A8
	);

	if (!Texture)
	{
		UE_LOG(LogTemp, Error, TEXT("CreateTransient failed"));
		return nullptr;
	}

	void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, UncompressedData.GetData(), UncompressedData.Num());
	Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
	Texture->UpdateResource();

	return Texture;
}