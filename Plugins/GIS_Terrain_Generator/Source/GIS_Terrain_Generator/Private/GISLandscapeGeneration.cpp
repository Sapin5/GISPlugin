// Fill out your copyright notice in the Description page of Project Settings.

#include "GISLandscapeGeneration.h"

#include "ImageUtils.h" 
#include "ImageCore.h"    
#include "Misc/FileHelper.h"

void GISLandscapeGeneration::Construct(const FArguments& InArgs) {
	ChildSlot
		[
			SNew(SButton)
				.Text(INVTEXT("Generate Test Map")).HAlign(HAlign_Center)
				//.OnClicked(this, &GISLandscapeGeneration::ConfirmLandscapeGeneration)
		];
}

void GISLandscapeGeneration::GenerateLandscape() {

}

FReply GISLandscapeGeneration::ConfirmLandscapeGeneration()
{
	ALandscape* Landscape = GEditor->GetEditorWorldContext().World()->SpawnActor<ALandscape>();
    /*
    Landscape->Import(
        Landscape->GetLandscapeGuid(),
        0, 0, 1000, 1000,
        15,
        1,

        );
    // Your loaded 16-bit heightmap, one entry per vertex (Width * Height), row-major
    // TArray<uint16> HeightData = LoadHeightmapFromPNG16(FilePath, Width, Height);

    
    TMap<FGuid, TArray<uint16>> HeightmapDataPerLayers;
    HeightmapDataPerLayers.Add(FGuid(), HeightData); // FGuid() = base/default layer

    TMap<FGuid, TArray<FLandscapeImportLayerInfo>> MaterialLayerDataPerLayer;
    HeightmapDataPerLayers.Add(FGuid(), TArray<FLandscapeImportLayerInfo>());

    Landscape->Import(
        Landscape->GetLandscapeGuid(),
        0, 0, Width - 1, Height - 1,
        SectionsPerComponent,
        QuadsPerSection,
        HeightmapDataPerLayers,
        nullptr, // heightmap filename, can be NAME_None if data supplied directly
        MaterialLayerDataPerLayer,
        ELandscapeImportAlphamapType::Additive
    );
	*/

	UE_LOG(LogTemp, Log, TEXT("Button is working"));
	return FReply::Handled();
}


GISLandscapeGeneration::~GISLandscapeGeneration()
{
}
