// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CineCameraActor.h"
#include "FixedCamera.generated.h"

/**
 * 
 */
UCLASS()
class ONDIRT2_API AFixedCamera : public ACineCameraActor
{
	GENERATED_BODY()


public:
	AFixedCamera(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;


	// Permitir o desactivar rotación
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Control")
	bool bAllowRotation = false;

	// Actor objetivo para seguimiento
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Control")
	AActor* TargetActor;

	


private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BoxComponent;



	void EnableRotation();
	void DisableRotation();

	
};
