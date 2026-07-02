// Fill out your copyright notice in the Description page of Project Settings.

#include "GISLandscapeGeneration.h"

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
	//UE_LOG(LogTemp, Log, TEXT("Button is working"));
	return FReply::Handled();
}


GISLandscapeGeneration::~GISLandscapeGeneration()
{
}
