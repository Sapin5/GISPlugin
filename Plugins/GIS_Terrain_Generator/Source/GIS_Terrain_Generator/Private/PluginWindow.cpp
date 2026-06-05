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
					SNew(SButton).Text(INVTEXT("I Will do something soon"))
				]
		];
};