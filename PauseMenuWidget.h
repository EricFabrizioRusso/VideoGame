// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONDIRT2_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	void UpdateTexts(FString Language);




protected:


	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ResumeText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NewGameText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* OptionsText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MainMenuText;




	
};
