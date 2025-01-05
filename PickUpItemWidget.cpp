// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpItemWidget.h"
#include "Components/TextBlock.h"

void UPickUpItemWidget::NativeConstruct() {



    ItemName = nullptr;

}

void  UPickUpItemWidget::SetItemName(FString NewText)
{

    
    if (ItemName)
    {

        ItemName->SetText(FText::FromString("It's a " + NewText + " Read?"));

    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("NoteText no está inicializado"));
    }
}
