// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemsOptionsWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "OnDirt2Character.h"
#include "Blueprint/UserWidget.h" // Asegúrate de incluir este archivo


void UItemsOptionsWidget::NativeConstruct() {

	Super::NativeConstruct();

    if (UseBtn)
    {

        UE_LOG(LogTemp, Warning, TEXT("Constructor"));
        UseBtn->OnClicked.AddDynamic(this, &UItemsOptionsWidget::OnUseClicked);
    }

    if (DropBtn)
    {
        DropBtn->OnClicked.AddDynamic(this, &UItemsOptionsWidget::OnDropClicked);
    }


   
}

void UItemsOptionsWidget::OnUseClicked() {

    UE_LOG(LogTemp, Warning, TEXT("Entra a OnUseClicked"));
    //PlayerCharacter->SetUseItem();
}

void UItemsOptionsWidget::OnDropClicked() {

    UE_LOG(LogTemp, Warning, TEXT("Entra a OnDropClicked"));

}



