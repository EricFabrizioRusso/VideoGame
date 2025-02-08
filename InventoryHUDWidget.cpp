// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryHUDWidget.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "InventoryItemWidget.h" // Clase del widget de los �tems
#include "InventoryItems.h"
#include "Components/Image.h"
#include "InventoryItemWidget.h"
#include "DetailsItemWidget.h"


void UInventoryHUDWidget::NativeConstruct() {

    Super::NativeConstruct();

    FString SavedLanguage;
    if (GConfig->GetString(TEXT("/Script/Engine.GameUserSettings"), TEXT("SelectedLanguage"),
        SavedLanguage, GGameUserSettingsIni))
    {
        UpdateTexts(SavedLanguage);
    }
    else
    {
        UpdateTexts("English"); // Por defecto
    }


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
            // Configura el �cono del �tem
            if (Item.ItemIcon)
            {
                InventoryItemWidget->ItemIconImage->SetBrushFromTexture(Item.ItemIcon);
            }

            // Configura la descripci�n (opcional)

            InventoryList->AddChild(InventoryItemWidget);

        }

    }


}


void UInventoryHUDWidget::UpdateTexts(FString Language) {



    if (Language == "Spanish") {

        InventaryText->SetText(FText::FromString("Inventario"));


    }
    else {


        InventaryText->SetText(FText::FromString("Inventary"));



    }


}




