// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"// Para trabajar con TextBlock
#include "Components/Image.h"
#include "Components/Button.h"
#include "InventoryHUDWidget.h"
#include "DetailsItemWidget.h"
#include "Blueprint/UserWidget.h" // Asegúrate de incluir este archivo
#include "OnDirt2Character.h"

void UInventoryItemWidget::NativeConstruct()
{
    Super::NativeConstruct();


    // Verifica que el TextBlock esté ligado y asigna el texto del ítem

    if (IconButton)
    {
        IconButton->OnClicked.AddDynamic(this, &UInventoryItemWidget::OnIconClicked);
    }
    //ItemName = "NameTest";
    //ItemDesc = "DescTest";


}



void UInventoryItemWidget::SetItemData(const FString& Name, const FString& Description, UTexture2D* Icon)
{

    ItemName = Name;
    ItemDesc = Description;


   
    if (ItemIconImage && Icon)
    {
        ItemIconImage->SetBrushFromTexture(Icon);
    }
   
}


void UInventoryItemWidget::OnIconClicked()
{
    // Crear un widget de texto para el nombre del item

    AOnDirt2Character* PlayerCharacter = Cast<AOnDirt2Character>(GetWorld()->GetFirstPlayerController()->GetPawn());
    if (!PlayerCharacter)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerCharacter no está inicializado"));
        return;
    }

    PlayerCharacter->SetText(ItemName,ItemDesc);
    


}





