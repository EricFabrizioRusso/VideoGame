// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryHUDWidget.generated.h"

/**
 * 
 */
class UInventoryItemWidget;
class UDetailsItemWidget;
UCLASS()
class ONDIRT2_API UInventoryHUDWidget : public UUserWidget
{
	GENERATED_BODY()



public:
    // Referencia al contenedor en el widget
    UPROPERTY(meta = (BindWidget))
    class UVerticalBox* InventoryList;


   /* UPROPERTY(meta = (BindWidget))
    class UButton* UseBtn;

    //UPROPERTY(meta = (BindWidget))
    //class UButton* CombineButton; 

    UPROPERTY(meta = (BindWidget))
    class UButton* DropButton;*/

    // Función para agregar un ítem al widget
    UFUNCTION(BlueprintCallable, Category = "Inventory")

    void PopulateInventory(const TArray<FInventoryItemData>& Items);

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> InventoryItemWidgetClass;

    UPROPERTY()
    UInventoryItemWidget* InventoryItemWidget;

    virtual void NativeConstruct() override;
	
};
