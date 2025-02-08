// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionsWidget.generated.h"

/**
 *
 */
UCLASS()
class ONDIRT2_API UOptionsWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;


    bool Language;

protected:
    UPROPERTY(meta = (BindWidget))
    class UButton* LeftArrowButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* RightArrowButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* ApplyButton;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ResolutionText;

    UPROPERTY(meta = (BindWidget))
    class UButton* TextureLeftArrowButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* TextureRightArrowButton;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TextureText;

    UPROPERTY(meta = (BindWidget))
    class UButton* ShadowLeftArrowButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* ShadowRightArrowButton;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ShadowText;


    UPROPERTY(meta = (BindWidget))
    class UButton* LanguageLeftArrowButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* LanguageRightArrowButton;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* LanguageText;



    //Texts
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* MainOptionsText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* MainResolutionText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* MainTexturesText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* MainShadowsText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* MainLanguageText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* BackBtnText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ApplyBtnText;

    //Volumen
    UPROPERTY(meta = (BindWidget))
    class USlider* MasterVolumeSlider;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* VolumeText;


    UFUNCTION()
    void OnLeftArrowClicked();

    UFUNCTION()
    void OnRightArrowClicked();

    UFUNCTION()
    void OnApplyClicked();

    UFUNCTION()
    void OnTextureLeftArrowClicked();

    UFUNCTION()
    void OnTextureRightArrowClicked();

    UFUNCTION()
    void OnShadowLeftArrowClicked();

    UFUNCTION()
    void OnShadowRightArrowClicked();

    UFUNCTION()
    void OnLanguageLeftArrowClicked();

    UFUNCTION()
    void OnLanguageRightArrowClicked();


    //Volumen
    UFUNCTION()
    void OnMasterVolumeChanged(float Value);


    UFUNCTION(BlueprintCallable, Category = "Settings")
    static float GetMasterVolume();

private:
    TArray<FString> Resolutions;
    int32 CurrentResolutionIndex;

    TArray<FString> TextureQualities;
    int32 CurrentTextureQualityIndex;

    TArray<FString> ShadowQualities;
    int32 CurrentShadowQualityIndex;

    TArray<FString> SelectLanguages;
    int32 CurrentLanguageIndex;


    class AOnDirt2Character* PlayerCharacter;


    //Connections
    class UPauseMenuWidget* PauseMenuWidget;

    void UpdateResolutionText();
    void UpdateTextureText();
    void UpdateShadowText();
    void UpdateLanguageText();
    void UpdateTexts();


};
	

