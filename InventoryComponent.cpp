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
        UE_LOG(LogTemp, Warning, TEXT("El inventario est� lleno. No se puede agregar el �tem: %s"), *NewItem.ItemName.ToString());
        return;
    }


    // Busca si el �tem ya existe en el inventario
    for (FInventoryItemData& Item : Inventory)
    {
        if (Item.ItemName == NewItem.ItemName)
        {
            // Incrementa la cantidad si el �tem es apilable
            Item.Quantity++;
            UE_LOG(LogTemp, Warning, TEXT("Item apilable incrementado: %s, Cantidad: %d"), *NewItem.ItemName.ToString(), Item.Quantity);
            return;
        }
    }

    // Si no existe, agrega un nuevo �tem al inventario
    Inventory.Add(NewItem);
    UE_LOG(LogTemp, Warning, TEXT("Nuevo �tem agregado: %s"), *NewItem.ItemName.ToString());
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
            Inventory.RemoveAt(i); // Eliminar el �tem del array
            //UE_LOG(LogTemp, Warning, TEXT("�tem eliminado: %s"), *ItemName.ToString());
        }
 
    }
    return;

}

void UInventoryComponent::SetInventoryItems(const TArray<FInventoryItemData>& NewInventory)
{
    // Reemplazar el inventario actual con el nuevo inventario
    Inventory = NewInventory;

    // Mensaje de depuraci�n para confirmar el cambio
    UE_LOG(LogTemp, Warning, TEXT("El inventario ha sido actualizado. Nuevos elementos:"));

    for (const FInventoryItemData& Item : Inventory)
    {
        UE_LOG(LogTemp, Warning, TEXT("�tem: %s, Cantidad: %d"), *Item.ItemName.ToString(), Item.Quantity);
    }
}



void UInventoryComponent::UpdateItemsLanguage()
{
    FString CurrentLanguage;
    if (!GConfig->GetString(TEXT("/Script/Engine.GameUserSettings"), TEXT("SelectedLanguage"),
        CurrentLanguage, GGameUserSettingsIni))
    {
        CurrentLanguage = "English"; // Idioma por defecto
    }

    for (FInventoryItemData& Item : Inventory)
    {
        if (Item.ItemName == "Pistol" || Item.ItemName == "Pistola") // Verifica si es la pistola
        {
            if (CurrentLanguage == "Spanish")
            {
                Item.ItemName = "Pistola";
                Item.ItemDescription = "Arma de fuego con capacidad de 10 balas en su cargador";
            }
            else
            {
                Item.ItemName = "Pistol";
                Item.ItemDescription = "Firegun with a capacity of 10 bullets in its magazine";
            }

            UE_LOG(LogTemp, Warning, TEXT("Pistol name updated to: %s"), *Item.ItemName.ToString());
        }


        if (Item.ItemName == "Bat" || Item.ItemName == "Bate") // Verifica si es la pistola
        {
            if (CurrentLanguage == "Spanish")
            {
                Item.ItemName = "Bate";
                Item.ItemDescription = "Arma cuerpo a cuerpo efectiva a corto alcance pero mas poderosa que un arma de fuego";
            }
            else
            {
                Item.ItemName = "Bat";
                Item.ItemDescription = "Melee weapon efective in short encounters but most powerful than fireguns";
            }

            UE_LOG(LogTemp, Warning, TEXT("Pistol name updated to: %s"), *Item.ItemName.ToString());
        }


        if (Item.ItemName == "Key" || Item.ItemName == "Llave") // Verifica si es la pistola
        {
            if (CurrentLanguage == "Spanish")
            {
                Item.ItemName = "Llave";
                Item.ItemDescription = "Se puede usar para abrir una puerta";
            }
            else
            {
                Item.ItemName = "Key";
                Item.ItemDescription = "You can use it to unlock a door";
            }

            UE_LOG(LogTemp, Warning, TEXT("Pistol name updated to: %s"), *Item.ItemName.ToString());
        }

        if (Item.ItemName == "Healthy Drink" || Item.ItemName == "Bebida saludable") // Verifica si es la pistola
        {
            if (CurrentLanguage == "Spanish")
            {
                Item.ItemName = "Bebida saludable";
                Item.ItemDescription = "Restura un 50% de tu salud";
            }
            else
            {
                Item.ItemName = "Healthy Drink";
                Item.ItemDescription = "Restores 50% of your life";
            }

            UE_LOG(LogTemp, Warning, TEXT("Pistol name updated to: %s"), *Item.ItemName.ToString());
        }




    }

   
   
}



