// Fill out your copyright notice in the Description page of Project Settings.
// Fill out your copyright notice in the Description page of Project Settings.


#include "OptionsWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "GameFramework/GameUserSettings.h"
#include "PauseMenuWidget.h"
#include "OnDirt2Character.h"
#include "EngineUtils.h"
#include "InventoryItems.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"
#include "Misc/ConfigCacheIni.h"
#include "Components/Slider.h"

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

    SelectLanguages = { "Spanish", "English"};

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
    else {


        // Configuración por defecto si no se encuentra UserSettings
        CurrentResolutionIndex = 0;
        CurrentTextureQualityIndex = 2; // Índice para "High"
        CurrentShadowQualityIndex = 2; // Índice para "High"
        CurrentLanguageIndex = 0; // English

    }


    // Obtener el volumen guardado en Config
    float Volume = 1.0f; // Valor por defecto
    GConfig->GetFloat(TEXT("/Script/Engine.AudioSettings"), TEXT("MasterVolume"), Volume, GGameUserSettingsIni);

    // Configurar el slider con el volumen actual
    if (MasterVolumeSlider)
    {
        MasterVolumeSlider->SetValue(Volume);
        MasterVolumeSlider->OnValueChanged.AddDynamic(this, &UOptionsWidget::OnMasterVolumeChanged);
    }


    FString SelectedLanguage;
    if (GConfig->GetString(TEXT("/Script/Engine.GameUserSettings"), TEXT("SelectedLanguage"), SelectedLanguage, GGameUserSettingsIni))
    {
        CurrentLanguageIndex = SelectLanguages.IndexOfByKey(SelectedLanguage);
        if (CurrentLanguageIndex == INDEX_NONE)
        {
            CurrentLanguageIndex = 0; // Si no se encuentra, usar el primer idioma
        }
    }




    // Actualizar textos
    UpdateResolutionText();
    UpdateTextureText();
    UpdateShadowText();
    UpdateTexts();

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

    if (LanguageLeftArrowButton) {

        LanguageLeftArrowButton->OnClicked.AddDynamic(this, &UOptionsWidget::OnLanguageLeftArrowClicked);
    }

    if (LanguageRightArrowButton) {

        LanguageRightArrowButton->OnClicked.AddDynamic(this, &UOptionsWidget::OnLanguageRightArrowClicked);
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
            UpdateTexts();
            
        }

        FString SelectedLanguage = SelectLanguages[CurrentLanguageIndex];

        // Guardar el idioma en GameUserSettings.ini
        GConfig->SetString(TEXT("/Script/Engine.GameUserSettings"), TEXT("SelectedLanguage"),
            *SelectedLanguage, GGameUserSettingsIni);
        GConfig->Flush(false, GGameUserSettingsIni); // Guardar en disco



        // Guardar y aplicar el volumen
        float Volume = MasterVolumeSlider->GetValue();
        GConfig->SetFloat(TEXT("/Script/Engine.AudioSettings"), TEXT("MasterVolume"), Volume, GGameUserSettingsIni);
        GConfig->Flush(false, GGameUserSettingsIni);




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

void UOptionsWidget::OnLanguageLeftArrowClicked() {

    if (CurrentLanguageIndex > 0)
    {
        CurrentLanguageIndex--;
        UpdateLanguageText();
    }


}
void UOptionsWidget::OnLanguageRightArrowClicked() {

    if (CurrentLanguageIndex < SelectLanguages.Num() - 1)
    {
        CurrentLanguageIndex++;

        UpdateLanguageText();
       
    }

}

void UOptionsWidget::UpdateLanguageText() {


    LanguageText->SetText(FText::FromString(SelectLanguages[CurrentLanguageIndex]));

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
void UOptionsWidget::UpdateTexts() {


    if (CurrentLanguageIndex == 1) // Español
    {
        SelectLanguages = { "English", "Spanish" };
        TextureQualities = { "Bajo", "Medio", "Alto", "Épico" };
        ShadowQualities = { "Bajo", "Medio", "Alto", "Épico" };

        MainOptionsText->SetText(FText::FromString("Opciones"));
        MainResolutionText->SetText(FText::FromString("Resolucion"));
        MainTexturesText->SetText(FText::FromString("Texturas"));
        MainShadowsText->SetText(FText::FromString("Sombras"));
        MainLanguageText->SetText(FText::FromString("Lenguaje"));

        BackBtnText->SetText(FText::FromString("Volver"));
        ApplyBtnText->SetText(FText::FromString("Aplicar"));
        VolumeText->SetText(FText::FromString("Volumen"));
        
        Language = true;






        UWorld* World = GetWorld();
        if (World)
        {
            for (TActorIterator<AInventoryItems> It(World); It; ++It)
            {
                AInventoryItems* Item = *It;
                if (Item && (Item->ItemData.ItemName == "Pistol" || Item->ItemData.ItemName == "Pistola"))
                {
                    
                    //UE_LOG(LogTemp, Warning, TEXT("Se detecta la pístola."));

                    //Item->ItemData.ItemName = "Pistola";
                    //Item->ItemData.ItemDescription = "Arma de fuego con capacidad de 10 balas en su cargador";


                    Item->UpdateTexts("Spanish");

                }



                if (Item && (Item->ItemData.ItemName == "Bat" || Item->ItemData.ItemName == "Bate"))
                {

                    //UE_LOG(LogTemp, Warning, TEXT("Se detecta la pístol."));

                    //Item->ItemData.ItemName = "Bate";
                    //Item->ItemData.ItemDescription = "Arma cuerpo a cuerpo efectiva a corto alcance pero mas poderosa que un arma de fuego";

                    Item->UpdateTexts("Spanish");

                }


                if (Item && (Item->ItemData.ItemName == "Key" || Item->ItemData.ItemName == "Llave"))
                {

                    UE_LOG(LogTemp, Warning, TEXT("Se detecta la pístol."));

                    //Item->ItemData.ItemName = "Llave";
                    //Item->ItemData.ItemDescription = "Se puede usar para abrir una puerta";

                    Item->UpdateTexts("Spanish");

                }



                if (Item && (Item->ItemData.ItemName == "Healthy Drink" || Item->ItemData.ItemName == "Bebida saludable"))
                {

                    UE_LOG(LogTemp, Warning, TEXT("Se detecta la pístol."));

                    //Item->ItemData.ItemName = "Bebida saludable";
                    //Item->ItemData.ItemDescription = "Restura un 50% de tu salud";

                    Item->UpdateTexts("Spanish");

                }



            }
        }




    }
    else // Inglés
    {
        SelectLanguages = { "English", "Spanish" };
        TextureQualities = { "Low", "Medium", "High", "Epic" };
        ShadowQualities = { "Low", "Medium", "High", "Epic" };

        MainOptionsText->SetText(FText::FromString("Options"));
        MainResolutionText->SetText(FText::FromString("Resolution"));
        MainTexturesText->SetText(FText::FromString("Textures"));
        MainShadowsText->SetText(FText::FromString("Shadows"));
        MainLanguageText->SetText(FText::FromString("Lenguage"));

        BackBtnText->SetText(FText::FromString("Back"));
        ApplyBtnText->SetText(FText::FromString("Apply"));
        VolumeText->SetText(FText::FromString("Volume"));


        Language = false;

        UWorld* World = GetWorld();
        if (World)
        {
            for (TActorIterator<AInventoryItems> It(World); It; ++It)
            {
                AInventoryItems* Item = *It;
                if (Item && (Item->ItemData.ItemName == "Pistol" || Item->ItemData.ItemName == "Pistola"))
                {

                    //UE_LOG(LogTemp, Warning, TEXT("Se detecta la pístol."));

                    //Item->ItemData.ItemName = "Pistol";
                    //Item->ItemData.ItemDescription = "Firegun with a capacity of 10 bullets in its magazine ";

                    Item->UpdateTexts("English");

                }


                if (Item && (Item->ItemData.ItemName == "Bat" || Item->ItemData.ItemName == "Bate"))
                {

                    //UE_LOG(LogTemp, Warning, TEXT("Se detecta la pístol."));

                   // Item->ItemData.ItemName = "Bat";
                    //Item->ItemData.ItemDescription = "Melee weapon efective in short encounters but most powerful than fireguns";

                    Item->UpdateTexts("English");

                }


                if (Item && (Item->ItemData.ItemName == "Key" || Item->ItemData.ItemName == "Llave"))
                {

                    //UE_LOG(LogTemp, Warning, TEXT("Se detecta la pístol."));

                    //Item->ItemData.ItemName = "Key";
                    //Item->ItemData.ItemDescription = "You can use it to unlock a door";

                    Item->UpdateTexts("English");

                }



                if (Item && (Item->ItemData.ItemName == "Healthy Drink" || Item->ItemData.ItemName == "Bebida saludable"))
                {

                    //UE_LOG(LogTemp, Warning, TEXT("Se detecta la pístol."));

                    //Item->ItemData.ItemName = "Healthy Drink";
                    //Item->ItemData.ItemDescription = "Restores 50% of your life ";
                  
                    Item->UpdateTexts("English");

                }


                if (Item && (Item->ItemData.ItemName == "Pistol Ammo" || Item->ItemData.ItemName == "Municion de pistola"))
                {

                   // UE_LOG(LogTemp, Warning, TEXT("Se detecta la pístol."));

                   // Item->ItemData.ItemName = "Pistol Ammo";
                   // Item->ItemData.ItemDescription = "Ammo for 9mm Pistol";

                    Item->UpdateTexts("English");

                }





            }
        }



    }

    // Actualizar el texto del idioma en UI
    if (LanguageText)
    {
        LanguageText->SetText(FText::FromString(SelectLanguages[CurrentLanguageIndex]));
    }

    // Actualizar los textos de calidad en UI
    UpdateTextureText();
    UpdateShadowText();



}


void UOptionsWidget::OnMasterVolumeChanged(float Value)
{
    // Cambiar el volumen del juego
    UGameplayStatics::SetSoundMixClassOverride(this, nullptr, nullptr, Value, 1.0f, 0.0f, true);

    // Guardar el valor en Config para que persista
    GConfig->SetFloat(TEXT("/Script/Engine.AudioSettings"), TEXT("MasterVolume"), Value, GGameUserSettingsIni);
    GConfig->Flush(false, GGameUserSettingsIni);
}



float UOptionsWidget::GetMasterVolume()
{
    float Volume = 1.0f; // Valor por defecto
    GConfig->GetFloat(TEXT("/Script/Engine.AudioSettings"), TEXT("MasterVolume"), Volume, GGameUserSettingsIni);
    return Volume;
}






























