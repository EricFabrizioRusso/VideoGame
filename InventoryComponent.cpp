// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "InventoryItems.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}


void UInventoryComponent::PickUpItem(AInventoryItems* Item)
{
    if (Item)
    {
        AddItem(Item->ItemData);

       // Item->Destroy();

        UE_LOG(LogTemp, Warning, TEXT("Item picked up: %s"), *Item->ItemData.ItemName.ToString());
    }
}

void UInventoryComponent::AddItem(const FInventoryItemData& NewItem)
{

    //Full Inventory
    if (Inventory.Num() >= 4)
    {
        UE_LOG(LogTemp, Warning, TEXT("El inventario está lleno. No se puede agregar el ítem: %s"), *NewItem.ItemName.ToString());
        return;
    }


    // Busca si el ítem ya existe en el inventario
    for (FInventoryItemData& Item : Inventory)
    {
        if (Item.ItemName == NewItem.ItemName)
        {
            // Incrementa la cantidad si el ítem es apilable
            Item.Quantity++;
            UE_LOG(LogTemp, Warning, TEXT("Item apilable incrementado: %s, Cantidad: %d"), *NewItem.ItemName.ToString(), Item.Quantity);
            return;
        }
    }

    // Si no existe, agrega un nuevo ítem al inventario
    Inventory.Add(NewItem);
    UE_LOG(LogTemp, Warning, TEXT("Nuevo ítem agregado: %s"), *NewItem.ItemName.ToString());
}

TArray<FInventoryItemData> UInventoryComponent::GetInventoryItems() const
{
    return Inventory;
}

void UInventoryComponent::RemoveItemByName(const FString& ItemName)
{
    for (int32 i = 0; i < Inventory.Num(); i++)
    {
        if (Inventory[i].ItemName == ItemName)
        {
            Inventory.RemoveAt(i); // Eliminar el ítem del array
            //UE_LOG(LogTemp, Warning, TEXT("Ítem eliminado: %s"), *ItemName.ToString());
        }
 
    }
    return;

}

void UInventoryComponent::SetInventoryItems(const TArray<FInventoryItemData>& NewInventory)
{
    // Reemplazar el inventario actual con el nuevo inventario
    Inventory = NewInventory;

    // Mensaje de depuración para confirmar el cambio
    UE_LOG(LogTemp, Warning, TEXT("El inventario ha sido actualizado. Nuevos elementos:"));

    for (const FInventoryItemData& Item : Inventory)
    {
        UE_LOG(LogTemp, Warning, TEXT("Ítem: %s, Cantidad: %d"), *Item.ItemName.ToString(), Item.Quantity);
    }
}



