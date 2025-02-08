// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OnDirt2 : ModuleRules
{
	public OnDirt2(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Niagara", "MovieScene","LevelSequence", "PhysicsCore" });

        PrivateDependencyModuleNames.AddRange(new string[] { "UMG", "Slate", "SlateCore" });
    }
}
