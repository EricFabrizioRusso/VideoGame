// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThrowOBJ.generated.h"

UCLASS()
class ONDIRT2_API AThrowOBJ : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AThrowOBJ();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    // Mesh component para el objeto
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* Mesh;

    // Componente de colisión (box)
    UPROPERTY(VisibleAnywhere)
    class UBoxComponent* CollisionComponent;

    // Función para manejar el overlap
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    // Variable para saber si puede ser agarrado
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ThrowOBJ")
    bool bCanBeGrabbed;

};
