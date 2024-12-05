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

private:
    TArray<FString> Resolutions;
    int32 CurrentResolutionIndex;

    TArray<FString> TextureQualities;
    int32 CurrentTextureQualityIndex;

    TArray<FString> ShadowQualities;
    int32 CurrentShadowQualityIndex;

    void UpdateResolutionText();
    void UpdateTextureText();
    void UpdateShadowText();
	
};
