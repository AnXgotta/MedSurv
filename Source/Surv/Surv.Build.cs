// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Surv : ModuleRules
{
	public Surv(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG" });
        PrivateIncludePaths.AddRange(new string[]
        {
            "Surv/Player",
            "Surv/Game",
            "Surv/Items",
            "Surv/UI",
            "Surv/Inventory",
            "Surv/Components"
        });
    }
}