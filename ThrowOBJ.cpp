// Fill out your copyright notice in the Description page of Project Settings.


#include "ThrowOBJ.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "OnDirt2Character.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AThrowOBJ::AThrowOBJ()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

    // Crea un Static Mesh Component
    Tags.Add(FName("ThrowOBJ"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh; // Asigna el StaticMesh como el RootComponent
    Mesh->SetSimulatePhysics(true);
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);


    // Crea un componente de colisión (por ejemplo, un box)
    CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
    CollisionComponent->SetupAttachment(RootComponent); // Adjunta la colisión al Mesh
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionComponent->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
    CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // Overlap con Pawns

    // Habilitar las notificaciones de overlap
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AThrowOBJ::OnOverlapBegin);
    CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &AThrowOBJ::OnOverlapEnd);

    bCanBeGrabbed= false;
}

// Called when the game starts or when spawned
void AThrowOBJ::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AThrowOBJ::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Componente de colisión


}



void AThrowOBJ::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor->IsA(AOnDirt2Character::StaticClass()))
    {
        bCanBeGrabbed = true;
    }
}

void AThrowOBJ::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor->IsA(AOnDirt2Character::StaticClass()))
    {

        bCanBeGrabbed = false;
        
    }
}

