// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LabyrAInth : ModuleRules
{
	public LabyrAInth(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput", "UMG" });
    }
}
