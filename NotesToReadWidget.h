// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NotesToReadWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONDIRT2_API UNotesToReadWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NoteText;

	UFUNCTION(BlueprintCallable, Category = "Widget")
	void SetNoteText(FString NewText);
	
};
