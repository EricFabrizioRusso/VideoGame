// Fill out your copyright notice in the Description page of Project Settings.


#include "AccessLevelTriggerActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "OnDirt2Character.h"
#include "Blueprint/UserWidget.h"

// Sets default values
AAccessLevelTriggerActor::AAccessLevelTriggerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


    // Crear y configurar el componente de colisión
    BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    BoxComponent->SetCollisionProfileName("Trigger");
    BoxComponent->SetupAttachment(RootComponent);

    // Vincular el evento OnOverlapBegin
    BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AAccessLevelTriggerActor::OnOverlapBegin);
    BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AAccessLevelTriggerActor::OnOverlapEnd);

    bIsOverlaped = false; // Inicializar variable de estado
    bIsLocked = false;
    AccessDoor = "Key";

}

// Called when the game starts or when spawned
void AAccessLevelTriggerActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAccessLevelTriggerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AAccessLevelTriggerActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{

    if (OtherActor && OtherActor->IsA(AOnDirt2Character::StaticClass()))
    {
        bIsOverlaped = true;

        UE_LOG(LogTemp, Error, TEXT("Overlap"));
       
    }
}

void AAccessLevelTriggerActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor->IsA(AOnDirt2Character::StaticClass()))
    {

        bIsOverlaped = false;

    }
}


