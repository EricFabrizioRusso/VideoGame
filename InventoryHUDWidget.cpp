// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryHUDWidget.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "InventoryItemWidget.h" // Clase del widget de los ítems
#include "InventoryItems.h"
#include "Components/Image.h"
#include "InventoryItemWidget.h"
#include "DetailsItemWidget.h"


void UInventoryHUDWidget::NativeConstruct() {




}

void UInventoryHUDWidget::PopulateInventory(const TArray<FInventoryItemData>& Items)
{

    if (!InventoryList) return;


    // Limpia el contenido actual
    InventoryList->ClearChildren();


    for (const FInventoryItemData& Item : Items)
    {
        InventoryItemWidget = CreateWidget<UInventoryItemWidget>(GetWorld(), InventoryItemWidgetClass);
        if (InventoryItemWidget)
        {
            // Establece el texto del nombre y la cantidad
           //InventoryItemWidget->ItemNameText->SetText(FText::FromString(FString::Printf(TEXT("%s x%d"), *Item.ItemName.ToString(), Item.Quantity)));
           //InventoryItemWidget->ItemDescriptionText->SetText(FText::FromString(Item.ItemDescription));

            InventoryItemWidget->SetItemData(Item.ItemName.ToString(), Item.ItemDescription,Item.ItemIcon);
            // Configura el ícono del ítem
            if (Item.ItemIcon)
            {
                InventoryItemWidget->ItemIconImage->SetBrushFromTexture(Item.ItemIcon);
            }

            // Configura la descripción (opcional)

            InventoryList->AddChild(InventoryItemWidget);

        }

    }


}




