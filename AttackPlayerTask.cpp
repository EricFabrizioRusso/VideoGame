// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackPlayerTask.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyCharacter.h"

UAttackPlayerTask::UAttackPlayerTask() {

    NodeName = "Attack Player";
    bIsAttacking = true;


}




EBTNodeResult::Type UAttackPlayerTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (AIController == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    AEnemyCharacter* AICharacter = Cast<AEnemyCharacter>(AIController->GetCharacter());
    if (AICharacter == nullptr)
    {
        return EBTNodeResult::Failed;
    }


    AICharacter->SetIsAttack(bIsAttacking);

    return EBTNodeResult::Succeeded;
}
