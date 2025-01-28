// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckPoint.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "OnDirt2Character.h"
#include "Blueprint/UserWidget.h"
#include "CheckPointWidget.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGameCheckPoint.h"
#include "EnemyCharacter.h"
#include "InventoryComponent.h"

// Sets default values
ACheckPoint::ACheckPoint()
{
    // Habilitar Tick si es necesario
    PrimaryActorTick.bCanEverTick = true;

    // Crear y configurar el componente de colisión
    BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    BoxComponent->SetCollisionProfileName("Trigger");
    BoxComponent->SetupAttachment(RootComponent);

    // Vincular el evento OnOverlapBegin
    BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ACheckPoint::OnOverlapBegin);

    Overlaped = false; // Inicializar variable de estado
}

// Llamado al inicio del juego
void ACheckPoint::BeginPlay()
{
    Super::BeginPlay();



}

// Llamado cada frame
void ACheckPoint::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Evento de colisión
void ACheckPoint::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    /*AOnDirt2Character* Character = Cast<AOnDirt2Character>(OtherActor);

    if (!Overlaped && Character)
    {
        // Guardar la ubicación del checkpoint
        //Character->SetLastCheckpointLocation(GetCheckpointLocation());

        if (CheckPointWidgetClass) {


            CheckPointWidget = CreateWidget<UCheckPointWidget>(GetWorld(), CheckPointWidgetClass);

            UE_LOG(LogTemp, Warning, TEXT("Se crea el widget"));
        }
        else {


            UE_LOG(LogTemp, Warning, TEXT("No se crea el widget"));
        }



        if (CheckPointWidget && !CheckPointWidget->IsInViewport()) {


            CheckPointWidget->AddToViewport();
            GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ACheckPoint::RemoveWidget, 1.0f, false);
            UE_LOG(LogTemp, Warning, TEXT("Checkpoint Reached"));


        }
        else {


            UE_LOG(LogTemp, Warning, TEXT("Es nulo"));

        }

        // Crear y mostrar el widget del checkpoin
        Overlaped = true; // Marcar checkpoint como activado


        // Guardar la ubicación del checkpoint
        Character->SetLastCheckpointLocation(GetCheckpointLocation());
      


    }*/



    AOnDirt2Character* Character = Cast<AOnDirt2Character>(OtherActor);

    if (!Overlaped && Character)
    {
        // Crear y mostrar el widget del checkpoint
        if (CheckPointWidgetClass)
        {
            CheckPointWidget = CreateWidget<UCheckPointWidget>(GetWorld(), CheckPointWidgetClass);

            if (CheckPointWidget && !CheckPointWidget->IsInViewport())
            {
                CheckPointWidget->AddToViewport();
                GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ACheckPoint::RemoveWidget, 1.0f, false);
                UE_LOG(LogTemp, Warning, TEXT("Checkpoint Reached"));
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Es nulo"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("No se crea el widget"));
        }

        Overlaped = true; // Marcar checkpoint como activado

        // Guardar la ubicación del checkpoint
        Character->SetLastCheckpointLocation(GetCheckpointLocation());

        // --- Lógica de Guardado ---
        // Crear una instancia de SaveGame
        USaveGameCheckPoint* SaveGameInstance = Cast<USaveGameCheckPoint>(UGameplayStatics::CreateSaveGameObject(USaveGameCheckPoint::StaticClass()));
        if (SaveGameInstance)
        {
            // Guardar la posición del jugador
            SaveGameInstance->PlayerLocation = Character->GetActorLocation();

            // Guardar el inventario del jugador
            if (UInventoryComponent* Inventory = Character->InventoryComponent)
            {
                SaveGameInstance->CollectedItems = Inventory->GetInventoryItems();
            }


            // Guardar las posiciones de los enemigos vivos
            TArray<AActor*> EnemyActors;
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacter::StaticClass(), EnemyActors);

            for (AActor* Enemy : EnemyActors)
            {
                if (AEnemyCharacter* EnemyActor = Cast<AEnemyCharacter>(Enemy))
                {
                    if (!EnemyActor->bIsDie) // Método personalizado para verificar si está vivo
                    {
                        SaveGameInstance->EnemyLocations.Add(EnemyActor->GetActorLocation());
                    }
                }
            }

            // Guardar los datos en un slot
            if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("CheckPointSaveSlot"), 0))
            {
                UE_LOG(LogTemp, Warning, TEXT("Datos guardados en el checkpoint"));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Error al guardar los datos del checkpoint"));
            }
        }
    }




}

// Obtener la ubicación del checkpoint
FVector ACheckPoint::GetCheckpointLocation() const
{
    return GetActorLocation();
}


void ACheckPoint::RemoveWidget() {


    if (CheckPointWidget && CheckPointWidget->IsInViewport()) {


        CheckPointWidget->RemoveFromParent();

    }

}



