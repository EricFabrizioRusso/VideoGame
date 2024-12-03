// Fill out your copyright notice in the Description page of Project Settings.


#include "LastKnownLocationTask.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "OnDirt2Character.h"

ULastKnownLocationTask::ULastKnownLocationTask()
{
    NodeName = "Update Last Known Location";
}

EBTNodeResult::Type ULastKnownLocationTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (AIController == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    APawn* AIPawn = AIController->GetPawn();
    if (AIPawn == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (BlackboardComp == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    // Obtener la ubicación del jugador
    AOnDirt2Character* PlayerCharacter = Cast<AOnDirt2Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (PlayerCharacter == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    FVector PlayerLocation = PlayerCharacter->GetActorLocation();
    BlackboardComp->SetValueAsVector("LastKnownLocation", PlayerLocation);

    return EBTNodeResult::Succeeded;
}