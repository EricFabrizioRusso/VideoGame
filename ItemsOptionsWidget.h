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


    UPROPERTY(meta = (BindWidget))
    class UButton* UseBtn; 

	//UPROPERTY(meta = (BindWidget))
	//class UButton* CombineButton; 

	UPROPERTY(meta = (BindWidget))
	class UButton* DropBtn; 

	class AOnDirt2Character* PlayerCharacter;

	virtual void NativeConstruct() override;

	void OnUseClicked();
	void OnDropClicked();
	
};
