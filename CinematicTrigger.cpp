// Fill out your copyright notice in the Description page of Project Settings.


#include "CinematicTrigger.h"
#include "OnDirt2Character.h"
#include "EnemyCharacter.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlayer.h"
#include "Camera/CameraComponent.h"
#include "EngineUtils.h"

// Sets default values
ACinematicTrigger::ACinematicTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


    // Configurar el RootComponent
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    // Crear el Box Collision
    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetupAttachment(RootComponent);

    // Configurar el Box Collision
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

    CinematicActivated = false;


}

// Called when the game starts or when spawned
void ACinematicTrigger::BeginPlay()
{
	Super::BeginPlay();

    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ACinematicTrigger::OnOverlapBegin);
	
}

// Called every frame
void ACinematicTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}





void ACinematicTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

    // Verificar si el actor es el personaje principal

    if (OtherActor && OtherActor->IsA(AOnDirt2Character::StaticClass()))
    {
        StartCinematic();
    }
 
}

void ACinematicTrigger::StartCinematic()
{

    if (!CinematicActivated) {


        if (CinematicSequence)
        {
            // Configurar el PlaybackSettings
            FMovieSceneSequencePlaybackSettings PlaybackSettings;
            PlaybackSettings.bAutoPlay = false;  // Establecer auto-play en PlaybackSettings
            //PlaybackSettings.bHidePlayer = true;

            // Crear el SequenceActor y SequencePlayer
            ALevelSequenceActor* SequenceActor;
            ULevelSequencePlayer* SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
                GetWorld(),
                CinematicSequence,
                PlaybackSettings,
                SequenceActor
            );

            if (SequencePlayer)
            {
                // Asegurar que la secuencia se asigne correctamente
                SequenceActor->SetSequence(CinematicSequence);
             

                // Ocultar al personaje principal
                AOnDirt2Character* PlayerCharacter = Cast<AOnDirt2Character>(GetWorld()->GetFirstPlayerController()->GetPawn());
                if (PlayerCharacter)
                {
                    PlayerCharacter->SetActorHiddenInGame(true);
                    PlayerCharacter->SetActorEnableCollision(false);
                    PlayerCharacter->SetActorTickEnabled(false);
                    
                }

                // Ocultar a todos los enemigos en la escena
                for (TActorIterator<AEnemyCharacter> It(GetWorld()); It; ++It)
                {
                    AEnemyCharacter* Enemy = *It;
                    if (Enemy)
                    {
                        Enemy->SetActorHiddenInGame(true);
                        Enemy->SetActorEnableCollision(false);
                        Enemy->SetActorTickEnabled(false);
                    }
                }

                // Iniciar la cinemática
                SequencePlayer->Play();

                // Cuando la cinemática termine, restauramos el control del jugador y enemigos
                SequencePlayer->OnFinished.AddDynamic(this, &ACinematicTrigger::OnCinematicFinished);

                CinematicActivated = true;
            }


            
        }

      
    }


}











void ACinematicTrigger::OnCinematicFinished()
{
    // Restaurar visibilidad del personaje principal
    AOnDirt2Character* PlayerCharacter = Cast<AOnDirt2Character>(GetWorld()->GetFirstPlayerController()->GetPawn());
    if (PlayerCharacter)
    {
        PlayerCharacter->SetActorHiddenInGame(false);
        PlayerCharacter->SetActorEnableCollision(true);
        PlayerCharacter->SetActorTickEnabled(true);

       // UE_LOG(LogTemp, Error, TEXT("Cinematica terminada"));

    }

    // Restaurar visibilidad de los enemigos
    for (TActorIterator<AEnemyCharacter> It(GetWorld()); It; ++It)
    {
        AEnemyCharacter* Enemy = *It;
        if (Enemy)
        {
            Enemy->SetActorHiddenInGame(false);
            Enemy->SetActorEnableCollision(true);
            Enemy->SetActorTickEnabled(true);
        }
    }
    //UE_LOG(LogTemp, Warning, TEXT("Cinematica terminada"));

}
