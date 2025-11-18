// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class View3D : ModuleRules
{
	public View3D(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput","RenderCore","RHI","UMG", "Networking", "Sockets"});

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore","Engine","RenderCore", "RHI" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
