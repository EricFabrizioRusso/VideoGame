// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LegacyCameraShake.h"
#include "CameraShakeComponent.generated.h"

/**
 * 
 */
UCLASS()
class ONDIRT2_API UCameraShakeComponent : public ULegacyCameraShake
{
	GENERATED_BODY()

public:

	UCameraShakeComponent();



	void SetOscillationDuration(float NewDuration);
	
};
