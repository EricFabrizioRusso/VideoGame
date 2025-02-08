// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseMenuWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "OptionsWidget.h"



void UPauseMenuWidget::NativeConstruct() {


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



void UPauseMenuWidget::UpdateTexts(FString Language) {



	if (Language == "Spanish") {

		ResumeText->SetText(FText::FromString("Reanudar"));
		NewGameText->SetText(FText::FromString("Nuevo Juego"));
		OptionsText->SetText(FText::FromString("Opciones"));
		MainMenuText->SetText(FText::FromString("Menu Principal"));

	}
	else {


		ResumeText->SetText(FText::FromString("Resume"));
		NewGameText->SetText(FText::FromString("New Game"));
		OptionsText->SetText(FText::FromString("Options"));
		MainMenuText->SetText(FText::FromString("Main Menu"));


	}


}