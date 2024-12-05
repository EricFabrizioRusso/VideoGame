// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckPoint.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "OnDirt2Character.h"
#include "Blueprint/UserWidget.h"

// Sets default values
ACheckPoint::ACheckPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACheckPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACheckPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


/*void ACheckpoint::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AOnDirt2Character* Character = Cast<AOnDirt2Character>(OtherActor);

	if (!Overlaped) {

		if (Character)
		{
			Character->SetLastCheckpointLocation(GetCheckpointLocation());
			UE_LOG(LogTemp, Warning, TEXT("Checkpoint Reached"));


			if (UUserWidget* WidgetInstance = CreateWidget<UUserWidget>(GetWorld(), Character->CheckPointWidget))
			{
				UE_LOG(LogTemp, Warning, TEXT("Widget created successfully"));
				WidgetInstance->AddToViewport();
				Overlaped = true;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to create widget"));
			}




		}

	}
}

FVector ACheckpoint::GetCheckpointLocation() const
{
	return GetActorLocation();
}

void ACheckpoint::RemoveWidget() {




}*/

