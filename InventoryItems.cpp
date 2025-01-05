// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItems.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "OnDirt2Character.h"
#include "GameFramework/Character.h"

// Sets default values
AInventoryItems::AInventoryItems()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh; // Asigna el StaticMesh como el RootComponent
    Mesh->SetSimulatePhysics(true);
    Mesh->SetNotifyRigidBodyCollision(true);


    // Configuración de colisión
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // Collision Enabled (Query and Physics)
    Mesh->SetCollisionObjectType(ECC_WorldDynamic); // Object Type: WorldDynamic

    // Configurar las respuestas de colisión
    Mesh->SetCollisionResponseToAllChannels(ECR_Ignore); // Ignorar todo por defecto
    Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // Bloquear Visibilidad
    Mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Block); // Bloquear Cámara
    Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // Overlap con Pawn
    Mesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block); // Bloquear WorldStatic
    Mesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap); // Overlap con WorldDynamic
    Mesh->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap); // Overlap con PhysicsBody
    Mesh->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Block); // Bloquear Vehicle
    Mesh->SetCollisionResponseToChannel(ECC_Destructible, ECR_Ignore); // Ignorar Destructible
    // Nota: Si "ThrowOBJ" es un canal personalizado, asegúrate de usarlo aquí.
    Mesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap); // Overlap con ThrowOBJ (asumiendo que este es el canal personalizado)

    // Notificar eventos de colisión física
    Mesh->SetNotifyRigidBodyCollision(true);


    // Crea un componente de colisión (por ejemplo, un box)
    CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
    CollisionComponent->SetupAttachment(RootComponent); // Adjunta la colisión al Mesh
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionComponent->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
    CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // Overlap con Pawns


    // Habilitar las notificaciones de overlap
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AInventoryItems::OnOverlapBegin);
    CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &AInventoryItems::OnOverlapEnd);


    bCanBeGrabbed = false;
}

// Called when the game starts or when spawned
void AInventoryItems::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInventoryItems::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AInventoryItems::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor->IsA(AOnDirt2Character::StaticClass()))
    {
        bCanBeGrabbed = true;

    }
}

void AInventoryItems::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor->IsA(AOnDirt2Character::StaticClass()))
    {

        bCanBeGrabbed = false;

    }
}

