// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"

void PrintString(const FString& inputString)
{
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, inputString);
	}
	
}

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
{
	PrintString("MSS Constructor");
}

void UMultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	PrintString("MSS Init");
}

void UMultiplayerSessionsSubsystem::Deinitialize()
{
	UE_LOG(LogTemp, Warning, TEXT("de init"));
}



