// Fill out your copyright notice in the Description page of Project Settings.


#include "FollowTargetTask.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyCharacter.h"

UFollowTargetTask::UFollowTargetTask() {

    NodeName = "Chase Player";




}



EBTNodeResult::Type UFollowTargetTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();

    if (AIController == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    ACharacter* AICharacter = Cast<ACharacter>(AIController->GetPawn());
    //ANewEnemyCharacter* Enemy = Cast<ANewEnemyCharacter>(AIController->GetPawn());

    if (AICharacter == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (PlayerCharacter == nullptr)
    {
        return EBTNodeResult::Failed;
    }


    float DistanceToPlayer = FVector::Dist(AICharacter->GetActorLocation(), PlayerCharacter->GetActorLocation());


    if (DistanceToPlayer <= AttackRange)
    {
        AIController->StopMovement();

        OwnerComp.GetBlackboardComponent()->SetValueAsBool("bIsAttacking", true);
        return EBTNodeResult::Succeeded;



    }
    else {

        OwnerComp.GetBlackboardComponent()->SetValueAsBool("bIsAttacking", false);
        //OwnerComp.GetBlackboardComponent()->SetValueAsBool("bIsSaw", true);
        //MovementComponent->MaxWalkSpeed = 400.f;


    }



    AIController->MoveToActor(PlayerCharacter);

    return EBTNodeResult::Succeeded;
}