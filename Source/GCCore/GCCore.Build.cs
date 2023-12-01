// Copyright (C) 2023 owoDra

using UnrealBuildTool;

public class GCCore : ModuleRules
{
	public GCCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[]
            {
                ModuleDirectory,
                ModuleDirectory + "/GCCore",
            }
        );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "ModularGameplay",
                "GameplayTags",
            }
        );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "NetCore",
            }
        );

        SetupIrisSupport(Target);
    }
}
