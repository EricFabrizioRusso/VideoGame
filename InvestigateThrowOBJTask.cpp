// Fill out your copyright notice in the Description page of Project Settings.


#include "InvestigateThrowOBJTask.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyCharacter.h"
#include "Navigation/PathFollowingComponent.h"



UInvestigateThrowOBJTask::UInvestigateThrowOBJTask() {


    NodeName = "Activate Anim";
    bIsLooking = true;
}

EBTNodeResult::Type UInvestigateThrowOBJTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (AIController == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (BlackboardComp == nullptr)
    {
        return EBTNodeResult::Failed;
    }
    AEnemyCharacter* AICharacter = Cast<AEnemyCharacter>(AIController->GetCharacter());


    AICharacter->SetIsLooking(bIsLooking);
    return EBTNodeResult::Succeeded;
}