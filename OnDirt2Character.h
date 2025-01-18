// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "OnDirt2Character.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UPickUpItemWidget;
class AInventoryItems;
class UInventoryComponent;
class UInventoryHUDWidget;
class UInventoryItemWidget;
class UDetailsItemWidget;
class UItemsOptionsWidget;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);


UCLASS(config=Game)
class AOnDirt2Character : public ACharacter
{

	


	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* GrabAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ThrowAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MenuPause;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InventoryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimingUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimingDownAction;

	UPROPERTY(EditDefaultsOnly, Category = "Grab")
	FName HandSocketName = "HandSocket";



public:
	AOnDirt2Character();
	


protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
		


protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY()
	class AThrowOBJ* OverlappingThrowOBJ; // Referencia al objeto lanzable
	class AThrowOBJ* HeldObject;

	//Notes Item
	class ANotesToRead* OverlappingNote; //Referencia Nota
	class ANotesToRead* HeldNote;

	//Health Item
	class AHealthItem* OverlappingHealthItem;
	class AHealthItem* HeldHealth;

	//Gun Pistol
	class APistolItem* OverlappingPistolItem;
	class APistolItem* HeldPistol;

	//MeleeWeapon
	class AMeleeGunItem* OverlappingMeleeGunItem;
	class AMeleeGunItem* HeldMeleeGunItem;

	// Función para agarrar el objeto
	UFUNCTION()
	void GrabThrowOBJ();
	void PickUpHandle();
	void DropOBJ();
	
	
	//CheckPoint

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkpoint")
	FVector LastCheckpointLocation;

	UFUNCTION()
	void SetLastCheckpointLocation(FVector NewLocation);

	UFUNCTION()
	void Respawn();


	//Damage
	UFUNCTION()
	void Die();

	void GetDamage();

	//Animations
	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool isAiming() const;


	//Throw Anim
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetIsThrowing(bool Value);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool GetIsThrowing() const;

	//PickUpAnim
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetIsPickUp(bool Value);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool GetIsPickUp() const;

	//Got Hit Character
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetIsHit(bool Value);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool GetIsHit() const;

	//Die Character
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetIsDie(bool Value);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool GetIsDie() const;


	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool GetAimingGun() const;


	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetAimingGunUp(bool Value);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool GetAimingGunUp() const;

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetAimingGunDown(bool Value);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool GetAimingGunDown() const;
	//Pistol Shoot

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetShootingPistol(bool Value);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool GetShootingPistol() const;

	//Pistol Crouch
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetShootingCrouch(bool Value);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool GetShootingCrouch() const;

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PerformGunTrace();

	//Bat



	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool GetBatIdle() const;

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetBating(bool Value);


	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool GetBating() const;




	//Widgets
	UFUNCTION(BlueprintCallable, Category = "Widget")
	FString GetPickUpItem();

	UFUNCTION(BlueprintCallable, Category = "Widget")
	void SetToRead(bool Value);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetText(const FString& Name, const FString& Desc);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetUseItem();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void  SetDropItem();





	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInventoryComponent* InventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> InventoryHUDWidgetClass;

	UPROPERTY()
	UInventoryHUDWidget* InventoryHUDWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FInventoryItemData ItemData;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> DetailsWidgetClass;

	UPROPERTY()
	UDetailsItemWidget* DetailsItemWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> ItemsOptionsWidgetClass;

	UPROPERTY()
	UItemsOptionsWidget* ItemsOptionsWidget;


	FString ItemName;
	FString ItemDesc;

private:
	void Sprinting();
	void Crouching();
	void Aiming();
	void StopAiming();
	void ThrowOBJ();
	void ExecuteThrow();
	void EnablePause();
	void ResetShootingFlag();


	//Looking
	void AimingUp();
	void AimingDown();
	void CancelLookUp();
	void CancelLookDown();
	


	//UI Functions
	void MenuPauseExec();
	void ShowOptionsMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowPauseMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowInventory();


	//Overlap
	void NotifyActorBeginOverlap(AActor* OtherActor);
	void NotifyActorEndOverlap(AActor* OtherActor);

	//Stats
	UPROPERTY(EditAnywhere)
	float Life;


	bool bCanCrouch;
	bool bIsHoldingObject;
	bool bIsAiming;
	bool bAimingPistol;
	bool bShootingPistol;
	bool bIsThrowing;
	bool bIsPickUp;
	bool bIsHit;
	bool bIsDie;
	bool bUsingFixedCamera;
	bool bAllowPause;
	bool bAllowInventary;
	bool bMeleeGun;
	bool bEquipedGun;
	bool bEquipedMeleeGun;
	bool bAimingMeleeGun;
	bool bIsShooting;
	bool bIsShootingCrouch;
	bool bIsBatIdle;
	bool bIsBating;



	//AimingBools
	bool bIsAimingUp;
	bool bIsAimingDown;

	FTimerHandle ThrowTimerHandle;
	FString TextNote;



	
	UPROPERTY()
	class AFixedCamera* FixedCameraActor;


	//UI
	UFUNCTION(BlueprintCallable, Category = "UI")
	void OpenOptionsMenu();

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> CheckPointWidget;


	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuClass;


	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> OptionsMenuClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> ReadNoteClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> PickUpItemClass;




	

	UPROPERTY()
	UUserWidget* PauseMenu;
	UPROPERTY()
	UUserWidget* OptionsMenu;
	UPROPERTY()
	UNotesToReadWidget* ReadNoteWidget;
	UPROPERTY()
	UPickUpItemWidget* PickUpItemWidget;





};

