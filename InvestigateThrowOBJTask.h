// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "InvestigateThrowOBJTask.generated.h"

/**
 * 
 */
UCLASS()
class ONDIRT2_API UInvestigateThrowOBJTask : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UInvestigateThrowOBJTask();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


private:

	bool bIsLooking;
	
};
