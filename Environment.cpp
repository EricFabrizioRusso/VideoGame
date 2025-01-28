// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "OnDirt2Character.h"
#include "Blueprint/UserWidget.h"

// Sets default values
AEnvironment::AEnvironment()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;



    /*CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
    CollisionComponent->SetupAttachment(RootComponent); // Adjunta la colisión al Mesh
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionComponent->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
    CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // Overlap con Pawns


    // Habilitar las notificaciones de overlap
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnvironment::OnOverlapBegin);
    CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &AEnvironment::OnOverlapEnd);*/



    PrimaryActorTick.bCanEverTick = true;

    // Crear y configurar el componente de colisión
    BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    BoxComponent->SetCollisionProfileName("Trigger");
    BoxComponent->SetupAttachment(RootComponent);

    // Vincular el evento OnOverlapBegin
    BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnvironment::OnOverlapBegin);
    BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AEnvironment::OnOverlapEnd);


    bCanBeGrabbed = false;

}

// Called when the game starts or when spawned
void AEnvironment::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnvironment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AEnvironment::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor->IsA(AOnDirt2Character::StaticClass()))
    {
        bCanBeGrabbed = true;

    }
}

void AEnvironment::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor->IsA(AOnDirt2Character::StaticClass()))
    {

        bCanBeGrabbed = false;

    }
}

