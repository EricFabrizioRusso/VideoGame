// Copyright Epic Games, Inc. All Rights Reserved.

#include "OnDirt2GameMode.h"
#include "OnDirt2Character.h"
#include "UObject/ConstructorHelpers.h"

AOnDirt2GameMode::AOnDirt2GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
