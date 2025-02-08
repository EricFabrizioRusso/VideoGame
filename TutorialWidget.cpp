// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialWidget.h"
#include "Components/TextBlock.h"

void  UTutorialWidget::NativeConstruct()
{
	Super::NativeConstruct();


	FString SavedLanguage;
	if (GConfig->GetString(TEXT("/Script/Engine.GameUserSettings"), TEXT("SelectedLanguage"),
		SavedLanguage, GGameUserSettingsIni))
	{
		if (SavedLanguage == "Spanish") {

			LanguageTutorial = true;
		}
		else {

			LanguageTutorial = false;

		}

	}
	else
	{
		LanguageTutorial = false;
	}


}


void  UTutorialWidget::SetNewText(FString SubText) {

	if (TutorialText) {

		TutorialText->SetText(FText::FromString(SubText));

	}



}

bool  UTutorialWidget::GetLanguageTutorial() const {


	return LanguageTutorial;

}
