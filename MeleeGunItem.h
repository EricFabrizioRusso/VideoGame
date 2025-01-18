// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItems.h"
#include "MeleeGunItem.generated.h"

/**
 * 
 */
UCLASS()
class ONDIRT2_API AMeleeGunItem : public AInventoryItems
{
	GENERATED_BODY()


public:

	AMeleeGunItem();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Properties")
	float AttackDamage;

	
};
