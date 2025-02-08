// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemsOptionsWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONDIRT2_API UItemsOptionsWidget : public UUserWidget
{
	GENERATED_BODY()

public:


	//UPROPERTY(meta = (BindWidget))
	//class UButton* CombineButton; 


    UPROPERTY(meta = (BindWidget))
    class UButton* UseBtn; 

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* UseText;


	//UPROPERTY(meta = (BindWidget))
	//class UTextBlock* EquipText;


	UPROPERTY(meta = (BindWidget))
	class UButton* DropBtn; 

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DropText;

	class AOnDirt2Character* PlayerCharacter;

	virtual void NativeConstruct() override;


	UFUNCTION()
	void OnUseClicked();

	UFUNCTION()
	void OnDropClicked();

	void UpdateTexts(FString Language);
	
};
