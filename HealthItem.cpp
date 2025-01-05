// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthItem.h"

AHealthItem::AHealthItem()
{
    RestoreLife = 50.0f; // Valor predeterminado


    ItemData.ItemName = FName("HealthPack");
    ItemData.ItemDescription = "Restores health.";
    ItemData.ItemIcon = nullptr;
    ItemData.Quantity = 1;
 
}