// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AccessLevelTriggerActor.generated.h"

UCLASS()
class ONDIRT2_API AAccessLevelTriggerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAccessLevelTriggerActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* BoxComponent;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	bool bIsOverlaped;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Properties")
	bool bIsLocked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Properties")
	FString AccessDoor;


	UFUNCTION(BlueprintImplementableEvent, Category = "Custom Events")
	void ChangeLevel();

};
