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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;



	//Anims
	
	//Attacking
	UFUNCTION(BlueprintCallable, Category = "AI Animation")
	void SetIsAttack(bool Value);

	UFUNCTION(BlueprintCallable, Category = "AI Animation")
	bool GetIsAttack() const;

	//Alert
	UFUNCTION(BlueprintCallable, Category = "AI Animation")
	void SetIsAlert(bool Value);

	UFUNCTION(BlueprintCallable, Category = "AI Animation")
	bool GetIsAlert() const;


private:

	bool bIsAttacking;
	bool bIsAlert;


	//Stats
	float Life;

};
