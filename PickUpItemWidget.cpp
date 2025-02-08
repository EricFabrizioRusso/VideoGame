// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpItemWidget.h"
#include "Components/TextBlock.h"

void UPickUpItemWidget::NativeConstruct() {



    ItemName = nullptr;






}

void  UPickUpItemWidget::SetItemName(FString NewText)
{


    FString SavedLanguage;
    if (GConfig->GetString(TEXT("/Script/Engine.GameUserSettings"), TEXT("SelectedLanguage"),
        SavedLanguage, GGameUserSettingsIni))
    {

        if (SavedLanguage == "Spanish") {

            if (ItemName)
            {

                ItemName->SetText(FText::FromString("Es una " + NewText + " Recoger?"));

                UpdateTexts("Spanish");

            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("NoteText no está inicializado"));
            }



        }
        else {



            if (ItemName)
            {

                ItemName->SetText(FText::FromString("It's a " + NewText + " take it?"));

                UpdateTexts("English");
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("NoteText no está inicializado"));
            }



        }


    }
    else
    {
        

        if (ItemName)
        {

            ItemName->SetText(FText::FromString("It's a " + NewText + " Read?"));

        }



    }


    


}


void UPickUpItemWidget::UpdateTexts(FString Language) {



    if (Language == "Spanish") {


        YesText->SetText(FText::FromString("Si"));
        NoText->SetText(FText::FromString("No"));


    }
    else {


        YesText->SetText(FText::FromString("Yes"));
        NoText->SetText(FText::FromString("No"));



    }







}

