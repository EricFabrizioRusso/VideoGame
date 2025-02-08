// Fill out your copyright notice in the Description page of Project Settings.


#include "SubtitleWidget.h"
#include "Components/TextBlock.h"

void USubtitleWidget::NativeConstruct()
{
    Super::NativeConstruct();


	FString SavedLanguage;
	if (GConfig->GetString(TEXT("/Script/Engine.GameUserSettings"), TEXT("SelectedLanguage"),
		SavedLanguage, GGameUserSettingsIni))
	{
		if (SavedLanguage == "Spanish") {

			LanguageSubtitle = true;
		}
		else {

			LanguageSubtitle = false;

		}
	
	}
	else
	{
		LanguageSubtitle = false;
	}


}


void USubtitleWidget::SetNewText(FString SubText) {

    if (SubtitleText) {

        SubtitleText->SetText(FText::FromString(SubText));

    }



}

bool USubtitleWidget::GetLanguageSubtitle() const{


	return LanguageSubtitle;

}