// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItems.h"
#include "HealthItem.generated.h"

/**
 * 
 */
UCLASS()
class ONDIRT2_API AHealthItem : public AInventoryItems
{
	GENERATED_BODY()


public:
    AHealthItem();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Properties")
    float RestoreLife;

 

	
};
