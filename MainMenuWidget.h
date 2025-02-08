// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONDIRT2_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()


public:

	virtual void NativeConstruct() override;

	void UpdateTexts(FString Language);


protected:

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NewGameText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* OptionsText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* QuitText;


};
