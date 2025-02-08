// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UMainMenuWidget::NativeConstruct() {


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

void UMainMenuWidget::UpdateTexts(FString Language) {



	if (Language == "Spanish") {

		NewGameText->SetText(FText::FromString("Nueva Partida"));
		OptionsText->SetText(FText::FromString("Opciones"));
		QuitText->SetText(FText::FromString("Salir"));


	}
	else {

		NewGameText->SetText(FText::FromString("New Game"));
		OptionsText->SetText(FText::FromString("Options"));
		QuitText->SetText(FText::FromString("Quit"));
	



	}


}