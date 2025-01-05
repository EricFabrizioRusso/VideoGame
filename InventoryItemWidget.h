// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemWidget.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIconClicked, const FInventoryItemData&, ItemData);

UCLASS()
class ONDIRT2_API UInventoryItemWidget : public UUserWidget
{
	GENERATED_BODY()


public:
    // Variable para almacenar el nombre del ítem
    /*UPROPERTY(meta = (BindWidget))
    class UTextBlock* ItemNameText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ItemDescriptionText;*/

    UPROPERTY(meta = (BindWidget))
    class UImage* ItemIconImage;

    UPROPERTY(meta = (BindWidget))
    class UButton* IconButton; // Botón que envolverá el ícono

    // Sobreescribir NativeConstruct para inicializar el widget
    virtual void NativeConstruct() override;

    void SetItemData(const FString& Name, const FString& Description, UTexture2D* Icon);


    UPROPERTY()
    UInventoryHUDWidget* HUDWidget;


   // FString ItemName;
    //FString ItemDesc;

    UPROPERTY(BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
    FString ItemName;

    UPROPERTY(BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
    FString ItemDesc;



    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> DetailsWidgetClass;

    UPROPERTY()
    UDetailsItemWidget* DetailsItemWidget;

   //class AOnDirt2Character* PlayerCharacter;

private:
    UFUNCTION()
    void OnIconClicked();

	
};
