// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONDIRT2_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	void UpdateTexts(FString Language);


protected:

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GameOverText;

};
