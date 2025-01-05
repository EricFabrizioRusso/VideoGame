// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryItems.generated.h"


USTRUCT(BlueprintType)
struct FInventoryItemData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	FString ItemDescription;

	// Cantidad de ítems para objetos apilables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	int32 Quantity;

	FInventoryItemData()
		: ItemName(NAME_None), ItemIcon(nullptr), ItemDescription(""), Quantity(1) {}
};

UCLASS()
class ONDIRT2_API AInventoryItems : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInventoryItems();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);




	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	FString ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ItemIcon; // Opcional: ícono del ítem*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	UStaticMeshComponent* Mesh;

	// Componente de colisión (box)
	UPROPERTY(VisibleAnywhere, Category = "Item Properties")
	class UBoxComponent* CollisionComponent;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FInventoryItemData ItemData;

	bool  bCanBeGrabbed;
};
