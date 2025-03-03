// Copyright Epic Games, Inc. All Rights Reserved.

#include "OnDirt2Character.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ThrowOBJ.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "FixedCamera.h"
#include "Components/PostProcessComponent.h"
#include "MyPostProcessVolume.h"
#include "CameraShakeComponent.h"
#include "EnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EngineUtils.h"
#include "Blueprint/UserWidget.h"
#include "NotesToRead.h"
#include "NotesToReadWidget.h"
#include "PickUpItemWidget.h"
#include "InventoryHUDWidget.h"
#include "InventoryItems.h"
#include "HealthItem.h"
#include "InventoryComponent.h"
#include "InventoryItemWidget.h"
#include "DetailsItemWidget.h"
#include "Components/TextBlock.h"
#include "ItemsOptionsWidget.h"
#include "PistolItem.h"
#include "MeleeGunItem.h"
#include "Math/Vector.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Hearing.h"
#include "GameOverWidget.h"
#include "EnvironmentWidget.h"
#include "NotificationWidget.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Environment.h"
#include "Components/PointLightComponent.h"
#include "SaveGameCheckPoint.h"
#include "CheckPoint.h"
#include "AmmoItem.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "PauseMenuWidget.h"
#include "AccessLevelTriggerActor.h"


//Sound
#include "Sound/SoundCue.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AOnDirt2Character

AOnDirt2Character::AOnDirt2Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	   // Crear e inicializar el componente de inventario
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));


	bCanCrouch = false;
	bIsHoldingObject = false;
	HeldObject = nullptr;
	bIsAiming = false;
	bIsThrowing = false;
	bIsPickUp = false;
	bIsHit = false;
	bIsDie = false;
	bAllowPause = false;
	bAllowInventary = false;
	TextNote = nullptr;
	bMeleeGun = false;
	bEquipedGun = false;
	bEquipedMeleeGun = false;
	bAimingPistol= false;
	bIsShooting = false;
	bShootingPistol = false;
	bIsShootingCrouch =false;
	bIsBatIdle= false;
	bIsBating= false;
	bAimingMeleeGun = false;
	bIsTakingDamage = false;
	ResetMeleeDamage = false;
	bIsReloading = false;
	//LookUpDown
	bIsAimingUp = false;
	bIsAimingDown = false;

	//Language=
	Language = false;


	//FirstP
	FirstPersonActivate = false;

	//Stats
	Life = 100;




}

void AOnDirt2Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	PrimaryActorTick.bCanEverTick = true; // Habilita el Tick
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if (PauseMenuClass)
	{
		PauseMenu = CreateWidget<UPauseMenuWidget>(GetWorld(), PauseMenuClass);
	}
	if (OptionsMenuClass) {

		OptionsMenu = CreateWidget<UUserWidget>(GetWorld(), OptionsMenuClass);
	}
	if (InventoryHUDWidget) {

		InventoryHUDWidget = CreateWidget<UInventoryHUDWidget>(GetWorld(), InventoryHUDWidgetClass);

	}

	// Buscar si el personaje ya est� dentro de un volumen de c�mara fija
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, AFixedCamera::StaticClass());

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor->IsA(AFixedCamera::StaticClass()))
		{
			FixedCameraActor = Cast<AFixedCamera>(Actor);
			bUsingFixedCamera = true;

			// Cambiar a la c�mara fija inmediatamente
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
			if (PlayerController)
			{
				PlayerController->SetViewTarget(FixedCameraActor);
			}

			UE_LOG(LogTemp, Warning, TEXT("C�mara fija activada en BeginPlay con %s"), *FixedCameraActor->GetName());
			break; // Solo tomar la primera c�mara encontrada
		}
	}



	//Language
	FString SavedLanguage;
	if (GConfig->GetString(TEXT("/Script/Engine.GameUserSettings"), TEXT("SelectedLanguage"),
		SavedLanguage, GGameUserSettingsIni))
	{
		if (SavedLanguage == "Spanish") {

			Language= true;
		}
		else {

			Language = false;

		}

	}
	else
	{
		Language = false;
	}





	GetCharacterMovement()->MaxAcceleration = 600.f; // Ajusta seg�n la suavidad deseada

	// Ajustar la desaceleraci�n al caminar
	GetCharacterMovement()->BrakingDecelerationWalking = 100.f;

	GetWorldTimerManager().SetTimer(
		ThrowTimerHandle,  // Identificador del temporizador
		this,                     // Objeto al que pertenece la funci�n
		&AOnDirt2Character::EnablePause, // Funci�n que ser� llamada
		2.0f,                     // Tiempo de espera
		false                     // No se repite
	);


}
	





void AOnDirt2Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Detectar si el personaje est� quieto
	FVector Velocity = GetVelocity();
	if (Velocity.SizeSquared() == 0.0f) // Si no se mueve
	{
		GetCharacterMovement()->MaxWalkSpeed = 200.f; // Resetear velocidad
	}
}

void AOnDirt2Character::EnablePause() {

	bAllowPause = true;
	bAllowInventary = true;

}

//////////////////////////////////////////////////////////////////////////
// Input

void AOnDirt2Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AOnDirt2Character::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AOnDirt2Character::Look);

		//Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AOnDirt2Character::Sprinting); //Shift

		//Crouch
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AOnDirt2Character::Crouching); //C

		//Grab
		EnhancedInputComponent->BindAction(GrabAction, ETriggerEvent::Started, this, &AOnDirt2Character::PickUpHandle);
		//EnhancedInputComponent->BindAction(GrabAction, ETriggerEvent::Completed, this, &AOnDirt2Character::GrabThrowOBJ);
		
		//Aim
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Ongoing, this, &AOnDirt2Character::Aiming);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Canceled, this, &AOnDirt2Character::StopAiming);
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Triggered, this, &AOnDirt2Character::ThrowOBJ);


		//AimingView
		EnhancedInputComponent->BindAction(AimingUpAction, ETriggerEvent::Ongoing, this, &AOnDirt2Character::AimingUp);
		EnhancedInputComponent->BindAction(AimingUpAction, ETriggerEvent::Canceled, this, &AOnDirt2Character::CancelLookUp);
		EnhancedInputComponent->BindAction(AimingDownAction, ETriggerEvent::Ongoing, this, &AOnDirt2Character::AimingDown);
		EnhancedInputComponent->BindAction(AimingDownAction, ETriggerEvent::Canceled, this, &AOnDirt2Character::CancelLookDown);

		//Reload
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &AOnDirt2Character::ReloadWeapon); //R

		//FirstPersonView
		EnhancedInputComponent->BindAction(FirstPersonAction, ETriggerEvent::Triggered, this, &AOnDirt2Character::SetFirstPerson); //V

		//Menu Pause
		EnhancedInputComponent->BindAction(MenuPause, ETriggerEvent::Triggered, this, &AOnDirt2Character::MenuPauseExec);

		//Inventory
		EnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Triggered, this, &AOnDirt2Character::ShowInventory);


		//Restart
		EnhancedInputComponent->BindAction(RestartAction, ETriggerEvent::Triggered, this, &AOnDirt2Character::Respawn);

		




	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AOnDirt2Character::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (bIsDie || bIsHit || bIsReloading || FirstPersonActivate) return;

	if (Controller != nullptr)
	{
		if (!bAimingPistol)
		{

			FRotator YawRotation;
			// Movimiento tipo tanque (no est� apuntando)
			// Verifica si se est� utilizando una c�mara fija
			if (bUsingFixedCamera && FixedCameraActor)
			{
				// Obtiene la rotaci�n de la c�mara fija
				YawRotation = FixedCameraActor->GetActorRotation();
			}
			else
			{
				// Obtiene la rotaci�n del controlador (c�mara libre)
				const FRotator Rotation = Controller->GetControlRotation();
				YawRotation = FRotator(0, Rotation.Yaw, 0); // Solo la rotaci�n en el eje Yaw
			}

			// Obtiene el vector hacia adelante basado en la rotaci�n
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

			// Obtiene el vector hacia la derecha basado en la rotaci�n
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			// Aqu� se usan los inputs de movimiento de forma independiente para adelante/atr�s y izquierda/derecha
			// Aseg�rate de que MovementVector.Y se use para el movimiento hacia adelante/atr�s
			// y MovementVector.X para el movimiento hacia los lados (izquierda/derecha)

			// Para controles tipo tanque, el movimiento en la direcci�n Y (hacia adelante) sigue siendo relativo a la c�mara,
			// pero el movimiento en la direcci�n X (hacia los lados) es fijo en el espacio global.

			AddMovementInput(ForwardDirection, MovementVector.Y);  // Movimiento hacia adelante/atr�s
			AddMovementInput(RightDirection, MovementVector.X);    // Movimiento hacia los lados (izquierda/derecha)
		}
		else // Si el jugador est� apuntando
		{


			// Solo permite rotaci�n sobre su propio eje, sin movimiento hacia adelante/atr�s
			FRotator YawRotation;

			// Si se est� utilizando una c�mara fija, obtenemos la rotaci�n de la c�mara
			if (bUsingFixedCamera && FixedCameraActor)
			{
				YawRotation = FixedCameraActor->GetActorRotation();
			}
			else
			{
				// Si no, obtenemos la rotaci�n del controlador (la rotaci�n de la c�mara)
				const FRotator Rotation = Controller->GetControlRotation();
				YawRotation = FRotator(0, Rotation.Yaw, 0);  // Solo Yaw para mantener la rotaci�n en el plano horizontal
			}

			// Aqu�, no calculamos la direcci�n desde la c�mara, solo la direcci�n derecha basada en las teclas A/D
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			// Solo permite el movimiento lateral (izquierda/derecha) mientras apunta
			AddMovementInput(RightDirection, MovementVector.X);  // Movimiento lateral (izquierda/derecha)

			// Rotaci�n del personaje mientras apunta (con teclas A/D)
			if (MovementVector.X != 0.f)  // Si el jugador se mueve hacia la izquierda (A) o hacia la derecha (D)
			{
				FRotator CurrentRotation = GetActorRotation();  // Obtener la rotaci�n actual del actor

				// Rotar directamente sobre el eje Yaw (vertical) para rotar sobre el eje propio
				float RotationAmount = (MovementVector.X > 0.f) ? 2.f : -2.f;  // Gira 2 grados por cada movimiento, en lugar de 5

				FRotator TargetRotation = FRotator(0.f, CurrentRotation.Yaw + RotationAmount, 0.f); // Gira 2 grados por cada movimiento

				// Suavizar la transici�n de rotaci�n (esto hace que el giro no sea instant�neo)
				FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), 50.f);  // Reduce el valor de 5.f a 2.f para que gire m�s lentamente

				// Establecer la nueva rotaci�n
				SetActorRotation(NewRotation);
			}



		}
	}

}



	







void AOnDirt2Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (bIsDie) return;

	if (Controller != nullptr)
	{


			// add yaw and pitch input to controller
			AddControllerYawInput(LookAxisVector.X);
			AddControllerPitchInput(LookAxisVector.Y);


	
	}




}

void  AOnDirt2Character::Sprinting() {

	GetCharacterMovement()->MaxWalkSpeed = 500.f;
}

void  AOnDirt2Character::Crouching() {

	if (!bCanCrouch) {

		Crouch();
		bCanCrouch = true;
	}
	else {

		UnCrouch();
		bCanCrouch = false;
	}
	
	
}

void AOnDirt2Character::ThrowOBJ() {

	if (!bShootingPistol) {

		if (bEquipedGun && bAimingPistol) {

			
			if (HeldPistol->Ammunation != 0) {

			

				HeldPistol->Ammunation--;
				PerformGunTrace();
				UE_LOG(LogTemp, Warning, TEXT("Dispara dispara"));
				bShootingPistol = true;


				GetWorld()->GetTimerManager().SetTimer(ThrowTimerHandle, this, &AOnDirt2Character::ResetShootingFlag, 1.f, false);

			}
			else {


				UE_LOG(LogTemp, Warning, TEXT("No Ammo"));

				

			}

		}

	}

	if (!bIsBating) {

		if (bEquipedMeleeGun && bIsBatIdle) {

			//PerformBatTrace();
			UE_LOG(LogTemp, Warning, TEXT("batea batea"));
			bIsBating = true;
			GetWorld()->GetTimerManager().SetTimer(ThrowTimerHandle, this, &AOnDirt2Character::ResetBatFlag, 1.2f, false);

		}

	}


	if (HeldObject && bIsAiming)
	{
		bIsThrowing = true;
		GetWorld()->GetTimerManager().SetTimer(ThrowTimerHandle, this, &AOnDirt2Character::ExecuteThrow, 0.4f, false);



	}




}


void AOnDirt2Character::ResetShootingFlag()
{
	bShootingPistol = false; 


}

void AOnDirt2Character::ResetBatFlag()
{
	bIsBating = false;
}


void AOnDirt2Character::ExecuteThrow() {


	//Direccion del personaje
// Obtener la direcci�n en la que el personaje est� mirando
	HeldObject->bIsRealising = false;
	FVector CharacterForward = GetActorForwardVector();
	FVector CharacterUp = GetActorUpVector();
	FVector ImpulseDirection = CharacterForward + (CharacterUp * 0.5f); // Ajusta la direcci�n si es necesario

	//HeldObject->bIsRealising = false;
	// Desanclar el objeto de la mano
	HeldObject->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// Hacer visible el objeto nuevamente
	//HeldObject->SetActorHiddenInGame(false);

	// Habilitar f�sica y colisiones nuevamente
	HeldObject->Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HeldObject->SetActorEnableCollision(true);
	HeldObject->Mesh->SetSimulatePhysics(true);

	// Aplicar una fuerza para lanzar el objeto
	if (HeldObject->Mesh)
	{
		// Aplicar una fuerza en la direcci�n calculada
		HeldObject->Mesh->AddImpulse(ImpulseDirection * 1000, NAME_None, true);
	}

	// Limpiar la referencia al objeto sostenido
	HeldObject = nullptr;
	OverlappingThrowOBJ = nullptr;
	bIsAiming = false;
	






}

void  AOnDirt2Character::Aiming() {

	if (bEquipedGun && !bIsCrouched) {

		if (FirstPersonActivate) return;

		if (!bIsTakingDamage) {

			bAimingPistol = true;
			//UE_LOG(LogTemp, Warning, TEXT("Apunta."));

			UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
			if (MovementComponent)
			{
				MovementComponent->DisableMovement(); // Desactiva el movimiento
			}


		}


	}

	if (bEquipedMeleeGun && !bIsCrouched) {

		if (FirstPersonActivate) return;

		if (!bIsTakingDamage) {

			//bAimingMeleeGun = true;
			bIsBatIdle = true;
			//UE_LOG(LogTemp, Warning, TEXT("Apunta Melee."));

			UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
			if (MovementComponent)
			{
				MovementComponent->DisableMovement(); // Desactiva el movimiento
			}


		}

	}

	
	if (HeldObject) {


		UCharacterMovementComponent* Movement = GetCharacterMovement();
		Movement->DisableMovement();
		//UE_LOG(LogTemp, Warning, TEXT("Apuntando"));

		// Direccion del personaje
		FVector CharacterForward = GetActorForwardVector();
		FVector CharacterUp = GetActorUpVector();
		FVector ImpulseDirection = CharacterForward + (CharacterUp * 0.5f); // Ajusta la direcci�n si es necesario

		// Par�metros de simulaci�n
		const float SimulationTimeStep = 0.005f; // Tiempo entre cada punto de la simulaci�n
		const int32 MaxSimulationSteps = 200; // N�mero m�ximo de pasos de simulaci�n
		const float Gravity = GetWorld()->GetGravityZ(); // Obtener la gravedad del mundo

		FVector StartLocation = HeldObject->GetActorLocation();
		FVector Velocity = ImpulseDirection * 1000;
		//FVector ImpactPoint;
		bIsAiming = true;




		for (int32 i = 0; i < MaxSimulationSteps; ++i)
		{
			float Time = i * SimulationTimeStep;
			FVector Position = StartLocation + (Velocity * Time) + (0.5f * FVector(0, 0, Gravity) * Time * Time);
			FVector NextPosition = StartLocation + (Velocity * (Time + SimulationTimeStep)) + (0.5f * FVector(0, 0, Gravity) * (Time + SimulationTimeStep) * Time);

			// Dibujar la l�nea de trayectoria
			DrawDebugLine(GetWorld(), Position, NextPosition, FColor::Green, false, 0.01f, 0, 2.0f);



		}

	}


}

void AOnDirt2Character::AimingUp() {

	if (!bAimingPistol) return;
	bIsAimingUp = true;

}
void AOnDirt2Character::AimingDown() {

	if (!bAimingPistol) return;
	bIsAimingDown = true;
}
void AOnDirt2Character::CancelLookUp() {

	//UE_LOG(LogTemp, Warning, TEXT("Deja de Apuntar Arriba"));
	bIsAimingUp = false;

}
void AOnDirt2Character::CancelLookDown() {

	//UE_LOG(LogTemp, Warning, TEXT("Deja de Apuntar Abajo"));
	bIsAimingDown = false;

}

void AOnDirt2Character::ReloadWeapon() {

	if (!bAimingPistol) return;



	if (bEquipedGun){

		if (HeldPistolAmmo != nullptr) {

			int32 MaxClipSize = 5;
			int32 AmmoNeeded = MaxClipSize - HeldPistol->Ammunation; // Balas necesarias

			if (HeldPistolAmmo->ItemData.Quantity > 0) // Verifica que haya munici�n disponible
			{
				int32 AmmoToReload = FMath::Min(AmmoNeeded, HeldPistolAmmo->ItemData.Quantity); // Cantidad real de balas a recargar

				// Recargar balas
				HeldPistol->Ammunation += AmmoToReload;
				HeldPistolAmmo->ItemData.Quantity -= AmmoToReload;

				UE_LOG(LogTemp, Warning, TEXT("Recargado: %d balas. Munici�n restante: %d"), AmmoToReload, HeldPistolAmmo->ItemData.Quantity);


				// Si la munici�n llega a 0, eliminarla del inventario





			}
			else
			{


				if (HeldPistolAmmo->ItemData.Quantity <= 0) {


					if (InventoryComponent) {

						for (int32 i = 0; i < InventoryComponent->Inventory.Num(); i++)
						{
							if (InventoryComponent->Inventory[i].ItemName == "Pistol ammo" || InventoryComponent->Inventory[i].ItemName == "Municion de pistola")
							{
								InventoryComponent->Inventory.RemoveAt(i); // Eliminar el �tem del array
								HeldPistolAmmo->RemoveFromRoot();
								HeldPistolAmmo->Destroy();
								HeldPistolAmmo = nullptr;
								UE_LOG(LogTemp, Warning, TEXT("Item Eliminado"));
							}

						}


					}

				}





				UE_LOG(LogTemp, Warning, TEXT("No hay balas suficientes para recargar."));
			}




			

			     
		}
		else {

			UE_LOG(LogTemp, Warning, TEXT("HeldPistolAmmo es nulo"));

		}

	}
	



	UE_LOG(LogTemp, Warning, TEXT("Reloading Weapon"));
	bIsReloading = false;

}
void AOnDirt2Character::SetFirstPerson() {

	UE_LOG(LogTemp, Warning, TEXT("Presion� la tecla V"));

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!PlayerController) return;


	if (FirstPersonActivate) {

		FRotator YawRotation;
		if (PlayerController)
		{
			PlayerController->SetViewTarget(FixedCameraActor);
		}

		FirstPersonActivate = false;


	}else {

	
		if (PlayerController)
		{
			PlayerController->SetViewTarget(this);
		}

			FRotator YawRotation;
		FirstPersonActivate = true;


	}

	

}



void AOnDirt2Character::StopAiming()
{
	bIsAiming = false;
	bAimingPistol = false;
	bIsAimingDown = false;
	bIsAimingUp = false;
	bIsBatIdle = false;
	bIsBating = false;
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->SetMovementMode(MOVE_Walking);
	UE_LOG(LogTemp, Warning, TEXT("Deja de apuntar."));
}

void AOnDirt2Character::PerformGunTrace() {


	// Verificar que el socket exista en el SkeletalMesh
	if (!GetMesh() || !GetMesh()->DoesSocketExist(TEXT("HandGunSocket")))
	{
		UE_LOG(LogTemp, Warning, TEXT("HandGunSocket no existe en el SkeletalMesh del personaje."));
		return;
	}

	// Obtener la posici�n y rotaci�n del HandGunSocket
	FVector Start = GetMesh()->GetSocketLocation(TEXT("HandGunSocket"));
	FRotator SocketRotation = GetMesh()->GetSocketRotation(TEXT("HandGunSocket"));
	FVector ForwardVector = SocketRotation.Vector(); // Direcci�n hacia donde apunta el socket

	// Definir la distancia del raycast
	float TraceDistance = 600.0f;
	FVector End = Start + (ForwardVector * TraceDistance);

	// Configurar los par�metros del trazado
	FHitResult HitResult;
	FCollisionQueryParams TraceParams;
	TraceParams.bTraceComplex = true;
	TraceParams.AddIgnoredActor(this); // Ignorar al personaje

	// Ejecutar el Line Trace
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility, // Ajusta el canal de colisi�n seg�n tus necesidades
		TraceParams
	);


	UPointLightComponent* PointLight = NewObject<UPointLightComponent>(this);
	if (PointLight)
	{
		PointLight->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("HandGunSocket"));
		PointLight->SetIntensity(3000.0f); // Ajusta la intensidad
		PointLight->SetAttenuationRadius(200.0f); // Ajusta el radio
		PointLight->SetLightColor(FLinearColor::Yellow); // Color del destello
		PointLight->RegisterComponent();

		// Destruir el Point Light despu�s de 0.5 segundos
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			[PointLight]()
			{
				if (PointLight)
				{
					PointLight->DestroyComponent();
				}
			},
			0.2f, // Duraci�n del destello
			false
		);
	}


	// Dibujar l�neas de depuraci�n para visualizar el trazado
	if (bHit)
	{
		DrawDebugLine(GetWorld(), Start, HitResult.Location, FColor::Red, false, 5.0f, 0, 1.0f);

		if (HitResult.GetActor())
		{
			// Verificar si el actor impactado es un AEnemyCharacter
			AEnemyCharacter* HitEnemy = Cast<AEnemyCharacter>(HitResult.GetActor());
			if (HitEnemy)
			{
				UE_LOG(LogTemp, Log, TEXT("�Enemigo alcanzado! %s"), *HitEnemy->GetName());
				HitEnemy->GetEnenmyDamage("Gun");


				if (BloodEffect)
				{
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(
						GetWorld(),
						BloodEffect,
						HitResult.Location,         // Ubicaci�n del impacto
						HitResult.ImpactNormal.Rotation() // Rotaci�n basada en la normal de la superficie
					);
				}




			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Impacto con un objeto no v�lido: %s"), *HitResult.GetActor()->GetName());
			}
		}
	}
	else
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 5.0f, 0, 1.0f);
		UE_LOG(LogTemp, Log, TEXT("No se impact� con ning�n objeto."));
	}

	//Camera Shake
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController && PlayerController->PlayerCameraManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("Shaking making"));
		PlayerController->PlayerCameraManager->StartCameraShake(UCameraShakeComponent::StaticClass());
	}




	if (LineShootEffect) {


		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			LineShootEffect,
			Start,         // Ubicaci�n del impacto
			HitResult.ImpactNormal.Rotation() // Rotaci�n basada en la normal de la superficie
		);

	}





	// Generar un est�mulo de sonido
	UAISense_Hearing::ReportNoiseEvent(
		GetWorld(),
		Start,                 // Posici�n del ruido
		1.0f,                  // Intensidad del ruido
		this,                  // Instigador (qui�n hace el ruido)
		0.0f,                  // Duraci�n del ruido
		TEXT("GunshotNoise")   // Tag opcional para identificar el tipo de ruido
	);


}

void AOnDirt2Character::PerformBatTrace() {


	// Verificar que el Mesh y el Socket existan
	if (!GetMesh() || !GetMesh()->DoesSocketExist(TEXT("MeleeSocket")))
	{
		UE_LOG(LogTemp, Warning, TEXT("HandGunSocket no existe en el SkeletalMesh del personaje."));
		return;
	}


	FVector HandLocation = GetMesh()->GetSocketLocation("MeleeSocket");
	FVector EndLocation = HandLocation + GetActorForwardVector() * 2000.f;


	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this); // Ignorar al propio enemigo
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, HandLocation, EndLocation, ECC_Visibility, CollisionParams);


	if (bHit)
	{
		AEnemyCharacter* HitActor = Cast<AEnemyCharacter>(HitResult.GetActor());


			UE_LOG(LogTemp, Warning, TEXT("Entra"));

			if (HitActor)
			{
				

					if (!bIsDie) {



						APlayerController* PlayerController = Cast<APlayerController>(GetController());
						if (PlayerController && PlayerController->PlayerCameraManager)
						{
							UE_LOG(LogTemp, Warning, TEXT("Shaking making"));
							PlayerController->PlayerCameraManager->StartCameraShake(UCameraShakeComponent::StaticClass());
						}

						UE_LOG(LogTemp, Warning, TEXT("Hit Enemy"));
						HitActor->GetEnenmyDamage("Melee");

						if (BloodEffect)
						{
							UNiagaraFunctionLibrary::SpawnSystemAtLocation(
								GetWorld(),
								BloodEffect,
								HitResult.Location,         // Ubicaci�n del impacto
								HitResult.ImpactNormal.Rotation() // Rotaci�n basada en la normal de la superficie
							);
						}



					}


				
			}
			else
			{


				UE_LOG(LogTemp, Warning, TEXT("Hit something else"));
			}

			UE_LOG(LogTemp, Warning, TEXT("Hit called"));

		


	}
	//UE_LOG(LogTemp, Warning, TEXT("FunctionCalled"));

	// Opcional: dibujar la l�nea para depuraci�n





}

void  AOnDirt2Character::PickUpHandle() {


	if (HeldObject != nullptr) {

		DropOBJ();

	}
	else {

		GrabThrowOBJ();
	}


}


void AOnDirt2Character::GrabThrowOBJ()
{

	if (OverlappingNote && OverlappingNote->bCanBeGrabbed) {

		HeldNote = OverlappingNote;


			if (PickUpItemClass)
			{
				// Crear el widget y castear al tipo personalizado
				UUserWidget* WidgetPickUp = CreateWidget<UUserWidget>(GetWorld(), PickUpItemClass);
				PickUpItemWidget = Cast<UPickUpItemWidget>(WidgetPickUp);

				if (PickUpItemWidget)
				{
					PickUpItemWidget->SetItemName(HeldNote->NoteName);


					PickUpItemWidget->AddToViewport();
					GetWorld()->GetFirstPlayerController()->SetPause(true);
					GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
					UE_LOG(LogTemp, Log, TEXT("Objecto a�adido"));
				}
			}
			
	


		
		


	}

	if (OverlappingPistolAmmo && OverlappingPistolAmmo->bCanBeGrabbed) {

		HeldPistolAmmo = OverlappingPistolAmmo;


		if (PickUpItemClass)
		{
			// Crear el widget y castear al tipo personalizado
			UUserWidget* WidgetPickUp = CreateWidget<UUserWidget>(GetWorld(), PickUpItemClass);
			PickUpItemWidget = Cast<UPickUpItemWidget>(WidgetPickUp);

			if (PickUpItemWidget)
			{
				PickUpItemWidget->SetItemName(HeldPistolAmmo->ItemData.ItemName.ToString());


				PickUpItemWidget->AddToViewport();
				GetWorld()->GetFirstPlayerController()->SetPause(true);
				GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
				UE_LOG(LogTemp, Log, TEXT("Objecto a�adido"));
			}
		}








	}



	if (OverlappingEnvironment && OverlappingEnvironment->bCanBeGrabbed) {

		if (!EnvironmentWidget) {

			EnvironmentWidget = CreateWidget<UEnvironmentWidget>(GetWorld(), EnvironmentWidgetClass);

		}

		if (EnvironmentWidget && !EnvironmentWidget->IsInViewport()) {
			
			EnvironmentWidget->DescriptionEnvironment->SetText(FText::FromString(OverlappingEnvironment->EnvironmentString));
			EnvironmentWidget->AddToViewport();
			GetWorld()->GetTimerManager().SetTimer(NotiTimerHandle, this, &AOnDirt2Character::RemoveWidgets, 1.5f, false);

		}




	}

	if (OverlappingHealthItem && OverlappingHealthItem->bCanBeGrabbed) {

		HeldHealth = OverlappingHealthItem;

		UUserWidget* WidgetHealth = CreateWidget<UUserWidget>(GetWorld(), PickUpItemClass);
		PickUpItemWidget = Cast<UPickUpItemWidget>(WidgetHealth);

		if (PickUpItemWidget)
		{
			PickUpItemWidget->SetItemName(HeldHealth->ItemData.ItemName.ToString());
			PickUpItemWidget->AddToViewport();
			GetWorld()->GetFirstPlayerController()->SetPause(true);
			GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
			UE_LOG(LogTemp, Log, TEXT("Objecto a�adido"));
		}



	}

	if (OverlappingMeleeGunItem && OverlappingMeleeGunItem->bCanBeGrabbed) {

		HeldMeleeGunItem = OverlappingMeleeGunItem;

		UUserWidget* WidgetMeleeGunItem = CreateWidget<UUserWidget>(GetWorld(), PickUpItemClass);
		PickUpItemWidget = Cast<UPickUpItemWidget>(WidgetMeleeGunItem);

		if (PickUpItemWidget)
		{
			PickUpItemWidget->SetItemName(HeldMeleeGunItem->ItemData.ItemName.ToString());
			PickUpItemWidget->AddToViewport();
			GetWorld()->GetFirstPlayerController()->SetPause(true);
			GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
			UE_LOG(LogTemp, Log, TEXT("Melee gun a�adida"));
		}


	}

	if (OverlappingPistolItem && OverlappingPistolItem->bCanBeGrabbed) {

		HeldPistol = OverlappingPistolItem;

		UUserWidget* WidgetPistolItem = CreateWidget<UUserWidget>(GetWorld(), PickUpItemClass);
		PickUpItemWidget = Cast<UPickUpItemWidget>(WidgetPistolItem);

		if (PickUpItemWidget)
		{
			PickUpItemWidget->SetItemName(HeldPistol->ItemData.ItemName.ToString());
			PickUpItemWidget->AddToViewport();
			GetWorld()->GetFirstPlayerController()->SetPause(true);
			GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
			UE_LOG(LogTemp, Log, TEXT("Pistola a�adida"));
		}


	}



	if (OverlappingThrowOBJ && OverlappingThrowOBJ->bCanBeGrabbed)
	{

		if (bEquipedGun || bEquipedMeleeGun) return;

		HeldObject = OverlappingThrowOBJ;
		OverlappingThrowOBJ->Mesh->SetSimulatePhysics(false);
		OverlappingThrowOBJ->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("HandSocket"));
		OverlappingThrowOBJ->SetActorEnableCollision(false);
		OverlappingThrowOBJ = nullptr;
		bIsPickUp = true;
		
	
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No hay un objeto v�lido para agarrar"));
	}

	if (OverlappingAccessToLevel && OverlappingAccessToLevel->bIsOverlaped) {


		if (OverlappingAccessToLevel->bIsLocked) {


			for (int32 i = 0; i < InventoryComponent->Inventory.Num(); i++)
			{
				if (InventoryComponent->Inventory[i].ItemName == OverlappingAccessToLevel->AccessDoor)
				{
					InventoryComponent->Inventory.RemoveAt(i); // Eliminar el �tem del array
					HeldMeleeGunItem->RemoveFromRoot();
					HeldMeleeGunItem->Destroy();
					HeldMeleeGunItem = nullptr;
					UE_LOG(LogTemp, Warning, TEXT("Item Eliminado"));



					if (!NotificationWidget) {

						NotificationWidget = CreateWidget<UNotificationWidget>(GetWorld(), NotificationWidgetClass);

						UE_LOG(LogTemp, Error, TEXT("Se Crea Noti"));


					}

					if (NotificationWidget && !NotificationWidget->IsInViewport()) {

						if (Language) {

							


							NotificationWidget->DescriptionNotification->SetText(FText::FromString("Desbloqueado"));
			
							
						
						}
						else {


							NotificationWidget->DescriptionNotification->SetText(FText::FromString("Unlock"));
							

						}

						NotificationWidget->AddToViewport();
						UE_LOG(LogTemp, Error, TEXT("Se a�ade noti"));
						GetWorld()->GetTimerManager().SetTimer(NotiTimerHandle, this, &AOnDirt2Character::RemoveWidgets, 1.5f, false);
						
						OverlappingAccessToLevel->bIsLocked = false;
						return;
					}





				}

			}


			if (!NotificationWidget) {

				NotificationWidget = CreateWidget<UNotificationWidget>(GetWorld(), NotificationWidgetClass);

				UE_LOG(LogTemp, Error, TEXT("Se Crea Noti"));


			}

			if (NotificationWidget && !NotificationWidget->IsInViewport()) {


				if (Language) {

					NotificationWidget->DescriptionNotification->SetText(FText::FromString("Cerrado"));

				}
				else {

					NotificationWidget->DescriptionNotification->SetText(FText::FromString("Lock"));

				}
			

				NotificationWidget->AddToViewport();
				UE_LOG(LogTemp, Error, TEXT("Se a�ade noti"));
				GetWorld()->GetTimerManager().SetTimer(NotiTimerHandle, this, &AOnDirt2Character::RemoveWidgets, 1.5f, false);

				
			}








		}
		else {

			OverlappingAccessToLevel->ChangeLevel();


		}




	}




	


}

void AOnDirt2Character::SetToRead(bool Value) {


	if (Value) {

		if(HeldNote){

			if (ReadNoteClass) {

				//UUserWidget* WidgetReadNote = CreateWidget<UUserWidget>(GetWorld(), ReadNoteClass);
				//ReadNoteWidget = CreateWidget<UNotesToReadWidget>(GetWorld(), ReadNoteClass);
				//ReadNoteWidget = Cast<UNotesToReadWidget>(WidgetReadNote);

				PickUpItemWidget->RemoveFromParent();
				ReadNoteWidget = CreateWidget<UNotesToReadWidget>(GetWorld(), ReadNoteClass);
				ReadNoteWidget->SetNoteText(HeldNote->NoteText);
				ReadNoteWidget->AddToViewport();
				HeldNote = nullptr;
			}


		}

		if (HeldHealth) {


			InventoryComponent->PickUpItem(HeldHealth);
			//HeldHealth->Destroy();
			//HeldHealth->SetActorHiddenInGame(true);
			//HeldHealth->SetActorEnableCollision(false);
			//HeldHealth->SetActorTickEnabled(false);
			OverlappingHealthItem = nullptr;
			GetWorld()->GetFirstPlayerController()->SetPause(false);
			GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
		}


		if (OverlappingMeleeGunItem) {
			InventoryComponent->PickUpItem(HeldMeleeGunItem);

			HeldMeleeGunItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			HeldMeleeGunItem->SetActorEnableCollision(true); // Reactivar la colisi�n si es necesario
			HeldMeleeGunItem->SetActorHiddenInGame(true);   // Ocultar el arma en el mundo
			HeldMeleeGunItem->SetActorTickEnabled(false);
			OverlappingMeleeGunItem = nullptr;
			// A�adir al root para evitar eliminaci�n
			HeldMeleeGunItem->AddToRoot();

			//HeldMeleeGunItem = nullptr;
			GetWorld()->GetFirstPlayerController()->SetPause(false);
			GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;

			UE_LOG(LogTemp, Warning, TEXT("Agarra Melee"));


		}

		if (OverlappingPistolItem) {

			InventoryComponent->PickUpItem(HeldPistol);
			HeldPistol->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			HeldPistol->SetActorEnableCollision(false); // Reactivar la colisi�n si es necesario
			HeldPistol->SetActorHiddenInGame(true);   // Ocultar el arma en el mundo
			HeldPistol->SetActorTickEnabled(false);
			OverlappingPistolItem = nullptr;
			//HeldPistol = nullptr;

			HeldPistol->AddToRoot();

			GetWorld()->GetFirstPlayerController()->SetPause(false);
			GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;

			UE_LOG(LogTemp, Warning, TEXT("Agarra pistola"));
		}

		if (OverlappingPistolAmmo) {


			InventoryComponent->PickUpItem(HeldPistolAmmo);
			HeldPistolAmmo->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			HeldPistolAmmo->SetActorEnableCollision(false); // Reactivar la colisi�n si es necesario
			HeldPistolAmmo->SetActorHiddenInGame(true);   // Ocultar el arma en el mundo
			HeldPistolAmmo->SetActorTickEnabled(false);
			OverlappingPistolAmmo = nullptr;
			

			HeldPistolAmmo->AddToRoot();

			GetWorld()->GetFirstPlayerController()->SetPause(false);
			GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;

			UE_LOG(LogTemp, Warning, TEXT("Agarra municion"));


		}

	}
	else {


		if (OverlappingPistolItem) {


			if (HeldPistol) {

				HeldPistol = nullptr;
			}

		}

		if (OverlappingMeleeGunItem) {
			

			if (HeldMeleeGunItem) {

				HeldMeleeGunItem = nullptr;

			}

		}
		
		if (OverlappingHealthItem) {


			if (HeldHealth) {

				HeldHealth = nullptr;

			}

		}


	}

}


void AOnDirt2Character::DropOBJ() {

	if (HeldObject) // Aseg�rate de que hay un objeto sostenido
	{
		// Soltar el objeto
		HeldObject->bIsRealising = true;
		HeldObject->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// Reactivar f�sicas y colisi�n
		HeldObject->Mesh->SetSimulatePhysics(true);
		HeldObject->SetActorEnableCollision(true);

		// Limpiar la referencia y estado
		HeldObject = nullptr;

		UE_LOG(LogTemp, Warning, TEXT("Objeto soltado"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No hay ning�n objeto sostenido para soltar"));
	}



}

void AOnDirt2Character::GetDamage() {


	if (bIsTakingDamage) return;

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController && PlayerController->PlayerCameraManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("Shaking making"));
		PlayerController->PlayerCameraManager->StartCameraShake(UCameraShakeComponent::StaticClass());
	}



	bIsHit = true;
	bIsPickUp = false;
	bIsThrowing = false;
	bAimingPistol = false;
	bIsAimingUp = false;
	bIsAimingDown = false;
	bIsTakingDamage = true;
	bIsShooting = false;
	bIsBatIdle = false;
	bIsBating = false;
	GetWorld()->GetTimerManager().SetTimer(ThrowTimerHandle, this, &AOnDirt2Character::ResetTakingDamage, 1.5f, false);


	if (!Life == 0) {

		Life = Life - 50;
		UE_LOG(LogTemp, Warning, TEXT("Tienes vida"));

	}
	else {


		bIsDie = true;
		bIsHit = false;
		bIsPickUp = false;
		bIsThrowing = false;
		bAimingPistol = false;
		bIsAimingUp = false;
		bIsAimingDown = false;
		bIsTakingDamage = true;
		bIsShooting = false;
		bIsBatIdle = false;
		bIsBating = false;
		

		for (TActorIterator<AEnemyCharacter> It(GetWorld()); It; ++It)
		{
			AEnemyCharacter* Enemy = *It;
			if (Enemy && Enemy->GetController())
			{
				UBlackboardComponent* BlackboardComp = Enemy->GetController()->FindComponentByClass<UBlackboardComponent>();

				if (BlackboardComp)
				{
					BlackboardComp->SetValueAsBool(TEXT("bIsDead"), true);
				}
			}
		}

		/*TArray<AActor*> PostProcessVolumes;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), PostProcessVolumes);

		for (AActor* Actor : PostProcessVolumes)
		{
			APostProcessVolume* PostProcessVolume = Cast<APostProcessVolume>(Actor);
			if (PostProcessVolume)
			{
				// Obt�n el perfil de post-procesado actual
				FPostProcessSettings& PostProcessSettings = PostProcessVolume->Settings;

				// Modifica el efecto de ruido
				PostProcessSettings.FilmGrainIntensity = 2.f;
				UE_LOG(LogTemp, Warning, TEXT("CAMBIA FILTROOO"));

				// Aqu� puedes ajustar la configuraci�n del ruido seg�n sea necesario
				// Por ejemplo:
				// PostProcessSettings.GrainIntensity = 0.5f;
			}
		}*/


		if (!GameOverWidget) {

			GameOverWidget = CreateWidget<UGameOverWidget>(GetWorld(), GameOverWidgetClass);



		}

		if (GameOverWidget && !GameOverWidget->IsInViewport()) {


			GameOverWidget->AddToViewport();

			UE_LOG(LogTemp, Warning, TEXT("A�ade al viewport"));
		}

		UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
		if (MovementComponent)
		{
			MovementComponent->DisableMovement(); // Desactiva el movimiento
		}




		UE_LOG(LogTemp, Warning, TEXT("You're Dead"));


	}

}


void AOnDirt2Character::ResetTakingDamage() {

	bIsTakingDamage = false;
}

void AOnDirt2Character::SetLastCheckpointLocation(FVector NewLocation)
{
	LastCheckpointLocation = NewLocation;
}


void AOnDirt2Character::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("Character Died"));
	Respawn();
}

void AOnDirt2Character::Respawn()
{

	if (!bIsDie) return;

	AController* LocalController = GetController();
	if (LocalController)
	{
		// Respawn the character at the last checkpoint location
		SetActorLocation(LastCheckpointLocation);


		// Optionally, reset other necessary attributes like health, inventory, etc.
		// Example: Reset health
		// Health = MaxHealth;

		// Possess the character again
		Controller->Possess(this);
	}

	Life = 100;
	bIsDie = false;

	if (GameOverWidget && GameOverWidget->IsInViewport()) {


		GameOverWidget->RemoveFromParent();

		UE_LOG(LogTemp, Warning, TEXT("Quita al viewport"));

	}


	if (USaveGameCheckPoint* SaveGameInstance = Cast<USaveGameCheckPoint>(UGameplayStatics::LoadGameFromSlot(TEXT("CheckPointSaveSlot"), 0)))
	{
		// Restaurar la posici�n del jugador
		SetActorLocation(SaveGameInstance->PlayerLocation);

		// Restaurar el inventario
		if (InventoryComponent) // Acceso directo al componente
		{
			InventoryComponent->SetInventoryItems(SaveGameInstance->CollectedItems);
		}

		// Restaurar los enemigos vivos
		TArray<AActor*> EnemyActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacter::StaticClass(), EnemyActors);

		for (AActor* Enemy : EnemyActors)
		{
			if (AEnemyCharacter* EnemyActor = Cast<AEnemyCharacter>(Enemy))
			{
				// Comparar las ubicaciones para restaurar enemigos vivos
				for (const FVector& SavedLocation : SaveGameInstance->EnemyLocations)
				{
					if (FVector::Dist(SavedLocation, EnemyActor->GetActorLocation()) < 1.0f) // Tolerancia peque�a para evitar errores de precisi�n
					{
						EnemyActor->bIsDie= false; // M�todo personalizado para revivir al enemigo
						break;
					}
				}
			}
		}
	}
	else
	{
		// Si no hay datos de guardado, reiniciar nivel desde 0
		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
	}







		
}


void AOnDirt2Character::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (OtherActor->IsA(AThrowOBJ::StaticClass()))
	{
		OverlappingThrowOBJ = Cast<AThrowOBJ>(OtherActor);
		//UE_LOG(LogTemp, Warning, TEXT("OVERLAP"));
	}

	if (OtherActor->IsA(ANotesToRead::StaticClass())) {

		OverlappingNote = Cast<ANotesToRead>(OtherActor);
		//UE_LOG(LogTemp, Warning, TEXT("OVERLAP NOTE"));

	}

	if (OtherActor->IsA(AFixedCamera::StaticClass()))
	{
		FixedCameraActor = Cast<AFixedCamera>(OtherActor);
		bUsingFixedCamera = true;

		// Cambiar a la c�mara fija
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		if (PlayerController)
		{
			PlayerController->SetViewTarget(FixedCameraActor);
		}
	}

	if (OtherActor->IsA(AHealthItem::StaticClass()))
	{
		
		OverlappingHealthItem = Cast<AHealthItem>(OtherActor);
		//UE_LOG(LogTemp, Warning, TEXT("OVERLAP Health"));

	}

	if (OtherActor->IsA(AMeleeGunItem::StaticClass()))
	{

		OverlappingMeleeGunItem = Cast<AMeleeGunItem>(OtherActor);
		//UE_LOG(LogTemp, Warning, TEXT("OVERLAP MeleeGun"));

	}

	if (OtherActor->IsA(APistolItem::StaticClass()))
	{

		OverlappingPistolItem = Cast<APistolItem>(OtherActor);
		//UE_LOG(LogTemp, Warning, TEXT("OVERLAP FireGun"));

	}

	if (OtherActor->IsA(AEnvironment::StaticClass()))
	{

		OverlappingEnvironment = Cast<AEnvironment>(OtherActor);
		

	}

	if (OtherActor->IsA(AAmmoItem::StaticClass()))
	{

		OverlappingPistolAmmo = Cast<AAmmoItem>(OtherActor);


	}

	if (OtherActor->IsA(AAccessLevelTriggerActor::StaticClass()))
	{

		OverlappingAccessToLevel = Cast<AAccessLevelTriggerActor>(OtherActor);


	}


}

void AOnDirt2Character::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if (HeldObject == nullptr)
	{
	
		OverlappingThrowOBJ = nullptr;
		
		//UE_LOG(LogTemp, Warning, TEXT("FIN OVERLAP"));
	}

	if (HeldNote == nullptr) {

		OverlappingThrowOBJ = nullptr;

		//UE_LOG(LogTemp, Warning, TEXT("FIN OVERLAP NOTE"));

	}

	if (HeldHealth == nullptr) {

		OverlappingHealthItem = nullptr;

		//UE_LOG(LogTemp, Warning, TEXT("FIN OVERLAP NOTE"));

	}

	if (HeldMeleeGunItem == nullptr) {

		OverlappingHealthItem = nullptr;

		//UE_LOG(LogTemp, Warning, TEXT("FIN OVERLAP MeleeGun"));

	}

	if (HeldPistol == nullptr) {

		OverlappingPistolItem = nullptr;

		//UE_LOG(LogTemp, Warning, TEXT("FIN OVERLAP Gun"));
	}

	if (HeldPistolAmmo == nullptr) {

		OverlappingPistolAmmo = nullptr;

		//UE_LOG(LogTemp, Warning, TEXT("FIN OVERLAP Gun"));
	}


}

//Animations -----------------------------------------------

bool AOnDirt2Character::isAiming() const {


	return bIsAiming;

}

//Pistol

bool AOnDirt2Character::GetAimingGun() const {


	return bAimingPistol;

}

void AOnDirt2Character::SetShootingPistol(bool Value) {


	 bShootingPistol = Value;

}

bool AOnDirt2Character::GetShootingPistol() const {


	return bShootingPistol;

}

void AOnDirt2Character::SetShootingCrouch(bool Value) {


	bIsShootingCrouch = Value;

}


bool AOnDirt2Character::GetShootingCrouch() const {


	return bIsShootingCrouch;

}

void AOnDirt2Character::SetReloadAnimation(bool Value) {


	bIsReloading = Value;

}

bool AOnDirt2Character::GetReloadAnimation() const {

	return bIsReloading;

}



//Looking UP DOWN
void AOnDirt2Character::SetAimingGunUp(bool Value){


	bIsAimingUp= Value;

}

bool AOnDirt2Character::GetAimingGunUp()const {


	return bIsAimingUp;

}

void AOnDirt2Character::SetAimingGunDown(bool Value) {


	bIsAimingDown = Value;

}

bool AOnDirt2Character::GetAimingGunDown()const {


	return bIsAimingDown;

}


//MeleeGun
void AOnDirt2Character::SetBating(bool Value){


	bIsBating= Value;

}

bool AOnDirt2Character::GetBatIdle() const {


	return bIsBatIdle;

}


bool AOnDirt2Character::GetBating() const{


	return bIsBating;

}




//Throw Anim
void AOnDirt2Character::SetIsThrowing(bool Value) {


	bIsThrowing = Value;

}

bool AOnDirt2Character::GetIsThrowing() const {


	return bIsThrowing;

}

//PickUp Anim
void AOnDirt2Character::SetIsPickUp(bool Value) {


	bIsPickUp = Value;

}

bool AOnDirt2Character::GetIsPickUp() const {


	return bIsPickUp;

}

//Hit Anim
void AOnDirt2Character::SetIsHit(bool Value) {


	bIsHit = Value;

}

bool AOnDirt2Character::GetIsHit() const {


	return bIsHit;

}

//Die Anim
void AOnDirt2Character::SetIsDie(bool Value) {


	bIsDie = Value;

}

bool AOnDirt2Character::GetIsDie() const {


	return bIsDie;

}


//UI -----------------------------------------------------

void  AOnDirt2Character::MenuPauseExec() {

	if (InventoryHUDWidget && InventoryHUDWidget->IsInViewport()) return;

	if (bAllowPause) {


		if (OptionsMenu && OptionsMenu->IsInViewport()) {

			OptionsMenu->RemoveFromParent();
			GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;

			UE_LOG(LogTemp, Warning, TEXT("Volviendo al Pause Menu"));
		}

		if (PauseMenu && !PauseMenu->IsInViewport())
		{
			// Habilitar input del jugador durante la pausa


			PauseMenu->AddToViewport();
			GetWorld()->GetFirstPlayerController()->SetPause(true);
			GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;

			UE_LOG(LogTemp, Warning, TEXT("Menu Pause"));

		}else
		{
			PauseMenu->RemoveFromParent();
			GetWorld()->GetFirstPlayerController()->SetPause(false);
			GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;


		
			UE_LOG(LogTemp, Warning, TEXT("Removiendo Pause Menu desde ManuPauseExec"));
		}

		UE_LOG(LogTemp, Warning, TEXT("Menu Pause Exect"));

	}


}


void AOnDirt2Character::ShowOptionsMenu()
{
	if (OptionsMenu && !OptionsMenu->IsInViewport())
	{
		OptionsMenu->AddToViewport();

		UE_LOG(LogTemp, Warning, TEXT("MostrandoOpciones"));
	}
}


void AOnDirt2Character::OpenOptionsMenu() {

	ShowOptionsMenu();

		if (PauseMenu && PauseMenu->IsInViewport())
		{
			PauseMenu->RemoveFromParent();
			GetWorld()->GetFirstPlayerController()->SetPause(false);
			GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;



			//UE_LOG(LogTemp, Warning, TEXT("Removiendo Puase Menu"));
		}
}

void AOnDirt2Character::ShowPauseMenu() {


	if (InventoryHUDWidget && InventoryHUDWidget->IsInViewport()) return;

	if (PauseMenu && !PauseMenu->IsInViewport())
	{
		PauseMenu->AddToViewport();
	}

}



FString AOnDirt2Character::GetPickUpItem() {


	return TextNote;

}


void AOnDirt2Character::ShowInventory()
{

	if (bIsDie) return;

	if (PauseMenu && PauseMenu->IsInViewport()) return;




	if (bAllowInventary) {

		if (!InventoryHUDWidgetClass) return;

		

		if (InventoryHUDWidget && InventoryHUDWidget->IsInViewport()) {

			InventoryHUDWidget->RemoveFromParent();
			
			GetWorld()->GetFirstPlayerController()->SetPause(false);
			GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
			if (DetailsItemWidget && DetailsItemWidget->IsInViewport()) {

				DetailsItemWidget->RemoveFromParent();
				//UE_LOG(LogTemp, Error, TEXT("DetailsWidget Quedo en el viewport"));

			}
			if (ItemsOptionsWidget && ItemsOptionsWidget->IsInViewport()) {

				ItemsOptionsWidget->RemoveFromParent();

			}

			UE_LOG(LogTemp, Warning, TEXT("Quitando Inventario"));

		}
		else {



			UE_LOG(LogTemp, Warning, TEXT("Poniendo Inventario"));
			GetWorld()->GetFirstPlayerController()->SetPause(true);
			GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;

			// Crear el widget si no existe

			if (!InventoryHUDWidget) {

				InventoryHUDWidget= CreateWidget<UInventoryHUDWidget>(GetWorld(), InventoryHUDWidgetClass);

				/*if (!DetailsItemWidget) {

					DetailsItemWidget = CreateWidget<UDetailsItemWidget>(GetWorld(), DetailsWidgetClass);
					
				}*/
			}
			if (InventoryHUDWidget)
			{
					
					InventoryHUDWidget->AddToViewport();

					/*if (DetailsItemWidget) {

						DetailsItemWidget->AddToViewport();

					}*/
			}

			InventoryComponent->UpdateItemsLanguage();

			// Obtener los nombres del inventario desde el componente
			TArray<FInventoryItemData> Items = InventoryComponent->GetInventoryItems();
			// Llenar el widget con los nombres
			InventoryHUDWidget->PopulateInventory(Items);


		}


	}
}

void  AOnDirt2Character::SetUseItem() {



	if (ItemName == "Pistol" || ItemName == "Pistola") {

		if (bEquipedMeleeGun) {


			bEquipedMeleeGun = false;

			HeldMeleeGunItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			HeldMeleeGunItem->SetActorEnableCollision(true); // Reactivar la colisi�n si es necesario
			HeldMeleeGunItem->SetActorHiddenInGame(true);   // Ocultar el arma en el mundo
			HeldMeleeGunItem->SetActorTickEnabled(true);


			// A�adir al root para evitar eliminaci�n
			HeldMeleeGunItem->AddToRoot();

		}

		if (bEquipedGun) {

			if (HeldPistol) {

				//El arma ya esta equipada
				UE_LOG(LogTemp, Error, TEXT("El arma ya esta equipada"));
				bEquipedGun = false;
				HeldPistol->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				HeldPistol->SetActorEnableCollision(true); // Reactivar la colisi�n si es necesario
				HeldPistol->SetActorHiddenInGame(true);   // Ocultar el arma en el mundo
				HeldPistol->SetActorTickEnabled(true);


				// A�adir al root para evitar eliminaci�n
				HeldPistol->AddToRoot();
					

				
			}
			else {

				UE_LOG(LogTemp, Error, TEXT("El arma no existe en memoria"));
			}

		}
		else {
			bEquipedGun = true;

			if (HeldPistol) {

				

				HeldPistol->SetActorHiddenInGame(false); // Hacer visible el arma
				HeldPistol->SetActorTickEnabled(true);  // Reactivar el tick si es necesario
				HeldPistol->Mesh->SetSimulatePhysics(false);
				HeldPistol->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("HandSocket"));
				HeldPistol->SetActorEnableCollision(false);

		

					HeldPistol->RemoveFromRoot();

				



			}
			else {
				UE_LOG(LogTemp, Error, TEXT("El arma no existe en memoria"));

			}



			

		}


		if (bEquipedGun) {


			//ItemsOptionsWidget->UpdateText(false);
			UE_LOG(LogTemp, Error, TEXT("UnEquip"), *ItemName);


			if (Language) {

				ItemsOptionsWidget->UseText->SetText(FText::FromString("Desequipar"));
			}
			else {


				ItemsOptionsWidget->UseText->SetText(FText::FromString("UnEquip"));

			}
			
		}
		else {

			if (Language) {

				ItemsOptionsWidget->UseText->SetText(FText::FromString("Equipar"));

			}
			else {


				ItemsOptionsWidget->UseText->SetText(FText::FromString("Equip"));

			}

			UE_LOG(LogTemp, Error, TEXT("Equip"), *ItemName);

		}




	}

	if (ItemName == "Pistol ammo" || ItemName == "Municion de pistola") {

		UE_LOG(LogTemp, Error, TEXT("No se puede usar este item"));
		return;

	}

	if (ItemName == "Bat") {

		if (bEquipedGun) {

			bEquipedGun = false;

			HeldPistol->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			HeldPistol->SetActorEnableCollision(false); // Reactivar la colisi�n si es necesario
			HeldPistol->SetActorHiddenInGame(true);   // Ocultar el arma en el mundo
			HeldPistol->SetActorTickEnabled(true);


			// A�adir al root para evitar eliminaci�n
			HeldPistol->AddToRoot();

		}

		if (bEquipedMeleeGun) {

			
			if (HeldMeleeGunItem) {

				//El arma ya esta equipada
				UE_LOG(LogTemp, Error, TEXT("El arma ya esta equipada"));
				bEquipedMeleeGun = false;
				HeldMeleeGunItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				HeldMeleeGunItem->SetActorEnableCollision(true); // Reactivar la colisi�n si es necesario
				HeldMeleeGunItem->SetActorHiddenInGame(true);   // Ocultar el arma en el mundo
				HeldMeleeGunItem->SetActorTickEnabled(true);


				// A�adir al root para evitar eliminaci�n
				HeldMeleeGunItem->AddToRoot();



			}
			else {

				UE_LOG(LogTemp, Error, TEXT("El arma no existe en memoria"));
			}

		}
		else {
			

			bEquipedMeleeGun = true;

			if (HeldMeleeGunItem) {



				HeldMeleeGunItem->SetActorHiddenInGame(false); // Hacer visible el arma
				HeldMeleeGunItem->SetActorTickEnabled(true);  // Reactivar el tick si es necesario
				HeldMeleeGunItem->Mesh->SetSimulatePhysics(false);
				HeldMeleeGunItem->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("MeleeSocket"));
				HeldMeleeGunItem->SetActorEnableCollision(false);

				HeldMeleeGunItem->RemoveFromRoot();





			}
			else {
				UE_LOG(LogTemp, Error, TEXT("El arma no existe en memoria"));

			}

		}

		if (bEquipedMeleeGun) {


			//ItemsOptionsWidget->UpdateText(false);
			UE_LOG(LogTemp, Error, TEXT("UnEquip"), *ItemName);

			if (Language) {


				ItemsOptionsWidget->UseText->SetText(FText::FromString("Desequipar"));
			}
			else {


				ItemsOptionsWidget->UseText->SetText(FText::FromString("UnEquip"));

			}

		}
		else {

			if (Language) {

				ItemsOptionsWidget->UseText->SetText(FText::FromString("Equipar"));
			}
			else {

				ItemsOptionsWidget->UseText->SetText(FText::FromString("Equip"));

			}

			UE_LOG(LogTemp, Error, TEXT("Equip"), *ItemName);

		}


		


	}


	//ItemsUso

	if (ItemName == "Healthy Drink" || ItemName == "Bebida saludable") {


		for (int32 i = 0; i < InventoryComponent->Inventory.Num(); i++)
		{
			if (InventoryComponent->Inventory[i].ItemName == ItemName)
			{
				InventoryComponent->Inventory.RemoveAt(i); // Eliminar el �tem del array
				UE_LOG(LogTemp, Warning, TEXT("Item Usado"));
			}

		}

		if (InventoryHUDWidget)
		{
			InventoryHUDWidget->PopulateInventory(InventoryComponent->Inventory);
		}

		if (DetailsItemWidget && DetailsItemWidget->IsInViewport()) {

			DetailsItemWidget->RemoveFromParent();
			//UE_LOG(LogTemp, Error, TEXT("DetailsWidget Quedo en el viewport"));

		}
		if (ItemsOptionsWidget && ItemsOptionsWidget->IsInViewport()) {

			ItemsOptionsWidget->RemoveFromParent();

		}

		if (Life <= 100.f) {

			Life = Life + 20.f;

		}
		else {

			//Notification you health is full!

		}

	}
	else if (ItemName == "Key" || ItemName == "Llave") {


		UE_LOG(LogTemp, Warning, TEXT("No puedes usar la llave aqui"));


		if (InventoryHUDWidget)
		{
			InventoryHUDWidget->PopulateInventory(InventoryComponent->Inventory);
		}

		if (DetailsItemWidget && DetailsItemWidget->IsInViewport()) {

			DetailsItemWidget->RemoveFromParent();
			//UE_LOG(LogTemp, Error, TEXT("DetailsWidget Quedo en el viewport"));

		}
		if (ItemsOptionsWidget && ItemsOptionsWidget->IsInViewport()) {

			ItemsOptionsWidget->RemoveFromParent();

		}

	}



}

void  AOnDirt2Character::SetDropItem() {



	if (ItemName == "Pistol" || ItemName == "Pistola") {

		if (!NotificationWidget){

			NotificationWidget = CreateWidget<UNotificationWidget>(GetWorld(), NotificationWidgetClass);

			UE_LOG(LogTemp, Error, TEXT("Se Crea Noti"));


		}

		if (NotificationWidget && !NotificationWidget->IsInViewport()) {



			if (Language) {

				NotificationWidget->DescriptionNotification->SetText(FText::FromString("No puedes tirar este item"));
			
			}
			else {

				NotificationWidget->DescriptionNotification->SetText(FText::FromString("Can't Drop this Item"));

			}

			NotificationWidget->AddToViewport();	
			UE_LOG(LogTemp, Error, TEXT("Se a�ade noti"));
			GetWorld()->GetTimerManager().SetTimer(NotiTimerHandle, this, &AOnDirt2Character::RemoveWidgets, 1.5f, false);


		}

		

	}

	if (ItemName == "Bat" || ItemName == "Bate") {

		//Tirar Melee weapon

		for (int32 i = 0; i < InventoryComponent->Inventory.Num(); i++)
		{
			if (InventoryComponent->Inventory[i].ItemName == ItemName)
			{
				InventoryComponent->Inventory.RemoveAt(i); // Eliminar el �tem del array
				HeldMeleeGunItem->RemoveFromRoot();
				HeldMeleeGunItem->Destroy();
				HeldMeleeGunItem = nullptr;
				UE_LOG(LogTemp, Warning, TEXT("Item Eliminado"));
			}

		}

		
		

		if (InventoryHUDWidget)
		{
			InventoryHUDWidget->PopulateInventory(InventoryComponent->Inventory);
		}

		if (DetailsItemWidget && DetailsItemWidget->IsInViewport()) {

			DetailsItemWidget->RemoveFromParent();
			//UE_LOG(LogTemp, Error, TEXT("DetailsWidget Quedo en el viewport"));

		}
		if (ItemsOptionsWidget && ItemsOptionsWidget->IsInViewport()) {

			ItemsOptionsWidget->RemoveFromParent();

		}

	}

	if (ItemName == "Pistol ammo" || ItemName == "Municion de pistola") {



		for (int32 i = 0; i < InventoryComponent->Inventory.Num(); i++)
		{
			if (InventoryComponent->Inventory[i].ItemName == ItemName)
			{
				InventoryComponent->Inventory.RemoveAt(i); // Eliminar el �tem del array
				HeldPistolAmmo->RemoveFromRoot();
				HeldPistolAmmo->Destroy();
				HeldPistolAmmo = nullptr;
				UE_LOG(LogTemp, Warning, TEXT("Item Eliminado"));
			}

		}




		if (InventoryHUDWidget)
		{
			InventoryHUDWidget->PopulateInventory(InventoryComponent->Inventory);
		}

		if (DetailsItemWidget && DetailsItemWidget->IsInViewport()) {

			DetailsItemWidget->RemoveFromParent();
			//UE_LOG(LogTemp, Error, TEXT("DetailsWidget Quedo en el viewport"));

		}
		if (ItemsOptionsWidget && ItemsOptionsWidget->IsInViewport()) {

			ItemsOptionsWidget->RemoveFromParent();

		}

		

	}


	if (ItemName == "Healthy Drink" || ItemName == "Bebida saludable") {


		for (int32 i = 0; i < InventoryComponent->Inventory.Num(); i++)
		{
			if (InventoryComponent->Inventory[i].ItemName == ItemName)
			{
				InventoryComponent->Inventory.RemoveAt(i); // Eliminar el �tem del array
				UE_LOG(LogTemp, Warning, TEXT("Item Eliminado"));
			}

		}

		if (InventoryHUDWidget)
		{
			InventoryHUDWidget->PopulateInventory(InventoryComponent->Inventory);
		}

		if (DetailsItemWidget && DetailsItemWidget->IsInViewport()) {

			DetailsItemWidget->RemoveFromParent();
			//UE_LOG(LogTemp, Error, TEXT("DetailsWidget Quedo en el viewport"));

		}
		if (ItemsOptionsWidget && ItemsOptionsWidget->IsInViewport()) {

			ItemsOptionsWidget->RemoveFromParent();

		}

		return;

	}
	else if (ItemName == "Key" || ItemName == "Llave") {


		UE_LOG(LogTemp, Error, TEXT("No puedes eliminar este item"));


		if (!NotificationWidget) {

			NotificationWidget = CreateWidget<UNotificationWidget>(GetWorld(), NotificationWidgetClass);

			UE_LOG(LogTemp, Error, TEXT("Se Crea Noti"));


		}

		if (NotificationWidget && !NotificationWidget->IsInViewport()) {

			if (Language) {

				NotificationWidget->DescriptionNotification->SetText(FText::FromString("No puedes tirar este item"));
			}
			else {

				NotificationWidget->DescriptionNotification->SetText(FText::FromString("Can't Drop this Item"));

			}

			NotificationWidget->AddToViewport();
			UE_LOG(LogTemp, Error, TEXT("Se a�ade noti"));
			GetWorld()->GetTimerManager().SetTimer(NotiTimerHandle, this, &AOnDirt2Character::RemoveWidgets, 1.5f, false);


		}


		/*if (InventoryHUDWidget)
		{
			InventoryHUDWidget->PopulateInventory(InventoryComponent->Inventory);
		}

		if (DetailsItemWidget && DetailsItemWidget->IsInViewport()) {

			DetailsItemWidget->RemoveFromParent();
			//UE_LOG(LogTemp, Error, TEXT("DetailsWidget Quedo en el viewport"));

		}
		if (ItemsOptionsWidget && ItemsOptionsWidget->IsInViewport()) {

			ItemsOptionsWidget->RemoveFromParent();

		}*/
		return;
	}



}

void AOnDirt2Character::RemoveWidgets() {

	if (NotificationWidget && NotificationWidget->IsInViewport()) {


		NotificationWidget->RemoveFromParent();
		NotificationWidget = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("Widget Noti eliminado del viewport."));

	}

	if (EnvironmentWidget && EnvironmentWidget->IsInViewport()) {

		EnvironmentWidget->RemoveFromParent();
		EnvironmentWidget = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("WidgetEnviroment eliminado del viewport."));

	}

}


void AOnDirt2Character::SetText(const FString& Name, const FString& Desc) {


	ItemName = Name;




	if (!ItemsOptionsWidget)
	{
		ItemsOptionsWidget = CreateWidget<UItemsOptionsWidget>(GetWorld(), ItemsOptionsWidgetClass);

	}

	if (!DetailsItemWidget)
	{

		DetailsItemWidget = CreateWidget<UDetailsItemWidget>(GetWorld(), DetailsWidgetClass);


	}



	if (ItemName == "Pistol" || ItemName == "Pistola") {


		if (bEquipedGun) {

			if (Language) {

				ItemsOptionsWidget->UseText->SetText(FText::FromString("Desequipar"));
			}
			else {

				UE_LOG(LogTemp, Error, TEXT("UnEquip"), *ItemName);
				ItemsOptionsWidget->UseText->SetText(FText::FromString("UnEquip"));

			}
			
		}
		else {

			if (Language) {

				ItemsOptionsWidget->UseText->SetText(FText::FromString("Equipar"));

			}
			else {

				UE_LOG(LogTemp, Error, TEXT("Equip"), *ItemName);
				ItemsOptionsWidget->UseText->SetText(FText::FromString("Equip"));


			}
			
			
		}


	}

	if (ItemName == "Bat" || ItemName == "Bate") {


		if (bEquipedMeleeGun) {

			if (Language) {

				ItemsOptionsWidget->UseText->SetText(FText::FromString("Desequipar"));
			}
			else {

				ItemsOptionsWidget->UseText->SetText(FText::FromString("UnEquip"));

			}


		}
		else {

			if (Language) {

				ItemsOptionsWidget->UseText->SetText(FText::FromString("Equipar"));
			}
			else {

				ItemsOptionsWidget->UseText->SetText(FText::FromString("Equip"));

			}


		}


	}

	if (ItemName == "Pistol ammo" || ItemName == "Municion de pistola") {


		if (Language) {

			ItemsOptionsWidget->UseText->SetText(FText::FromString("Usar"));

		}
		else {


			ItemsOptionsWidget->UseText->SetText(FText::FromString("Use"));

		}
		

	}


	if (ItemName == "Healthy Drink" || ItemName == "Bebida saludable") {


		if (Language) {

			ItemsOptionsWidget->UseText->SetText(FText::FromString("Usar"));

		}
		else {


			ItemsOptionsWidget->UseText->SetText(FText::FromString("Use"));

		}


	}

	if (DetailsItemWidget->IsInViewport()) {

		DetailsItemWidget->RemoveFromParent();



	}
	if (ItemsOptionsWidget->IsInViewport())
	{
		ItemsOptionsWidget->RemoveFromParent();


	}
		







	DetailsItemWidget->ItemNameText->SetText(FText::FromString(Name));
	DetailsItemWidget->ItemDescriptionText->SetText(FText::FromString(Desc));
	DetailsItemWidget->AddToViewport();
	ItemsOptionsWidget->AddToViewport();





}

void AOnDirt2Character::SelectLanguage(bool Value) {

	Language = Value;

}

//Sound Sistem
/*void AOnDirt2Character::PlayFootstepSound()
{
	if (!HasAuthority()) return;

	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0, 0, 50); // L�nea de rastreo corta hacia abajo

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams))
	{
		UPhysicalMaterial* PhysMaterial = HitResult.PhysMaterial.Get();
		if (PhysMaterial)
		{
			USoundCue* FootstepSound = GetFootstepSoundForMaterial(PhysMaterial);
			if (FootstepSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FootstepSound, GetActorLocation());
			}
		}
	}
}*/

void AOnDirt2Character::PlayFootstepSound()
{

	/*if (!HasAuthority()) return;

	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0, 0, 200); // Aumentamos la distancia

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	// Dibujar l�nea para depuraci�n
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f, 0, 2.0f);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_WorldStatic, QueryParams))
	{
		UPhysicalMaterial* PhysMaterial = HitResult.PhysMaterial.Get();
		USoundCue* FootstepSound = DefaultFootstepSound; // Por defecto, usar sonido gen�rico

		if (PhysMaterial)
		{
			FootstepSound = GetFootstepSoundForMaterial(PhysMaterial);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No se detect� material f�sico en el LineTrace, usando sonido por defecto."));
		}

		// Si hay sonido, reproducirlo
		if (FootstepSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FootstepSound, GetActorLocation());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("El LineTrace no detect� ning�n impacto"));
	}*/



	FVector Start = GetActorLocation(); // Obt�n la ubicaci�n del personaje
	FVector End = Start - FVector(0, 0, 500); // Rayo hacia abajo para detectar la superficie

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;

	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f, 0, 1.0f);


	// Realiza un rayo hacia abajo para detectar la superficie
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
	{
		if (HitResult.PhysMaterial.IsValid())  // Verificamos si hay un material f�sico v�lido
		{
			UPhysicalMaterial* SurfaceMaterial = HitResult.PhysMaterial.Get(); // Desreferenciar el TWeakObjectPtr

			if (SurfaceMaterial)
			{
				// Imprime el nombre del material f�sico para depuraci�n
				FString MaterialName = SurfaceMaterial->GetFName().ToString();
				UE_LOG(LogTemp, Warning, TEXT("Material detectado: %s"), *MaterialName);

				// Comparar el nombre del material f�sico con los tipos de superficies
				if (MaterialName.Contains(TEXT("Dirt")))
				{
					UGameplayStatics::PlaySoundAtLocation(this, FootstepDirtSound, GetActorLocation());
					UE_LOG(LogTemp, Warning, TEXT("Piso Dirt"));
				}
				else if (MaterialName.Contains(TEXT("Grass")))
				{
					UGameplayStatics::PlaySoundAtLocation(this, FootstepGrassSound, GetActorLocation());
					UE_LOG(LogTemp, Warning, TEXT("Piso Grass"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Material no reconocido: %s"), *MaterialName);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("El material f�sico est� presente pero es nulo"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No se obtiene material f�sico en HitResult"));
		}

	}

}




USoundCue* AOnDirt2Character::GetFootstepSoundForMaterial(UPhysicalMaterial* PhysMaterial)
{
	//if (!PhysMaterial) return nullptr;

	
	// Si no hay material f�sico, usar sonido por defecto
	/*if (!PhysMaterial)
	{
		return DefaultFootstepSound;
	}

	// Verificar el tipo de material f�sico
	if (PhysMaterial == PM_Dirt)
	{
		return FootstepDirtSound;
	}

	if (PhysMaterial == PM_Grass)
	{
		return FootstepGrassSound;
	}

	// Si el material no coincide con ninguno, retornar sonido por defecto
	return DefaultFootstepSound;*/



	if (!PhysMaterial)
	{
		return DefaultFootstepSound;
	}

	// Obtener el tipo de superficie
	EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(PhysMaterial);

	switch (SurfaceType)
	{
	case SurfaceType1:
		return FootstepDirtSound;
	case SurfaceType2:
		return FootstepGrassSound;
	default:
		return DefaultFootstepSound;
	}


}







void AOnDirt2Character::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	// Aseg�rate de limpiar el objeto referenciado
	if (HeldPistol) {
		HeldPistol->RemoveFromRoot();
		HeldPistol = nullptr; // Limpia la referencia
	}

	if (HeldMeleeGunItem) {


		HeldMeleeGunItem->RemoveFromRoot();
		HeldMeleeGunItem = nullptr;
	}

	if (HeldPistolAmmo) {

		HeldPistolAmmo->RemoveFromRoot();
		HeldPistolAmmo = nullptr;
	}




	// Nombre del slot que deseas borrar
	FString SaveSlotName = TEXT("CheckPointSaveSlot");

	// Eliminar el slot de guardado
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		UGameplayStatics::DeleteGameInSlot(SaveSlotName, 0);
		UE_LOG(LogTemp, Warning, TEXT("Checkpoint eliminado del slot: %s"), *SaveSlotName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No se encontr� el checkpoint para eliminar."));
	}



	UE_LOG(LogTemp, Warning, TEXT("EndPlay llamado. Se ha liberado HeldPistol."));



}











