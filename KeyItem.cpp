// Fill out your copyright notice in the Description page of Project Settings.


#include "KeyItem.h"



AKeyItem::AKeyItem() {


	ItemData.ItemName = "Key";
	ItemData.ItemDescription = "You can use it to unlock a door";
	ItemData.Quantity = 1;
	AcessDoor = "FrontDoor";





}


void AKeyItem::BeginPlay()
{
	Super::BeginPlay();

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



void AKeyItem::UpdateTexts(FString Language) {



	if (Language == "Spanish") {


		ItemData.ItemName = "Llave";
		ItemData.ItemDescription = "Se puede usar para abrir una puerta";


	}
	else {


		ItemData.ItemName = "Key";
		ItemData.ItemDescription = "You can use it to unlock a door";



	}







}