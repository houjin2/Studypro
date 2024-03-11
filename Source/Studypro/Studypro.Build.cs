// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Studypro : ModuleRules
{
	public Studypro(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] {
			//InitialModules
			"Core", "CoreUObject", "Engine", "InputCore",
		
			//JsonModules
			"Json", "JsonUtilities",

			//Enhanced Input
			"EnhancedInput",

			//AI
            "NavigationSystem",
            "AIModule",
            "GameplayTasks",

			//UI
			"UMG",
        });

		PrivateDependencyModuleNames.AddRange(new string[] {
			//Custom Modules
			"StudyProjectSettings",
		});
	}
}
