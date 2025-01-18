// Fill out your copyright notice in the Description page of Project Settings.


#include "PistolItem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

APistolItem::APistolItem() {


	AttackDamage = 50.0f;

	ItemData.ItemName = "Pistol";
	ItemData.ItemDescription = "Firegun with a capacity of 15 bullets in its magazine ";


    // Crear un componente de escena para representar el MuzzleSocket
    MuzzleSocketComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleSocket"));

    // Ajustar la ubicación y rotación del socket según la pistola
    MuzzleSocketComponent->SetRelativeLocation(FVector(10.f, 0.f, 0.f));  // Ajustar la ubicación según la pistola
    MuzzleSocketComponent->SetRelativeRotation(FRotator(0.f, 0.f, 0.f)); // Ajustar la rotación si es necesario
    MuzzleSocketComponent->SetupAttachment(Mesh); // GunMesh es un componente existente en el actor
    MuzzleSocketComponent->SetMobility(EComponentMobility::Movable);
}


void APistolItem::BeginPlay()
{
    Super::BeginPlay();


}