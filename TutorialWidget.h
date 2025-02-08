// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TutorialWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONDIRT2_API UTutorialWidget : public UUserWidget
{
	GENERATED_BODY()
	


public:

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TutorialText;


	UFUNCTION(BlueprintCallable)
	void SetNewText(FString SubText);


	UFUNCTION(BlueprintCallable)
	bool GetLanguageTutorial() const;

private:

	bool LanguageTutorial;

};
