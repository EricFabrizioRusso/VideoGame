// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "FollowTargetTask.generated.h"

/**
 * 
 */
UCLASS()
class ONDIRT2_API UFollowTargetTask : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UFollowTargetTask();


protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "AI")
	float AttackRange = 200.0f;  // Ajusta el rango de ataque según sea necesario

private:


	
};
