// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Demo : ModuleRules
{
    public Demo(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {
                "Demo"
            }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "EnhancedInput",
                "GameplayTags"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
            }
        );
    }
}