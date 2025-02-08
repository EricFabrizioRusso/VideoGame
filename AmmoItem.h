// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItems.h"
#include "AmmoItem.generated.h"

/**
 * 
 */
UCLASS()
class ONDIRT2_API AAmmoItem : public AInventoryItems
{
	GENERATED_BODY()


public:


	AAmmoItem();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoAmount")
	int32 AmmoAmount;



	void UpdateTexts(FString Language);
	
};
