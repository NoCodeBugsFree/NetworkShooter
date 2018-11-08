// Fill out your copyright notice in the Description page of Project Settings.

#include "NetworkShooterGameSession.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionInterface.h"

void ANetworkShooterGameSession::RegisterServer()
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	IOnlineSessionPtr OnlineSessionPtr = OnlineSubsystem->GetSessionInterface();
	
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsDedicated = true;
	SessionSettings.bIsLANMatch = true;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.NumPublicConnections = 16;
	SessionSettings.bUsesPresence = true;

	OnlineSessionPtr->CreateSession(0, GameSessionName, SessionSettings);

	UE_LOG(LogTemp, Warning, TEXT("Session Created! -------------------------- "));
}
