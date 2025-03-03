// Fill out your copyright notice in the Description page of Project Settings.


#include "GameOverWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"



void UGameOverWidget::NativeConstruct() {


	Super::NativeConstruct();

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

void UGameOverWidget::UpdateTexts(FString Language) {



	if (Language == "Spanish") {

		GameOverText->SetText(FText::FromString("Presiona click izquierdo para continuar"));


	}
	else {


		GameOverText->SetText(FText::FromString("Press left click to continue"));



	}


}