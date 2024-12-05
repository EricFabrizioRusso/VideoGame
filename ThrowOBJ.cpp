// Fill out your copyright notice in the Description page of Project Settings.


#include "ThrowOBJ.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "OnDirt2Character.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Perception/AISense_Hearing.h"

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
    Mesh->SetNotifyRigidBodyCollision(true);
    Mesh->OnComponentHit.AddDynamic(this, &AThrowOBJ::OnImpact);


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
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AThrowOBJ::OnOverlapBegin);
    CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &AThrowOBJ::OnOverlapEnd);

    bCanBeGrabbed= false;
    bNoiseEmitted = false;
    bIsRealising = false;
}

// Called when the game starts or when spawned
void AThrowOBJ::BeginPlay()
{
	Super::BeginPlay();
    bIsRealising = true;
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


// Impact Sound
void AThrowOBJ::OnImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    FVector ImpactLocation = GetActorLocation();
    // Emitir un estímulo auditivo en la ubicación del impacto

   // UE_LOG(LogTemp, Warning, TEXT("Entra al Impact"));

    if (bIsRealising == true) return;
    UE_LOG(LogTemp, Warning, TEXT("Making Sound"));
    if (!bNoiseEmitted)
    {
        UAISense_Hearing::ReportNoiseEvent(GetWorld(), ImpactLocation, 1.0f, this, 0.0f, TEXT("Noise"));
        MakeNoise(1.f, nullptr, ImpactLocation, 0.f, "Noise");
        bNoiseEmitted = true; // Mark noise as emitted
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AThrowOBJ::HandleNoiseTime, 1.5f, false);
    }
    // Puedes añadir lógica adicional aquí si es necesario
}

void AThrowOBJ::HandleNoiseTime() {


    bNoiseEmitted = false;
}



