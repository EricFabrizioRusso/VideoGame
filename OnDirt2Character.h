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

	// Función para agarrar el objeto
	UFUNCTION()
	void GrabThrowOBJ();
	void PickUpHandle();
	void DropOBJ();


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


private:
	void Sprinting();
	void Crouching();
	void Aiming();
	void StopAiming();
	void ThrowOBJ();
	void ExecuteThrow();
	void NotifyActorBeginOverlap(AActor* OtherActor);
	void NotifyActorEndOverlap(AActor* OtherActor);

	//Stats
	float Life;


	bool bCanCrouch;
	bool bIsHoldingObject;
	bool bIsAiming;
	bool bIsThrowing;
	bool bIsPickUp;
	bool bIsHit;
	bool bIsDie;
	FTimerHandle ThrowTimerHandle;

	
	UPROPERTY()
	class AFixedCamera* FixedCameraActor;

	bool bUsingFixedCamera;


};

