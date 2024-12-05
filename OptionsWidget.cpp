// Fill out your copyright notice in the Description page of Project Settings.


#include "OptionsWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "GameFramework/GameUserSettings.h"

void UOptionsWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Initialize resolutions
    Resolutions = { "800x600", "1024x768", "1280x720", "1920x1080" };
    //CurrentResolutionIndex = 0;
    //UpdateResolutionText();

    // Initialize texture qualities
    TextureQualities = { "Low", "Medium", "High", "Epic" };
    //CurrentTextureQualityIndex = 2; // Default to "High"
    //UpdateTextureText();

    // Initialize shadow qualities
    ShadowQualities = { "Low", "Medium", "High", "Epic" };
    // = 2; // Default to "High"
    //UpdateShadowText();

    // Leer configuraciones actuales
    UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
    if (UserSettings)
    {
        FIntPoint Resolution = UserSettings->GetScreenResolution();
        FString ResolutionString = FString::Printf(TEXT("%dx%d"), Resolution.X, Resolution.Y);
        CurrentResolutionIndex = Resolutions.IndexOfByKey(ResolutionString);
        if (CurrentResolutionIndex == INDEX_NONE)
        {
            CurrentResolutionIndex = 0; // Si no se encuentra, usar la primera resolución
        }

        CurrentTextureQualityIndex = UserSettings->GetTextureQuality();
        CurrentShadowQualityIndex = UserSettings->GetShadowQuality();
    }

    // Actualizar textos
    UpdateResolutionText();
    UpdateTextureText();
    UpdateShadowText();

    if (LeftArrowButton)
    {
        LeftArrowButton->OnClicked.AddDynamic(this, &UOptionsWidget::OnLeftArrowClicked);
    }

    if (RightArrowButton)
    {
        RightArrowButton->OnClicked.AddDynamic(this, &UOptionsWidget::OnRightArrowClicked);
    }

    if (ApplyButton)
    {
        ApplyButton->OnClicked.AddDynamic(this, &UOptionsWidget::OnApplyClicked);
    }

    if (TextureLeftArrowButton)
    {
        TextureLeftArrowButton->OnClicked.AddDynamic(this, &UOptionsWidget::OnTextureLeftArrowClicked);
    }

    if (TextureRightArrowButton)
    {
        TextureRightArrowButton->OnClicked.AddDynamic(this, &UOptionsWidget::OnTextureRightArrowClicked);
    }

    if (ShadowLeftArrowButton)
    {
        ShadowLeftArrowButton->OnClicked.AddDynamic(this, &UOptionsWidget::OnShadowLeftArrowClicked);
    }

    if (ShadowRightArrowButton)
    {
        ShadowRightArrowButton->OnClicked.AddDynamic(this, &UOptionsWidget::OnShadowRightArrowClicked);
    }
}

void UOptionsWidget::OnLeftArrowClicked()
{
    if (CurrentResolutionIndex > 0)
    {
        CurrentResolutionIndex--;
        UpdateResolutionText();
    }
}

void UOptionsWidget::OnRightArrowClicked()
{
    if (CurrentResolutionIndex < Resolutions.Num() - 1)
    {
        CurrentResolutionIndex++;
        UpdateResolutionText();
    }
}

void UOptionsWidget::OnApplyClicked()
{
    FString Resolution = Resolutions[CurrentResolutionIndex];
    TArray<FString> ResArray;
    Resolution.ParseIntoArray(ResArray, TEXT("x"));

    if (ResArray.Num() == 2)
    {
        int32 Width = FCString::Atoi(*ResArray[0]);
        int32 Height = FCString::Atoi(*ResArray[1]);

        UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
        if (UserSettings)
        {
            UserSettings->SetScreenResolution(FIntPoint(Width, Height));
            UserSettings->SetTextureQuality(CurrentTextureQualityIndex);
            UserSettings->SetShadowQuality(CurrentShadowQualityIndex);
            UserSettings->ApplySettings(false);
            UserSettings->SaveSettings(); // Guardar configuraciones
            UE_LOG(LogTemp, Warning, TEXT("Resolution changed to: %dx%d"), Width, Height);
            UE_LOG(LogTemp, Warning, TEXT("Texture Quality changed to: %s"), *TextureQualities[CurrentTextureQualityIndex]);
            UE_LOG(LogTemp, Warning, TEXT("Shadow Quality changed to: %s"), *ShadowQualities[CurrentShadowQualityIndex]);
        }
    }
}

void UOptionsWidget::UpdateResolutionText()
{
    if (ResolutionText)
    {
        ResolutionText->SetText(FText::FromString(Resolutions[CurrentResolutionIndex]));
    }
}

void UOptionsWidget::OnTextureLeftArrowClicked()
{
    if (CurrentTextureQualityIndex > 0)
    {
        CurrentTextureQualityIndex--;
        UpdateTextureText();
    }
}

void UOptionsWidget::OnTextureRightArrowClicked()
{
    if (CurrentTextureQualityIndex < TextureQualities.Num() - 1)
    {
        CurrentTextureQualityIndex++;
        UpdateTextureText();
    }
}

void UOptionsWidget::OnShadowLeftArrowClicked()
{
    if (CurrentShadowQualityIndex > 0)
    {
        CurrentShadowQualityIndex--;
        UpdateShadowText();
    }
}

void UOptionsWidget::OnShadowRightArrowClicked()
{
    if (CurrentShadowQualityIndex < ShadowQualities.Num() - 1)
    {
        CurrentShadowQualityIndex++;
        UpdateShadowText();
    }
}

void UOptionsWidget::UpdateTextureText()
{
    if (TextureText)
    {
        TextureText->SetText(FText::FromString(TextureQualities[CurrentTextureQualityIndex]));
    }
}

void UOptionsWidget::UpdateShadowText()
{
    if (ShadowText)
    {
        ShadowText->SetText(FText::FromString(ShadowQualities[CurrentShadowQualityIndex]));
    }
}