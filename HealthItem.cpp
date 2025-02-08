// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthItem.h"

AHealthItem::AHealthItem()
{
    RestoreLife = 50.0f; // Valor predeterminado


    ItemData.ItemName = FName("Healthy Drink");
    ItemData.ItemDescription = "Restores 50% of your life";
    ItemData.ItemIcon = nullptr;
    ItemData.Quantity = 1;




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



void AHealthItem::UpdateTexts(FString Language) {



	if (Language == "Spanish") {


		ItemData.ItemName = FName("Bebida saludable");
		ItemData.ItemDescription = "Restura un 50% de tu salud";


	}
	else {


		ItemData.ItemName = FName("Healthy Drink");
		ItemData.ItemDescription = "Restores 50% of your life";



	}





}