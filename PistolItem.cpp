// Fill out your copyright notice in the Description page of Project Settings.


#include "PistolItem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "OnDirt2Character.h"
#include "Kismet/GameplayStatics.h"

APistolItem::APistolItem() {


	AttackDamage = 50.0f;
    Ammunation = 5;
	ItemData.Quantity = 1;

	//ItemData.ItemName = "Pistol";
	//ItemData.ItemDescription = "Firegun with a capacity of 15 bullets in its magazine ";


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

	FString SavedLanguage;
	if (GConfig->GetString(TEXT("/Script/Engine.GameUserSettings"), TEXT("SelectedLanguage"),
		SavedLanguage, GGameUserSettingsIni))
	{
		UpdateTexts(SavedLanguage);
	}
	else
	{
		UpdateTexts("English"); // Por defecto
	}


}



void APistolItem::UpdateTexts(FString Language) {



	if (Language == "Spanish") {


		ItemData.ItemName = "Pistola";
		ItemData.ItemDescription = "Arma de fuego con capacidad de 10 balas en su cargador";


	}
	else {


		ItemData.ItemName = "Pistol";
		ItemData.ItemDescription = "Firegun with a capacity of 10 bullets in magazine ";



	}







}