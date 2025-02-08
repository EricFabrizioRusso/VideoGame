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



	FString SavedLanguage;
	if (GConfig->GetString(TEXT("/Script/Engine.GameUserSettings"), TEXT("SelectedLanguage"),
		SavedLanguage, GGameUserSettingsIni))
	{
		UpdateTexts(SavedLanguage);
	}
	else
	{
		UpdateTexts("English"); // Por defecto
	}

   
}

void UItemsOptionsWidget::OnUseClicked() {

    UE_LOG(LogTemp, Warning, TEXT("Entra a OnUseClicked"));
    //PlayerCharacter->SetUseItem();
}

void UItemsOptionsWidget::OnDropClicked() {

    UE_LOG(LogTemp, Warning, TEXT("Entra a OnDropClicked"));

}




void UItemsOptionsWidget::UpdateTexts(FString Language) {



	if (Language == "Spanish") {

		UseText->SetText(FText::FromString("Usar"));
		DropText->SetText(FText::FromString("Tirar"));
		//EquipText->SetText(FText::FromString("Equipar"));




	}
	else {


		UseText->SetText(FText::FromString("Use"));
		DropText->SetText(FText::FromString("Drop"));
		//EquipText->SetText(FText::FromString("Equip"));



	}


}



