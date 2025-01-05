// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class ONDIRT2_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	class UCapsuleComponent* CollisionComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;



	//Anims
	
	
	UFUNCTION(BlueprintCallable, Category = "AI Animation")
	void SetIsAttack(bool Value);

	UFUNCTION(BlueprintCallable, Category = "AI Animation")
	bool GetIsAttack() const;

	//Alert
	UFUNCTION(BlueprintCallable, Category = "AI Animation")
	void SetIsAlert(bool Value);

	UFUNCTION(BlueprintCallable, Category = "AI Animation")
	bool GetIsAlert() const;

	UFUNCTION(BlueprintCallable, Category = "IA FUNCTION")
	void SetIsLooking(bool Value);

	UFUNCTION(BlueprintCallable, Category = "IA FUNCTION")
	bool GetIsLooking() const;


private:

	FTimerHandle StopTimerHandle;



	UPROPERTY(EditAnywhere)
	FName HandSocketName;

	UPROPERTY(EditAnywhere)
	float TraceDistance = 0.1f; // Distancia del trazado


	void PerformLineTrace();
	//void ResetAttacked();
	//void PlayerDeath(bool bPlayerDead);
	//void SetPlayerDeath(bool bNewPlayerDead);
	//void SetAttack(bool Value);



	bool bIsAttacking;
	bool bIsAlert;
	bool bIsLooking;
	bool bIsPlayerDead;
	bool bIsStopAttack;
	//Stats
	float Life;

};
