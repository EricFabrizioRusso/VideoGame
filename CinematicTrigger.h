// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CinematicTrigger.generated.h"

class UBoxComponent;
class ULevelSequence;
UCLASS()

class ONDIRT2_API ACinematicTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACinematicTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinematic")
	ULevelSequence* CinematicSequence;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	void StartCinematic();


	UFUNCTION()
	void OnCinematicFinished();

	bool CinematicActivated;



private:

private:
	UPROPERTY()
	class ULevelSequencePlayer* SequencePlayerRef;


};
