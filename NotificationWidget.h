// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NotificationWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONDIRT2_API UNotificationWidget : public UUserWidget
{
	GENERATED_BODY()


public:



	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DescriptionNotification;
	
};
