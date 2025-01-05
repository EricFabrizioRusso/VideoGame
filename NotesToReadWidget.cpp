// Fill out your copyright notice in the Description page of Project Settings.


#include "NotesToReadWidget.h"
#include "Components/TextBlock.h"

void UNotesToReadWidget::NativeConstruct() {

    /*if (NoteText)
    {
        NoteText->SetText(FText::FromString("Hello, World!"));
    }*/
    NoteText = nullptr;
}

void UNotesToReadWidget::SetNoteText(FString NewText)
{
    if (NoteText)
    {
        
        NoteText->SetText(FText::FromString(NewText));
     
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("NoteText no está inicializado"));
    }
}
