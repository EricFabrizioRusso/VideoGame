// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DetailsItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONDIRT2_API UDetailsItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
class UTextBlock* ItemNameText;

UPROPERTY(meta = (BindWidget))
class UTextBlock* ItemDescriptionText;
	
};
