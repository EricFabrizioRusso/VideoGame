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
#include "EnemyAIController.h"

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
	bIsDamage = false;
	bIsDie = false;
	bResetAttack = false;
	bResetDamage = false;
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
			//UE_LOG(LogTemp, Warning, TEXT("Entra"));

			if (HitActor)
			{
				if (!bResetAttack) {

					if (!bIsDie) {


						UE_LOG(LogTemp, Warning, TEXT("Hit AOnDirtCharacter"));
						HitActor->GetDamage();
						bResetAttack = true;
						GetWorld()->GetTimerManager().SetTimer(StopTimerHandle, this, &AEnemyCharacter::ResetAttack, 1.5f, false);
					}




				}
			}
			else
			{
				
				
				UE_LOG(LogTemp, Warning, TEXT("Hit something else"));
			}

			UE_LOG(LogTemp, Warning, TEXT("Hit called"));

		}

		
	}
	//UE_LOG(LogTemp, Warning, TEXT("FunctionCalled"));

// Opcional: dibujar la línea para depuración
	DrawDebugSphere(GetWorld(), HitResult.Location, 20.0f, 2, FColor::Green, false, -50.0f);





}

void AEnemyCharacter::ResetAttack() {

	bResetAttack = false;

}

void AEnemyCharacter::ResetTakingDamage() {

	bResetDamage = false;

}



void AEnemyCharacter::GetEnenmyDamage(const FString& Weapon) {

	if (Weapon == "Gun") {


		if (Life >= 0.f) {

			if (!bResetDamage) {

				Life= Life - 50.f;
				bIsDamage = true;
				bIsAlert = false;
				bIsLooking = false;
				bIsAttacking = false;
				bResetDamage = true;
				GetWorld()->GetTimerManager().SetTimer(StopTimerHandle, this, &AEnemyCharacter::ResetTakingDamage, 1.f, false);

			}

		}
		else {

		
			bIsDie = true;
			UE_LOG(LogTemp, Warning, TEXT("Enemigo Muertooooo"));
			SetActorEnableCollision(false);


			AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
			if (AIController)
			{
				AIController->StopMovement();
				//AIController->Unpossess(); // Desconecta el controlador del enemigo

				UBehaviorTreeComponent* BehaviorTreeComp = Cast<UBehaviorTreeComponent>(AIController->GetBrainComponent());
				if (BehaviorTreeComp)
				{
					BehaviorTreeComp->StopTree(); // Detiene el árbol de comportamiento
				}
			}

		}

	}

	if (Weapon == "Melee") {


		if (Life >= 0.f) {

			if (!bResetDamage) {

				Life = Life - 50.f;
				bIsDamage = true;
				bIsAlert = false;
				bIsLooking = false;
				bResetDamage = true;
				GetWorld()->GetTimerManager().SetTimer(StopTimerHandle, this, &AEnemyCharacter::ResetTakingDamage, 1.f, false);

			}

		}
		else {


			bIsDie = true;
			UE_LOG(LogTemp, Warning, TEXT("Enemigo Muertooooo"));
			SetActorEnableCollision(false);

			UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
			if (MovementComponent)
			{
				MovementComponent->DisableMovement(); // Desactiva el movimiento
			}
			
			AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
			if (AIController)
			{
				AIController->StopMovement();
				//AIController->Unpossess(); // Desconecta el controlador del enemigo

				UBehaviorTreeComponent* BehaviorTreeComp = Cast<UBehaviorTreeComponent>(AIController->GetBrainComponent());
				if (BehaviorTreeComp)
				{
					BehaviorTreeComp->StopTree(); // Detiene el árbol de comportamiento
				}
			}


		}
	}


}



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

//Damage

void AEnemyCharacter::SetIsDamage(bool Value) {


	bIsDamage= Value;

}

bool AEnemyCharacter::GetIsDamage() const {


	return bIsDamage;

}

//Death

bool AEnemyCharacter::GetIsDie() const {


	return bIsDie;

}