// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "LastKnownLocationTask.generated.h"

/**
 * 
 */
UCLASS()
class ONDIRT2_API ULastKnownLocationTask : public UBTTaskNode
{
	GENERATED_BODY()

public:
	ULastKnownLocationTask();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
