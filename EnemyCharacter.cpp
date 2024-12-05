// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "FollowTargetTask.h"
#include "OnDirt2Character.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/DamageType.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	HandSocketName = "EnemyHandSocket";

	//Stats
	Life = 100;

	bIsAttacking = false;
	bIsAlert = false;
	bIsLooking = false;
	bIsPlayerDead = false;
	bIsStopAttack = false;
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PerformLineTrace();

}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacter::PerformLineTrace()
{
	FVector HandLocation = GetMesh()->GetSocketLocation(HandSocketName);
	FVector EndLocation = HandLocation + GetActorForwardVector() * TraceDistance;


	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this); // Ignorar al propio enemigo
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, HandLocation, EndLocation, ECC_Visibility, CollisionParams);

	
	if (bHit)
	{
		AOnDirt2Character* HitActor = Cast<AOnDirt2Character>(HitResult.GetActor());
		
		if (bIsAttacking) {
			UE_LOG(LogTemp, Warning, TEXT("Entra"));

			if (HitActor)
			{
				UE_LOG(LogTemp, Warning, TEXT("Hit AOnDirtCharacter"));

				bIsAttacking = true;
				HitActor->GetDamage();
				GetWorld()->GetTimerManager().SetTimer(StopTimerHandle, this, &AEnemyCharacter::ResetAttacked, 0.5f, false);
				//UE_LOG(LogTemp, Warning, TEXT("bIsAttacked: %s"), bIsAttacked ? TEXT("true") : TEXT("false"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Hit something else"));
			}

			UE_LOG(LogTemp, Warning, TEXT("Hit called"));

		}

		//GetWorld()->GetTimerManager().SetTimer(StopTimerHandle, this, &ANewEnemyCharacter::ResetAttacked, 1.f, false);
	}
	//UE_LOG(LogTemp, Warning, TEXT("FunctionCalled"));

// Opcional: dibujar la línea para depuración
	DrawDebugSphere(GetWorld(), HitResult.Location, 20.0f, 2, FColor::Green, false, -50.0f);





}


/*void AEnemyCharacter::ResetAttacked() {

	bIsAttacking = false;

}

void AEnemyCharacter::SetPlayerDeath(bool bNewPlayerDead) {

	PlayerDeath(bNewPlayerDead);

}

void AEnemyCharacter::PlayerDeath(bool bPlayerDead) {

	if (bPlayerDead) {

		GetCharacterMovement()->DisableMovement();

	}

}

void AEnemyCharacter::SetAttack(bool Value) {

	bIsStopAttack = Value;
	UE_LOG(LogTemp, Warning, TEXT("Entra al SetAttack..."));

	if (bIsStopAttack) {

		GetCharacterMovement()->DisableMovement();

	}



}*/



//Anims

void AEnemyCharacter::SetIsAttack(bool Value)
{
	bIsAttacking = Value;



}

bool AEnemyCharacter::GetIsAttack() const {


	return bIsAttacking;

}


void AEnemyCharacter::SetIsAlert(bool Value)
{
	bIsAlert = Value;

}

bool AEnemyCharacter::GetIsAlert() const {


	return bIsAlert;

}


void AEnemyCharacter::SetIsLooking(bool Value)
{
	bIsLooking = Value;

}

bool AEnemyCharacter::GetIsLooking() const {


	return bIsLooking;

}