// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItems.h"
#include "KeyItem.generated.h"

/**
 * 
 */
UCLASS()
class ONDIRT2_API AKeyItem : public AInventoryItems
{
	GENERATED_BODY()


public: 

	AKeyItem();



	void UpdateTexts(FString Language);


	FString AcessDoor;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};


