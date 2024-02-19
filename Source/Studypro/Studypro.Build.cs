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
			"Json", "JsonUtilities"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}
