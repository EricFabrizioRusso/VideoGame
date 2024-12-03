// Fill out your copyright notice in the Description page of Project Settings.


#include "FixedCamera.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "OnDirt2Character.h"

AFixedCamera::AFixedCamera(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
    // Ensure the RootComponent is created if it hasn't been already
    if (!RootComponent)
    {
        RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    }

    // Create the BoxComponent and attach it to the RootComponent
    BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
    //BoxComponent->SetupAttachment(RootComponent);
    // Configurar el BoxComponent como estático
    BoxComponent->SetMobility(EComponentMobility::Static);


}

void AFixedCamera::BeginPlay()
{
    Super::BeginPlay();


    // Obtén al jugador como objetivo
    AActor* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    TargetActor = PlayerCharacter;
}

void AFixedCamera::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bAllowRotation && TargetActor)
    {
        // Calcula la dirección hacia el actor objetivo
        FVector ToTarget = TargetActor->GetActorLocation() - GetActorLocation();
        FRotator TargetRotation = FRotationMatrix::MakeFromX(ToTarget).Rotator();

        // Suavizar la rotación si es necesario
        FRotator SmoothedRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 5.0f);

        // Establece la nueva rotación
        SetActorRotation(SmoothedRotation);
    }
}

void AFixedCamera::EnableRotation()
{
    bAllowRotation = true;
}

void AFixedCamera::DisableRotation()
{
    bAllowRotation = false;
}
