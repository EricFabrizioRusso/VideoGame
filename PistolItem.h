// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItems.h"
#include "PistolItem.generated.h"

/**
 * 
 */
UCLASS()
class ONDIRT2_API APistolItem : public AInventoryItems
{
	GENERATED_BODY()

public:
	APistolItem();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Properties")
	float AttackDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* MuzzleSocketComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
};
