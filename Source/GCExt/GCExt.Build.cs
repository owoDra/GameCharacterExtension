﻿// Copyright (C) 2023 owoDra

using UnrealBuildTool;

public class GCExt : ModuleRules
{
	public GCExt(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[]
            {
                ModuleDirectory,
                ModuleDirectory + "/GCExt",
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
                "GameFeatures",
            }
        );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "NetCore",
                "GFCore",
            }
        );

        SetupIrisSupport(Target);
    }
}
