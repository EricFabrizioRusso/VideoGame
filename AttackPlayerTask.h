// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "AttackPlayerTask.generated.h"

/**
 * 
 */
UCLASS()
class ONDIRT2_API UAttackPlayerTask : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UAttackPlayerTask();
	UPROPERTY(EditAnywhere, Category = "AI")
	bool bIsAttacking;


protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "AI")
	float AttackRange = 110.0f;  // Ajusta el rango de ataque según sea necesario
	
};
