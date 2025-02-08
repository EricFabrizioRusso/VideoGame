// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoItem.h"

AAmmoItem::AAmmoItem() {


	PrimaryActorTick.bCanEverTick = true;


	ItemData.ItemName = "Pistol Ammo";
	ItemData.ItemDescription = "Ammo for Pistol 9mm";
	ItemData.Quantity = 20;
	AmmoAmount = ItemData.Quantity;



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



void AAmmoItem::UpdateTexts(FString Language) {



	if (Language == "Spanish") {

		ItemData.ItemName = "Municion de pistola";
		ItemData.ItemDescription = "Municion para pistola 9mm ";


	}
	else {



		ItemData.ItemName = "Pistol Ammo";
		ItemData.ItemDescription = "Ammo for 9mm Pistol";



	}





}