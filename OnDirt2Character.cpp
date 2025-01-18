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


	//LookUpDown
	bIsAimingUp = false;
	bIsAimingDown = false;

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
		PauseMenu = CreateWidget<UUserWidget>(GetWorld(), PauseMenuClass);
	}
	if (OptionsMenuClass) {

		OptionsMenu = CreateWidget<UUserWidget>(GetWorld(), OptionsMenuClass);
	}
	if (InventoryHUDWidget) {

		InventoryHUDWidget = CreateWidget<UInventoryHUDWidget>(GetWorld(), InventoryHUDWidgetClass);

	}


	GetCharacterMovement()->MaxAcceleration = 600.f; // Ajusta según la suavidad deseada

	// Ajustar la desaceleración al caminar
	GetCharacterMovement()->BrakingDecelerationWalking = 100.f;

	GetWorldTimerManager().SetTimer(
		ThrowTimerHandle,  // Identificador del temporizador
		this,                     // Objeto al que pertenece la función
		&AOnDirt2Character::EnablePause, // Función que será llamada
		2.0f,                     // Tiempo de espera
		false                     // No se repite
	);


}
	





void AOnDirt2Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Detectar si el personaje está quieto
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
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AOnDirt2Character::Sprinting);

		//Crouch
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AOnDirt2Character::Crouching);

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

		//Menu Pause
		EnhancedInputComponent->BindAction(MenuPause, ETriggerEvent::Triggered, this, &AOnDirt2Character::MenuPauseExec);

		//Inventory
		EnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Triggered, this, &AOnDirt2Character::ShowInventory);




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
	//FRotator YawRotation;

	/*if (Controller != nullptr) {

		if(!bAimingPistol){


			// Verifica si se está utilizando una cámara fija
			if (bUsingFixedCamera && FixedCameraActor)
			{
				// Obtiene la rotación de la cámara fija
				YawRotation = FixedCameraActor->GetActorRotation();
			}
			else
			{
				// Obtiene la rotación del controlador (cámara libre)
				const FRotator Rotation = Controller->GetControlRotation();
				YawRotation = FRotator(0, Rotation.Yaw, 0); // Solo la rotación en el eje Yaw
			}

			// Obtiene el vector hacia adelante basado en la rotación
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

			// Obtiene el vector hacia la derecha basado en la rotación
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			// Aquí se usan los inputs de movimiento de forma independiente para adelante/atrás y izquierda/derecha
			// Asegúrate de que MovementVector.Y se use para el movimiento hacia adelante/atrás
			// y MovementVector.X para el movimiento hacia los lados (izquierda/derecha)

			// Para controles tipo tanque, el movimiento en la dirección Y (hacia adelante) sigue siendo relativo a la cámara,
			// pero el movimiento en la dirección X (hacia los lados) es fijo en el espacio global.

			AddMovementInput(ForwardDirection, MovementVector.Y);  // Movimiento hacia adelante/atrás
			AddMovementInput(RightDirection, MovementVector.X);    // Movimiento hacia los lados (izquierda/derecha)



		}

	}*/


	if (Controller != nullptr)
	{
		if (!bAimingPistol)
		{

			FRotator YawRotation;
			// Movimiento tipo tanque (no está apuntando)
			// Verifica si se está utilizando una cámara fija
			if (bUsingFixedCamera && FixedCameraActor)
			{
				// Obtiene la rotación de la cámara fija
				YawRotation = FixedCameraActor->GetActorRotation();
			}
			else
			{
				// Obtiene la rotación del controlador (cámara libre)
				const FRotator Rotation = Controller->GetControlRotation();
				YawRotation = FRotator(0, Rotation.Yaw, 0); // Solo la rotación en el eje Yaw
			}

			// Obtiene el vector hacia adelante basado en la rotación
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

			// Obtiene el vector hacia la derecha basado en la rotación
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			// Aquí se usan los inputs de movimiento de forma independiente para adelante/atrás y izquierda/derecha
			// Asegúrate de que MovementVector.Y se use para el movimiento hacia adelante/atrás
			// y MovementVector.X para el movimiento hacia los lados (izquierda/derecha)

			// Para controles tipo tanque, el movimiento en la dirección Y (hacia adelante) sigue siendo relativo a la cámara,
			// pero el movimiento en la dirección X (hacia los lados) es fijo en el espacio global.

			AddMovementInput(ForwardDirection, MovementVector.Y);  // Movimiento hacia adelante/atrás
			AddMovementInput(RightDirection, MovementVector.X);    // Movimiento hacia los lados (izquierda/derecha)
		}
		else // Si el jugador está apuntando
		{


			// Solo permite rotación sobre su propio eje, sin movimiento hacia adelante/atrás
			FRotator YawRotation;

			// Si se está utilizando una cámara fija, obtenemos la rotación de la cámara
			if (bUsingFixedCamera && FixedCameraActor)
			{
				YawRotation = FixedCameraActor->GetActorRotation();
			}
			else
			{
				// Si no, obtenemos la rotación del controlador (la rotación de la cámara)
				const FRotator Rotation = Controller->GetControlRotation();
				YawRotation = FRotator(0, Rotation.Yaw, 0);  // Solo Yaw para mantener la rotación en el plano horizontal
			}

			// Aquí, no calculamos la dirección desde la cámara, solo la dirección derecha basada en las teclas A/D
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			// Solo permite el movimiento lateral (izquierda/derecha) mientras apunta
			AddMovementInput(RightDirection, MovementVector.X);  // Movimiento lateral (izquierda/derecha)

			// Rotación del personaje mientras apunta (con teclas A/D)
			if (MovementVector.X != 0.f)  // Si el jugador se mueve hacia la izquierda (A) o hacia la derecha (D)
			{
				FRotator CurrentRotation = GetActorRotation();  // Obtener la rotación actual del actor

				// Rotar directamente sobre el eje Yaw (vertical) para rotar sobre el eje propio
				float RotationAmount = (MovementVector.X > 0.f) ? 2.f : -2.f;  // Gira 2 grados por cada movimiento, en lugar de 5

				FRotator TargetRotation = FRotator(0.f, CurrentRotation.Yaw + RotationAmount, 0.f); // Gira 2 grados por cada movimiento

				// Suavizar la transición de rotación (esto hace que el giro no sea instantáneo)
				FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), 50.f);  // Reduce el valor de 5.f a 2.f para que gire más lentamente

				// Establecer la nueva rotación
				SetActorRotation(NewRotation);
			}



		}
	}

}



	







void AOnDirt2Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

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



	if (bEquipedGun && bAimingPistol) {

		if (bShootingPistol) return;

		PerformGunTrace();
		UE_LOG(LogTemp, Warning, TEXT("Dispara dispara"));
		GetWorld()->GetTimerManager().SetTimer(ThrowTimerHandle, this, &AOnDirt2Character::ResetShootingFlag, 0.6f, false);
	}

	if (bEquipedMeleeGun && bAimingMeleeGun) {




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


void AOnDirt2Character::ExecuteThrow() {


	//Direccion del personaje
// Obtener la dirección en la que el personaje está mirando
	HeldObject->bIsRealising = false;
	FVector CharacterForward = GetActorForwardVector();
	FVector CharacterUp = GetActorUpVector();
	FVector ImpulseDirection = CharacterForward + (CharacterUp * 0.5f); // Ajusta la dirección si es necesario

	//HeldObject->bIsRealising = false;
	// Desanclar el objeto de la mano
	HeldObject->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// Hacer visible el objeto nuevamente
	//HeldObject->SetActorHiddenInGame(false);

	// Habilitar física y colisiones nuevamente
	HeldObject->Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HeldObject->SetActorEnableCollision(true);
	HeldObject->Mesh->SetSimulatePhysics(true);

	// Aplicar una fuerza para lanzar el objeto
	if (HeldObject->Mesh)
	{
		// Aplicar una fuerza en la dirección calculada
		HeldObject->Mesh->AddImpulse(ImpulseDirection * 1000, NAME_None, true);
	}

	// Limpiar la referencia al objeto sostenido
	HeldObject = nullptr;
	OverlappingThrowOBJ = nullptr;
	bIsAiming = false;
	






}

void  AOnDirt2Character::Aiming() {

	if (bEquipedGun && !bIsCrouched) {

		bAimingPistol = true;
		UE_LOG(LogTemp, Warning, TEXT("Apunta."));

		UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
		if (MovementComponent)
		{
			MovementComponent->DisableMovement(); // Desactiva el movimiento
		}

	}

	if (bEquipedMeleeGun) {


	}

	
	if (HeldObject) {


		UCharacterMovementComponent* Movement = GetCharacterMovement();
		Movement->DisableMovement();
		//UE_LOG(LogTemp, Warning, TEXT("Apuntando"));

		// Direccion del personaje
		FVector CharacterForward = GetActorForwardVector();
		FVector CharacterUp = GetActorUpVector();
		FVector ImpulseDirection = CharacterForward + (CharacterUp * 0.5f); // Ajusta la dirección si es necesario

		// Parámetros de simulación
		const float SimulationTimeStep = 0.005f; // Tiempo entre cada punto de la simulación
		const int32 MaxSimulationSteps = 200; // Número máximo de pasos de simulación
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

			// Dibujar la línea de trayectoria
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

	UE_LOG(LogTemp, Warning, TEXT("Deja de Apuntar Arriba"));
	bIsAimingUp = false;

}
void AOnDirt2Character::CancelLookDown() {

	UE_LOG(LogTemp, Warning, TEXT("Deja de Apuntar Abajo"));
	bIsAimingDown = false;

}

void AOnDirt2Character::StopAiming()
{
	bIsAiming = false;
	bAimingPistol = false;
	bIsAimingDown = false;
	bIsAimingUp = false;
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

	bShootingPistol = true;
	//UCharacterMovementComponent* Movement = GetCharacterMovement();
	//Movement->DisableMovement();

	// Obtener la posición y rotación del HandGunSocket
	FVector Start = GetMesh()->GetSocketLocation(TEXT("HandGunSocket"));
	FRotator SocketRotation = GetMesh()->GetSocketRotation(TEXT("HandGunSocket"));
	FVector ForwardVector = SocketRotation.Vector(); // Dirección hacia donde apunta el socket

	// Definir la distancia del raycast
	float TraceDistance = 300.0f;
	FVector End = Start + (ForwardVector * TraceDistance);

	// Configurar los parámetros del trazado
	FHitResult HitResult;
	FCollisionQueryParams TraceParams;
	TraceParams.bTraceComplex = true;
	TraceParams.AddIgnoredActor(this); // Ignorar al personaje

	// Ejecutar el Line Trace
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility, // Ajusta el canal de colisión según tus necesidades
		TraceParams
	);

	// Dibujar líneas de depuración para visualizar el trazado
	if (bHit)
	{
		DrawDebugLine(GetWorld(), Start, HitResult.Location, FColor::Red, false, 5.0f, 0, 1.0f);

		if (HitResult.GetActor())
		{
			// Verificar si el actor impactado es un AEnemyCharacter
			AEnemyCharacter* HitEnemy = Cast<AEnemyCharacter>(HitResult.GetActor());
			if (HitEnemy)
			{
				UE_LOG(LogTemp, Log, TEXT("¡Enemigo alcanzado! %s"), *HitEnemy->GetName());
				// Realiza cualquier acción adicional con el enemigo alcanzado
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Impacto con un objeto no válido: %s"), *HitResult.GetActor()->GetName());
			}
		}
	}
	else
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 5.0f, 0, 1.0f);
		UE_LOG(LogTemp, Log, TEXT("No se impactó con ningún objeto."));
	}

	// Verificar que el socket exista en el SkeletalMesh
	/*if (!GetMesh() || !GetMesh()->DoesSocketExist(TEXT("HandGunSocket")))
	{
		UE_LOG(LogTemp, Warning, TEXT("HandGunSocket no existe en el SkeletalMesh del personaje."));
		return;
	}

	// Activar estado de disparo
	bShootingPistol = true;

	// Obtener la posición y rotación del socket
	FVector SocketLocation = GetMesh()->GetSocketLocation(TEXT("HandGunSocket"));
	FRotator SocketRotation = GetMesh()->GetSocketRotation(TEXT("HandGunSocket"));
	FVector ForwardVector = SocketRotation.Vector(); // Dirección hacia adelante

	// Altura de desplazamiento para las líneas (medio metro en ambas direcciones)
	float VerticalOffset = 50.0f; // 50 unidades = 0.5 metros
	float TraceDistance = 300.0f; // Distancia hacia adelante

	// Array de alturas para los trazados (-0.5m, original, +0.5m)
	TArray<float> VerticalOffsets = { -VerticalOffset, 0.0f, VerticalOffset };

	for (float Offset : VerticalOffsets)
	{
		// Calcular inicio y fin del trazado
		FVector Start = SocketLocation + FVector(0, 0, Offset); // Mover hacia arriba/abajo
		FVector End = Start + (ForwardVector * TraceDistance);  // Extender hacia adelante

		// Configurar los parámetros del trazado
		FHitResult HitResult;
		FCollisionQueryParams TraceParams;
		TraceParams.bTraceComplex = true;
		TraceParams.AddIgnoredActor(this); // Ignorar al personaje

		// Ejecutar el Line Trace
		bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			Start,
			End,
			ECC_Visibility, // Ajusta el canal de colisión según tus necesidades
			TraceParams
		);

		// Dibujar líneas de depuración para visualizar los trazados
		if (bHit)
		{
			DrawDebugLine(GetWorld(), Start, HitResult.Location, FColor::Red, false, 5.0f, 0, 1.0f);

			if (HitResult.GetActor())
			{
				// Verificar si el actor impactado es un AEnemyCharacter
				AEnemyCharacter* HitEnemy = Cast<AEnemyCharacter>(HitResult.GetActor());
				if (HitEnemy)
				{
					UE_LOG(LogTemp, Log, TEXT("¡Enemigo alcanzado! %s"), *HitEnemy->GetName());
					// Realiza cualquier acción adicional con el enemigo alcanzado
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("Impacto con un objeto no válido: %s"), *HitResult.GetActor()->GetName());
				}
			}
		}
		else
		{
			DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 5.0f, 0, 1.0f);
			UE_LOG(LogTemp, Log, TEXT("No se impactó con ningún objeto en la línea vertical offset: %f"), Offset);
		}
	}*/







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
					UE_LOG(LogTemp, Log, TEXT("Objecto añadido"));
				}
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
			UE_LOG(LogTemp, Log, TEXT("Objecto añadido"));
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
			UE_LOG(LogTemp, Log, TEXT("Melee gun añadida"));
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
			UE_LOG(LogTemp, Log, TEXT("Pistola añadida"));
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
		UE_LOG(LogTemp, Warning, TEXT("No hay un objeto válido para agarrar"));
	}



	


}

void AOnDirt2Character::SetToRead(bool Value) {


	if (Value) {

		if(HeldNote){

			if (ReadNoteClass) {

				UUserWidget* WidgetReadNote = CreateWidget<UUserWidget>(GetWorld(), ReadNoteClass);
				ReadNoteWidget = Cast<UNotesToReadWidget>(WidgetReadNote);

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
			HeldHealth = nullptr;
			GetWorld()->GetFirstPlayerController()->SetPause(false);
			GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
		}


		if (HeldMeleeGunItem) {
			InventoryComponent->PickUpItem(HeldMeleeGunItem);
			//HeldMeleeGunItem = nullptr;
			GetWorld()->GetFirstPlayerController()->SetPause(false);
			GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;


		}

		if (HeldPistol) {

			InventoryComponent->PickUpItem(HeldPistol);
			//HeldPistol = nullptr;
			GetWorld()->GetFirstPlayerController()->SetPause(false);
			GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
		}

	}
	else {

		HeldNote = nullptr;
		HeldHealth = nullptr;
		HeldPistol = nullptr;
		HeldMeleeGunItem = nullptr;

	}

}


void AOnDirt2Character::DropOBJ() {

	if (HeldObject) // Asegúrate de que hay un objeto sostenido
	{
		// Soltar el objeto
		HeldObject->bIsRealising = true;
		HeldObject->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// Reactivar físicas y colisión
		HeldObject->Mesh->SetSimulatePhysics(true);
		HeldObject->SetActorEnableCollision(true);

		// Limpiar la referencia y estado
		HeldObject = nullptr;

		UE_LOG(LogTemp, Warning, TEXT("Objeto soltado"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No hay ningún objeto sostenido para soltar"));
	}



}

void AOnDirt2Character::GetDamage() {



	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController && PlayerController->PlayerCameraManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("Shaking making"));
		PlayerController->PlayerCameraManager->StartCameraShake(UCameraShakeComponent::StaticClass());
	}

	bIsHit = true;
	/*if (!Life == 0) {

		Life = Life - 50;
		UE_LOG(LogTemp, Warning, TEXT("Tienes vida"));
		bIsHit = true;

	}
	else {

		bIsDie = true;

		//bIsCallEnemy = true;

		for (TActorIterator<AEnemyCharacter> It(GetWorld()); It; ++It)
		{
			AEnemyCharacter* Enemy = *It;
			if (Enemy && Enemy->GetController())
			{
				UBlackboardComponent* BlackboardComp = Enemy->GetController()->FindComponentByClass<UBlackboardComponent>();

				if (BlackboardComp)
				{
					BlackboardComp->SetValueAsBool(TEXT("isDead"), true);
				}
			}
		}

		TArray<AActor*> PostProcessVolumes;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), PostProcessVolumes);

		for (AActor* Actor : PostProcessVolumes)
		{
			APostProcessVolume* PostProcessVolume = Cast<APostProcessVolume>(Actor);
			if (PostProcessVolume)
			{
				// Obtén el perfil de post-procesado actual
				FPostProcessSettings& PostProcessSettings = PostProcessVolume->Settings;

				// Modifica el efecto de ruido
				PostProcessSettings.FilmGrainIntensity = 2.f;
				UE_LOG(LogTemp, Warning, TEXT("CAMBIA FILTROOO"));

				// Aquí puedes ajustar la configuración del ruido según sea necesario
				// Por ejemplo:
				// PostProcessSettings.GrainIntensity = 0.5f;
			}
		}





		GetCharacterMovement()->DisableMovement();
		UE_LOG(LogTemp, Warning, TEXT("You're Dead"));


	}*/

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
}


void AOnDirt2Character::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (OtherActor->IsA(AThrowOBJ::StaticClass()))
	{
		OverlappingThrowOBJ = Cast<AThrowOBJ>(OtherActor);
		UE_LOG(LogTemp, Warning, TEXT("OVERLAP"));
	}

	if (OtherActor->IsA(ANotesToRead::StaticClass())) {

		OverlappingNote = Cast<ANotesToRead>(OtherActor);
		UE_LOG(LogTemp, Warning, TEXT("OVERLAP NOTE"));

	}

	if (OtherActor->IsA(AFixedCamera::StaticClass()))
	{
		FixedCameraActor = Cast<AFixedCamera>(OtherActor);
		bUsingFixedCamera = true;

		// Cambiar a la cámara fija
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		if (PlayerController)
		{
			PlayerController->SetViewTarget(FixedCameraActor);
		}
	}

	if (OtherActor->IsA(AHealthItem::StaticClass()))
	{
		
		OverlappingHealthItem = Cast<AHealthItem>(OtherActor);
		UE_LOG(LogTemp, Warning, TEXT("OVERLAP Health"));

	}

	if (OtherActor->IsA(AMeleeGunItem::StaticClass()))
	{

		OverlappingMeleeGunItem = Cast<AMeleeGunItem>(OtherActor);
		UE_LOG(LogTemp, Warning, TEXT("OVERLAP MeleeGun"));

	}

	if (OtherActor->IsA(APistolItem::StaticClass()))
	{

		OverlappingPistolItem = Cast<APistolItem>(OtherActor);
		UE_LOG(LogTemp, Warning, TEXT("OVERLAP FireGun"));

	}


}

void AOnDirt2Character::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if (HeldObject == nullptr)
	{
	
		OverlappingThrowOBJ = nullptr;
		
		UE_LOG(LogTemp, Warning, TEXT("FIN OVERLAP"));
	}

	if (HeldNote == nullptr) {

		OverlappingThrowOBJ = nullptr;

		UE_LOG(LogTemp, Warning, TEXT("FIN OVERLAP NOTE"));

	}

	if (HeldHealth == nullptr) {

		OverlappingHealthItem = nullptr;

		UE_LOG(LogTemp, Warning, TEXT("FIN OVERLAP NOTE"));

	}

	if (HeldMeleeGunItem == nullptr) {

		OverlappingHealthItem = nullptr;

		UE_LOG(LogTemp, Warning, TEXT("FIN OVERLAP MeleeGun"));

	}

	if (HeldPistol == nullptr) {

		OverlappingPistolItem = nullptr;

		UE_LOG(LogTemp, Warning, TEXT("FIN OVERLAP Gun"));
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
bool AOnDirt2Character::GetBatIdle() const {


	return bIsBatIdle;

}

void AOnDirt2Character::SetBating(bool Value){


	bIsBating= Value;

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
		

			// Obtener los nombres del inventario desde el componente
			TArray<FInventoryItemData> Items = InventoryComponent->GetInventoryItems();
			// Llenar el widget con los nombres
			InventoryHUDWidget->PopulateInventory(Items);


		}


	}
}

void  AOnDirt2Character::SetUseItem() {



	if (ItemName == "Gun") {

		if (bEquipedMeleeGun) {


			bEquipedMeleeGun = false;

		}

		if (bEquipedGun) {

			if (HeldPistol) {

				//El arma ya esta equipada
				UE_LOG(LogTemp, Error, TEXT("El arma ya esta equipada"));
				bEquipedGun = false;
				HeldPistol->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				HeldPistol->SetActorEnableCollision(true); // Reactivar la colisión si es necesario
				HeldPistol->SetActorHiddenInGame(true);   // Ocultar el arma en el mundo
				HeldPistol->SetActorTickEnabled(true);


				// Añadir al root para evitar eliminación
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
			
			ItemsOptionsWidget->EquipText->SetText(FText::FromString("UnEquip"));
		}
		else {


			UE_LOG(LogTemp, Error, TEXT("Equip"), *ItemName);
			ItemsOptionsWidget->EquipText->SetText(FText::FromString("Equip"));

		}




	}

	if (ItemName == "bat") {

		if (bEquipedGun) {

			bEquipedGun = false;

		}

		if (bEquipedMeleeGun) {


			//El arma ya esta equipada
			return;

		}
		else {
			
			bEquipedMeleeGun = true;

			HeldMeleeGunItem->Mesh->SetSimulatePhysics(false);
			HeldMeleeGunItem->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("HandSocket"));
			HeldMeleeGunItem->SetActorEnableCollision(false);

			//Logica Melee gun en mano

		}
		


	}


	//ItemsUso

	if (ItemName == "Healthy Drink") {


		for (int32 i = 0; i < InventoryComponent->Inventory.Num(); i++)
		{
			if (InventoryComponent->Inventory[i].ItemName == ItemName)
			{
				InventoryComponent->Inventory.RemoveAt(i); // Eliminar el ítem del array
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
	else if (ItemName == "Key") {


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



	if (ItemName == "Gun") {

		UE_LOG(LogTemp, Warning, TEXT("Cant drop FIREGUN"));
		return;

	}

	if (ItemName == "Bat") {

		//Tirar Melee weapon

		for (int32 i = 0; i < InventoryComponent->Inventory.Num(); i++)
		{
			if (InventoryComponent->Inventory[i].ItemName == ItemName)
			{
				InventoryComponent->Inventory.RemoveAt(i); // Eliminar el ítem del array
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


	if (ItemName == "Healthy Drink") {


		for (int32 i = 0; i < InventoryComponent->Inventory.Num(); i++)
		{
			if (InventoryComponent->Inventory[i].ItemName == ItemName)
			{
				InventoryComponent->Inventory.RemoveAt(i); // Eliminar el ítem del array
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
	else if (ItemName == "Key") {


		UE_LOG(LogTemp, Error, TEXT("No puedes eliminar este item"));


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



	if (ItemName == "Gun") {


		if (bEquipedGun) {


			//ItemsOptionsWidget->UpdateText(false);
			UE_LOG(LogTemp, Error, TEXT("UnEquip"), *ItemName);
			ItemsOptionsWidget->EquipText->SetText(FText::FromString("UnEquip"));
		}
		else {

			
			UE_LOG(LogTemp, Error, TEXT("Equip"), *ItemName);
			ItemsOptionsWidget->EquipText->SetText(FText::FromString("Equip"));
			
		}


	}

	if (ItemName == "Bat") {


		if (bEquipedMeleeGun) {


			ItemsOptionsWidget->EquipText->SetText(FText::FromString("UnEquip"));
		}
		else {

			ItemsOptionsWidget->EquipText->SetText(FText::FromString("Equip"));

		}


	}

	if (ItemName == "Healthy Drink") {

		ItemsOptionsWidget->EquipText->SetText(FText::FromString("Use"));

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

void AOnDirt2Character::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	// Asegúrate de limpiar el objeto referenciado
	if (HeldPistol) {
		HeldPistol->RemoveFromRoot();
		HeldPistol = nullptr; // Limpia la referencia
	}

	UE_LOG(LogTemp, Warning, TEXT("EndPlay llamado. Se ha liberado HeldPistol."));
}











