// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameCheckPoint.generated.h"

/**
 * 
 */

UCLASS()

class ONDIRT2_API USaveGameCheckPoint : public USaveGame
{
	GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite)
    FVector PlayerLocation;

    UPROPERTY(BlueprintReadWrite)
    TArray<FVector> EnemyLocations;




    UPROPERTY(BlueprintReadWrite)
    TArray<FInventoryItemData> CollectedItems; // Estructura que contiene los datos del inventario
	
};
