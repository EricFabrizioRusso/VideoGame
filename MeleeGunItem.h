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

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Properties")
	float AttackDamage;


	// Manejar el evento de superposición
	UFUNCTION()
	void OnMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	
};
