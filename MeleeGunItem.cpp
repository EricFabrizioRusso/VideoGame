// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeGunItem.h"
#include "EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"

AMeleeGunItem::AMeleeGunItem() {

	AttackDamage = 30.0f;

	ItemData.ItemName = "Bat";
	ItemData.ItemDescription = "Firegun with a capacity of 15 bullets in its magazine ";



    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Mesh->SetCollisionObjectType(ECC_WorldDynamic);
    Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);


    // Habilitar eventos de superposición
    Mesh->SetGenerateOverlapEvents(true);

}


void AMeleeGunItem::BeginPlay()
{
    Super::BeginPlay();


    if (Mesh)
    {
        Mesh->OnComponentBeginOverlap.AddDynamic(this, &AMeleeGunItem::OnMeshBeginOverlap);
    }

}


void AMeleeGunItem::OnMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    UE_LOG(LogTemp, Log, TEXT("Overlpea con algo el MeleeGun"));

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(OtherActor);
    if (Enemy)
    {
        UE_LOG(LogTemp, Log, TEXT("MeleeGunItem impactó a: %s"), *Enemy->GetName());

        // Aplica daño al enemigo
        UGameplayStatics::ApplyDamage(Enemy, AttackDamage, nullptr, this, nullptr);
    }
}


void AMeleeGunItem::UpdateTexts(FString Language) {



    if (Language == "Spanish") {


        ItemData.ItemName = "Bate";
        ItemData.ItemDescription = "Arma cuerpo a cuerpo efectiva a corto alcance pero mas poderosa que un arma de fuego";


    }
    else {


        ItemData.ItemName = "Bat";
        ItemData.ItemDescription = "Melee weapon efective in short encounters but most powerful than fireguns";



    }


}






