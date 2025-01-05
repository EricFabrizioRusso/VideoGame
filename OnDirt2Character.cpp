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
	//HeldHealth = nullptr;

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

	/*if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}*/

	if (Controller != nullptr) {

		FRotator YawRotation;

		//UE_LOG(LogTemp, Warning, TEXT("ExisteController"));
		if (bUsingFixedCamera && FixedCameraActor)
		{
			YawRotation = FixedCameraActor->GetActorRotation();
		}
		else
		{
			const FRotator Rotation = Controller->GetControlRotation();
			YawRotation = FRotator(0, Rotation.Yaw, 0);
		}

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);


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

	if (HeldObject && bIsAiming)
	{
		bIsThrowing = true;
		GetWorld()->GetTimerManager().SetTimer(ThrowTimerHandle, this, &AOnDirt2Character::ExecuteThrow, 0.4f, false);
		//ExecuteThrow();


	}




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

	if (!HeldObject) return;

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

void AOnDirt2Character::StopAiming()
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->SetMovementMode(MOVE_Walking);
	bIsAiming = false;
	UE_LOG(LogTemp, Log, TEXT("Aiming detenido"));
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


	if (OverlappingThrowOBJ && OverlappingThrowOBJ->bCanBeGrabbed)
	{

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

	}
	else {

		HeldNote = nullptr;
		HeldHealth = nullptr;

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
}

//Animations -----------------------------------------------

bool AOnDirt2Character::isAiming() const {


	return bIsAiming;

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


	if (ItemName == "Healthy Drink") {


		for (int32 i = 0; i < InventoryComponent->Inventory.Num(); i++)
		{
			if (InventoryComponent->Inventory[i].ItemName == ItemName)
			{
				InventoryComponent->Inventory.RemoveAt(i); // Eliminar el ítem del array
				UE_LOG(LogTemp, Warning, TEXT("Item Usado"));
			}

		}
	}
	else if (ItemName == "Key") {


		UE_LOG(LogTemp, Warning, TEXT("No puedes usar la llave aqui"));
		return;

	}



}

void  AOnDirt2Character::SetDropItem() {


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

		

	}
	else if (ItemName == "Key") {


		UE_LOG(LogTemp, Error, TEXT("No puedes eliminar este item"));
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
		//UE_LOG(LogTemp, Log, TEXT("Entraaaaaaaa"));

	}


	if (DetailsItemWidget->IsInViewport()) {

		DetailsItemWidget->RemoveFromParent();
	


	}
	if (ItemsOptionsWidget->IsInViewport())
	{
		DetailsItemWidget->RemoveFromParent();

	}

	DetailsItemWidget->ItemNameText->SetText(FText::FromString(Name));
	DetailsItemWidget->ItemDescriptionText->SetText(FText::FromString(Desc));
	DetailsItemWidget->AddToViewport();
	ItemsOptionsWidget->AddToViewport();





}











