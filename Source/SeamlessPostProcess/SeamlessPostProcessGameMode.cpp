// Copyright Epic Games, Inc. All Rights Reserved.

#include "SeamlessPostProcessGameMode.h"
#include "SeamlessPostProcessCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASeamlessPostProcessGameMode::ASeamlessPostProcessGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
