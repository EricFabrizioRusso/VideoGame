// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SubtitleWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONDIRT2_API USubtitleWidget : public UUserWidget
{
	GENERATED_BODY()




public:

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SubtitleText;


	UFUNCTION(BlueprintCallable)
	void SetNewText(FString SubText);


	UFUNCTION(BlueprintCallable)
	bool GetLanguageSubtitle() const;

private:

	bool LanguageSubtitle;
	
};
