// Fill out your copyright notice in the Description page of Project Settings.


#include "PluginWindow.h"

#include "RenderingThread.h"
#include "Async/Async.h"

// I dont like unreal
void SPluginWindow::Construct(const FArguments& InArgs)
{
	SPluginWindow::LoadPythonFile();
	DefaultBrush = FAppStyle::Get().GetBrush("Productivity.Info");

	ChildSlot
		[
			SAssignNew(WidgetSwitcher, SWidgetSwitcher)
				+SWidgetSwitcher::Slot()
				[
					SPluginWindow::PreviewPage()
				]
				+ SWidgetSwitcher::Slot()
				[
					SPluginWindow::GISMapPage()
				]
		];


	// Combo button could be useful later
	// This section si for slate stuff I deleted butt could be usefull later
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

TSharedRef<SWidget> SPluginWindow::PreviewPage() {

	return SNew(SVerticalBox)
		// Heading slot
		+ SVerticalBox::Slot().AutoHeight().Padding(10.0f).HAlign(HAlign_Center)
		[
			SNew(STextBlock).Text(INVTEXT("Terrain Generation"))
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
						+ SVerticalBox::Slot().AutoHeight().Padding(4.0f)
						[
							SNew(SHorizontalBox)
								+ SHorizontalBox::Slot().AutoWidth().Padding(10.0f)
								[
									SNew(SVerticalBox)
										+ SVerticalBox::Slot().AutoHeight().Padding(10.0f)
										[
											SNew(SButton)
												.Text(INVTEXT("Select File")).HAlign(HAlign_Center)
												.OnClicked(this, &SPluginWindow::SelectFile)
										]
										+ SVerticalBox::Slot()
										[
											SNew(SSpacer).Size(FVector2D(0, 20))
										]
										+ SVerticalBox::Slot().AutoHeight().Padding(10.0f)
										[
											SNew(SButton)
												.Text(INVTEXT("Clear")).HAlign(HAlign_Center)
												.IsEnabled(this, &SPluginWindow::IsFileSelected)
												.OnClicked(this, &SPluginWindow::ClearBrush)
										]
										+ SVerticalBox::Slot().AutoHeight().Padding(10.0f)
										[
											SNew(SButton)
												.Text(INVTEXT("Confirm")).HAlign(HAlign_Center)
												.IsEnabled(this, &SPluginWindow::IsFileSelected)
												.OnClicked(this, &SPluginWindow::ConfirmFile)
										]
								]
							+ SHorizontalBox::Slot().AutoWidth().Padding(4.0f)
								[
									SNew(SVerticalBox)
										+ SVerticalBox::Slot().AutoHeight().Padding(10.0f)
										[
											SNew(STextBlock).Text(INVTEXT("GeoTIFF Preview"))
										]
										+ SVerticalBox::Slot().AutoHeight().Padding(10.0f)
										[
											SNew(SImage)
												.Image(this, &SPluginWindow::GetMyBrush)
												.DesiredSizeOverride(FVector2D(400.0f, 400.0f))
										]
								]
						]
				]
		];
}



TSharedRef<SWidget> SPluginWindow::GISMapPage()
{
	return SNew(SVerticalBox)
		+ SVerticalBox::Slot().AutoHeight().Padding(10.0f).HAlign(HAlign_Center)
		[
			SNew(STextBlock).Text(INVTEXT("Generating High Resolution Map"))
				.Font(FCoreStyle::Get().GetFontStyle("HeadingMain"))
		]
		+ SVerticalBox::Slot().AutoHeight().Padding(10.0f).HAlign(HAlign_Center)
		[
			SNew(STextBlock).Text_Lambda([this]()
				{
					return SPluginWindow::PollRasterGeneration() ? INVTEXT("Done") : INVTEXT("Running...");
				})
		]
		+ SVerticalBox::Slot().AutoHeight().Padding(10.0f).HAlign(HAlign_Center)
		[
			SNew(SButton)
				.Text(INVTEXT("Go Back")).HAlign(HAlign_Center)
				.OnClicked(this, &SPluginWindow::GoToPreviewPage)
		]
		+ SVerticalBox::Slot().AutoHeight().Padding(10.0f).HAlign(HAlign_Center)
		[
			// This doesnt do anything yet and is here for testing stuff
			// I need to clean up this file and this was a quick test
			// Will likely break up more
			SNew(GISLandscapeGeneration)
		]
		+ SVerticalBox::Slot().Padding(10.0f)
		[
			SAssignNew(SizeBox, SBox)
				// So, the SBox is needed because we need to override the 
				.HeightOverride(RasterCount * 64 + RasterCount * 4.0f)
				.WidthOverride(RasterCount * 64 + RasterCount * 4.0f)
				[
					SAssignNew(RasterGridPanel, SUniformGridPanel)
				]
		];
		
}



void SPluginWindow::BuildRasterGrid()
{
	if (!RasterGridPanel.IsValid())
	{
		return;
	}

	RasterGridPanel->ClearChildren();

	for (int32 i = 0; i < RasterCount * RasterCount; ++i)
	{
		const int32 Row = i / RasterCount;
		const int32 Col = i % RasterCount;
		const int32 Index = i;

		RasterGridPanel->AddSlot(Col, Row)
			[
				SNew(SBox)
					.WidthOverride(64)
					.HeightOverride(64)
					[
						SNew(SImage)
							.Image_Lambda([this, Index]() -> const FSlateBrush*
								{
									return GetMyRasterBrush(Index);
								})
					]
			];
	}
	
}


void SPluginWindow::SetRasterCount(int NewCount)
{
	RasterCount = NewCount;

	const float NewSize = RasterCount * 64 + RasterCount * 4.0f;
	SizeBox->SetWidthOverride(NewSize);
	SizeBox->SetHeightOverride(NewSize);
	AsyncTask(ENamedThreads::GameThread, [this]() { SPluginWindow::BuildRasterGrid(); });
}



TSharedRef<ITableRow> SPluginWindow::OnGenerateTile(TSharedPtr<int32> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	int32 Index = Item.IsValid() ? *Item : 0;

	return SNew(STableRow<TSharedPtr<int32>>, OwnerTable)
		[	
			SNew(SImage)
				.Image_Lambda([this, Index]() -> const FSlateBrush*
					{
						return GetMyRasterBrush(Index);
					})		
		];
}



void SPluginWindow::LoadRasterImages() {

	SPluginWindow::ClearRasterTextures();
	FString RasterFolder = FPaths::ProjectDir() / TEXT("Plugins/GIS_Terrain_Generator/Content/raster_segments_low");

	FString	FilePath;
	FImage OutImage;
	for (int i = 0; i < FoundFiles.Num(); i++) {
		UTexture2D* Texture{ nullptr };
		FilePath = RasterFolder / FoundFiles[i];

		if (FImageUtils::LoadImage(*FilePath, OutImage)) {
			Texture = SPluginWindow::OptimizeImage(OutImage);
		}

		if (Texture)
		{
			Texture->AddToRoot();
			RasterTextures.Add(Texture);
			RasterBrush.Add(FDeferredCleanupSlateBrush::CreateBrush(Texture, FVector2D(64.0f, 64.0f)));
		}
		else
		{
			RasterBrush.Add(nullptr);
			RasterTextures.Add(nullptr);
		}
	}
}

void SPluginWindow::ClearRasterTextures() {

	for (UTexture2D* Texture : RasterTextures) {
		if (Texture) {

			Texture->RemoveFromRoot();
		}
	}

	RasterTextures.Empty();

	RasterBrush.Empty();

}

const FSlateBrush* SPluginWindow::GetMyRasterBrush(int Index) const {
	/*
	* Gets texture for brush
	* Has a fallback when there is no brush available
	*/

	if (RasterBrush.IsValidIndex(Index) && RasterBrush[Index].IsValid()) {
		return RasterBrush[Index]->GetSlateBrush();
	}

	UE_LOG(LogTemp, Warning, TEXT("GetMyRasterBrush: invalid brush at index %d"), Index);
	return DefaultBrush;
}



bool SPluginWindow::PollRasterGeneration() {

	if (RasterDone) {
		if(!RasterCountDone)
		{
			FString RasterFolder = FPaths::ProjectDir() / TEXT("Plugins/GIS_Terrain_Generator/Content/raster_segments_low");
			FString FilterPath = RasterFolder / TEXT("*");

			IFileManager::Get().FindFiles(FoundFiles, *FilterPath, true, false);

			FoundFiles.Sort([](const FString& A, const FString& B) { return FCString::Atoi(*A) < FCString::Atoi(*B); });

			SPluginWindow::LoadRasterImages();

			RasterCount = (int32)FMath::Sqrt((float)FoundFiles.Num());

			RasterCountDone = true;

			SPluginWindow::SetRasterCount(RasterCount);

			// AsyncTask(ENamedThreads::GameThread, [this]() { SPluginWindow::BuildRasterGrid(); });
			if (RasterGridPanel.IsValid())
			{
				RasterGridPanel->Invalidate(EInvalidateWidget::Layout);
			}
		}
		return true;
	}


	FPythonCommandEx PollRaster;
	FString PollArgs = TEXT("SubProcessFile.poll()");
	// FString::Format(TEXT("SubProcessFile.poll()"), { "" });
	SPluginWindow::RunPythonCommand(PollRaster, PollArgs, true);

	RasterDone = (PollRaster.CommandResult == "True");
	return RasterDone;
};



FReply SPluginWindow::GoToPreviewPage()
{
	WidgetSwitcher->SetActiveWidgetIndex(0);
	return FReply::Handled();
}



const FSlateBrush* SPluginWindow::GetMyBrush() const{
	/*
	* Gets texture for brush
	* Has a fallback when there is no brush available
	*/
	if (DynamicBrush.IsValid()) {
		// If texture exists for brush, loads it and returns it
		return DynamicBrush.Get();
	}

	return DefaultBrush;
}



FReply SPluginWindow::SelectFile() {
	/*
	* Handles button click
	*/

	// Generic pointer type, Gets converted later
	void* NativeWinHandle = nullptr;

	// Gets the current active top level window (focused window)
	TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().GetActiveTopLevelWindow();

	// To Do: break this into more functions or into a seperate file
	if (ParentWindow.IsValid()) {

		// Gets the OS/platform specific window handle (win32, linux, etc) 
		TSharedPtr<FGenericWindow> NativeWindow = ParentWindow->GetNativeWindow();
		if (NativeWindow.IsValid()) {

			// Gets the raw, platform-specific OS handle (HWND on Windows, NSWindow* on macOS)
			NativeWinHandle = NativeWindow->GetOSWindowHandle();

			// Accesses the desktop platform interface to handle native OS file/folder pickers
			IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

			if (DesktopPlatform) {

				// // Array to store the paths/names of the files selected
				TArray<FString> OutNames;

				// Opens native file explorer and filters for only .tiff, .tif files for selection
				// Stores selected files/folder in outnames
				bool BOpened = DesktopPlatform->OpenFileDialog(NativeWinHandle, TEXT("Open folder"), "d:\\", FString(""),
					TEXT("GeoTIFF |*.tiff; *.tif"), 0, OutNames);
				// Image File | *.png;| <- png files, dropping for now June 15th

				if (BOpened && OutNames.Num() > 0) {

					// Gets first item in array
					SelectedFilePath = OutNames[0];

					// Removes current texture if it exists
					// replaces it with null pointer
					if (PreviewTexture) {
						PreviewTexture->RemoveFromRoot();
						PreviewTexture = nullptr;
					}

					// Sets new texture created from GeoTIFF
					PreviewTexture = SPluginWindow::GeneratePreview(SelectedFilePath);

					// Adds the texture to root
					if (PreviewTexture) {
						PreviewTexture->AddToRoot();

						// Creates brush to display in widget
						DynamicBrush = MakeShared<FSlateDynamicImageBrush>(
							PreviewTexture,
							FVector2D(400.0f, 400.0f),
							FName(*SelectedFilePath)
						);

						EnableConfirm = true;
					}

					UE_LOG(LogTemp, Log, TEXT("Selected File: %s"), *SelectedFilePath)
	}	}	}	} // I will not be undoing this, it looks funny
	return FReply::Handled();
}



FReply SPluginWindow::ConfirmFile() {
	/*
	  Change widget to highlight and select screen 
	*/
	WidgetSwitcher->SetActiveWidgetIndex(1);
	SPluginWindow::GenerateRaster(SelectedFilePath);

	// UE_LOG(LogTemp, Error, TEXT("file logged, this is red to stand out. not an error"))
	return FReply::Handled();
}



FReply SPluginWindow::ClearBrush() {

	// Reset brush to blank
	if (DynamicBrush.IsValid())
	{
		DynamicBrush.Reset();
	}

	EnableConfirm = false;

	return FReply::Handled();
}



bool SPluginWindow::IsFileSelected() const{
	// So unreal is dumb and I have to do this to update button states in slate
// I want to cry
	return EnableConfirm;
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
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Failed to copy file to: %s"), *DestPath);
	}
}



void SPluginWindow::LoadPythonFile() {
	/*
	* Creates instance of python script plugin
	* Defines path to script and loads it
	*/
	PythonPlugin = IPythonScriptPlugin::Get();
	FString FilePath = FPaths::ProjectPluginsDir() / TEXT("GIS_Terrain_Generator/Content/Python/GIS_Data_Processor.py");

	if (!PythonPlugin || !PythonPlugin->IsPythonAvailable()) {
		UE_LOG(LogTemp, Error, TEXT("Python Script Plugin is not available!"));
		return;
	}

	// Imports files
	FPythonCommandEx ImportGIS;
	FString ImportArgs = TEXT("import importlib, GIS_Data_Processor; importlib.reload(GIS_Data_Processor)");
	FPythonCommandEx ImportSubP;
	FString SubProcessingArgs = TEXT("import importlib, SubProcessFile; importlib.reload(SubProcessFile)");

	if (!SPluginWindow::RunPythonCommand(ImportGIS, ImportArgs, false)) return;
	if (!SPluginWindow::RunPythonCommand(ImportSubP, SubProcessingArgs, false)) return;
}



void SPluginWindow::GenerateRaster(FString& FilePath) {
	/*
	* Generates raster segments of full GeoTiff
	* This is needed since unreal cannot always process raw GeoTIFFS
	* Currentl not actually running or doing anything as file paths need to be reworked
	* created .png files are added directly to folders which bypasses unreals import
	* which unreal does not like. This is gonna be a headache
	*/

	// prepares command for execution
	FPythonCommandEx CreateRaster;
	FString args = FString::Format(TEXT("SubProcessFile.main('{0}')"), { FilePath });
	if (!SPluginWindow::RunPythonCommand(CreateRaster, args, true)) return;
}



UTexture2D* SPluginWindow::GeneratePreview(FString& FilePath) {

	// Theres a slight chance that if the file path contains ../n/..
	// python will read the filepath and make a newline this prevents that
	FPythonCommandEx CreatePreview;
	FString args = FString::Format(TEXT("GIS_Data_Processor.lowResolutionPreview('{0}')"), { FilePath });
	if (!SPluginWindow::RunPythonCommand(CreatePreview, args, true)) return nullptr;

	// function returns string, this will remove whitespace
	FString TempPath = CreatePreview.CommandResult.TrimStartAndEnd();
	// Python can use single quotes for strings, this is just gonna remove that
	TempPath = TempPath.Replace(TEXT("'"), TEXT("")).Replace(TEXT("\""), TEXT(""));

	// Array to hold the raw binary data of the image
	TArray<uint8> ImageBytes;

	// Read the temporary file's raw bytes into the array
	if (!FFileHelper::LoadFileToArray(ImageBytes, *TempPath)) {
		UE_LOG(LogTemp, Error, TEXT("Failed to load temp file: %s"), *TempPath);
		return nullptr;
	}

	// Deletes tempfile that was created by python script
	IFileManager::Get().Delete(*TempPath);

	UE_LOG(LogTemp, Warning, TEXT("Preview has been created"));

	// Creates and returns Texture2D created from bytes
	return SPluginWindow::BytesToTexture(ImageBytes);
}



bool SPluginWindow::RunPythonCommand(FPythonCommandEx& Execution, FString& argument, bool ChangeMode) {
	/*
	* This is what executes commands
	*/
    argument = argument.Replace(TEXT("\\"), TEXT("/"));
	// Execution = FPythonCommandEx();
	UE_LOG(LogTemp, Error, TEXT("Here"));
	// Change execution mode to not print output or return value
	if (ChangeMode) {
		Execution.ExecutionMode = EPythonCommandExecutionMode::EvaluateStatement;
	} else {
		Execution.ExecutionMode = EPythonCommandExecutionMode::ExecuteStatement;
	}
	Execution.Command = argument;

	if (!PythonPlugin->ExecPythonCommandEx(Execution)) // <- this both executes the command and checks if it failed
	{
		UE_LOG(LogTemp, Error, TEXT("Python call failed: %s"), *Execution.CommandResult);
		return false;
	}
	return true;
}



UTexture2D* SPluginWindow::BytesToTexture(const TArray<uint8>& ImageBytes) {
	/*
	* Creates a Texture2D from bytes
	* NOTE - If you ask claude, gemini, or gpt they will tell you to use an image wrapper
	* Unreals own documentation says dont use an image wrapper
	* Image wrapper is also complicated to set up
	*/

	FImage OutImage;

	// Create FImage from bytes, for some reason this is in the wrong colour channels
	if (FImageUtils::DecompressImage(ImageBytes.GetData(), ImageBytes.Num(), OutImage)) {
		return SPluginWindow::OptimizeImage(OutImage);
	}

	return nullptr;
}



UTexture2D* SPluginWindow::OptimizeImage(FImage& OutImage) {
	
	// Force colour channel to valid usable one in unreal
	// For some reason unreal uses BRGA instead of RGBA?????
	UTexture2D* Texture{ nullptr };

	if ((int32)OutImage.GammaSpace == 0) {
		// Some images passed through have a gammaspace of 0.
		// If we set the gammaspace again like in the else
		// the image will just get "brighter" and the colour will be off
		OutImage.ChangeFormat(ERawImageFormat::BGRA8, OutImage.GammaSpace);
		Texture = FImageUtils::CreateTexture2DFromImage(OutImage);
		return Texture;
	}
	else {
		OutImage.ChangeFormat(ERawImageFormat::BGRA8, EGammaSpace::sRGB);
		Texture = FImageUtils::CreateTexture2DFromImage(OutImage);
		/*
		Optimize the texture specifically for Editor UI rendering
		This actually doesnt do anything major, and flushing rendering commands was causing issues
		Uncomment if needed

		Texture->CompressionSettings = TC_EditorIcon;
		Texture->LODGroup = TEXTUREGROUP_UI;
		Texture->NeverStream = true;
		Texture->UpdateResource();
		FlushRenderingCommands();
		*/
		return Texture;
	}
}



SPluginWindow::~SPluginWindow() {
	// remove textures from root
	if (PreviewTexture) PreviewTexture->RemoveFromRoot();
	SPluginWindow::ClearRasterTextures();
}