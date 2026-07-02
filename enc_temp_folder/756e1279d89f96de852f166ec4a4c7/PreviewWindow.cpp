// Fill out your copyright notice in the Description page of Project Settings.


#include "PreviewWindow.h"

SPreviewWindow::SPreviewWindow()
{
}

SPreviewWindow::~SPreviewWindow()
{

}
FReply SPreviewWindow::ConfirmFile() {
	UE_LOG(LogTemp, Error, TEXT("file logged, this is red to stand out. not an error"))
		return FReply::Handled();
}