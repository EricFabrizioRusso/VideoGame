// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"


class AInventoryItems;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ONDIRT2_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;



public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// Agrega un ítem al inventario
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void PickUpItem(AInventoryItems* Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory")

	void AddItem(const FInventoryItemData& NewItem);

	/*UFUNCTION(BlueprintCallable, Category = "Inventory")
	TArray<FString> GetInventoryItemNames() const;*/

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TArray<FInventoryItemData> GetInventoryItems() const;

	// Lista de ítems almacenados en el inventario
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<AInventoryItems*> Inventory;*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FInventoryItemData> Inventory;

	void RemoveItemByName(const FString& ItemName);

	// Reemplazar los datos del inventario
	void SetInventoryItems(const TArray<FInventoryItemData>& NewInventory);


	void UpdateItemsLanguage();

	

		
};



