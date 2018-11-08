// Fill out your copyright notice in the Description page of Project Settings.

#include "PC.h"
#include "Player/PCh.h"
#include "Player/PCM.h"
#include "Net/UnrealNetwork.h"
#include "UI/NSHUD.h"
#include "Game/GS.h"
#include "Player/PS.h"
#include "Blueprint/UserWidget.h"
#include "ConstructorHelpers.h"

APC::APC()
{
	PlayerCameraManagerClass = APCM::StaticClass();

	/**  set the main menu widget asset */
	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuWidgetClassBP(TEXT("/Game/BP/UI/HUD/MainManu/WBP_MainMenu"));
	if (MainMenuWidgetClassBP.Class)
	{
		MainMenuWidgetClass = MainMenuWidgetClassBP.Class;
	}
}

void APC::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Respawn", IE_Pressed, this, &APC::Respawn);
	InputComponent->BindAction("ToggleTabMenu", IE_Pressed, this, &APC::ToggleTabMenu);
	InputComponent->BindAction("TogglePauseMenu", IE_Pressed, this, &APC::TogglePauseMenu);
	InputComponent->BindAction("EnableChat", IE_Pressed, this, &APC::EnableChat);
	InputComponent->BindAction("DisableChat", IE_Pressed, this, &APC::DisableChat);
}

void APC::BeginPlay()
{
	Super::BeginPlay();

	/** set casted HUD reference  */
	if(ANSHUD* TestNSHUD = Cast<ANSHUD>(GetHUD()))
	{
		NSHUD = TestNSHUD;
	}

	/** Game Only  */
	FInputModeGameOnly InputModeGameOnly;
	SetInputMode(InputModeGameOnly);
}

void APC::Possess(APawn* Pawn)
{
	Super::Possess(Pawn);

	if(APCh* PCh = Cast<APCh>(Pawn))
	{
		PCh->SetPlayerTeamColor();
	}
}

void APC::CharacterPossessed(class APCh* PCh)
{
	OnPossessed.Broadcast(PCh);
}

void APC::LaunchRespawnTimer(float Time)
{
	GetWorldTimerManager().SetTimer(RespawnTImer, this, &APC::Respawn, Time, false);
	if(AGM* GM = Cast<AGM>(GetWorld()->GetAuthGameMode()))
	{
		GM->OnRoundEnded.AddDynamic(this, &APC::RoundEnded);
	}
}

void APC::ServerSendVote_Implementation(int32 MapIndex)
{
	if(AGS* GS = Cast<AGS>(GetWorld()->GetGameState()))
	{
		if (GS->GetMapVoteComp())
		{
			GS->GetMapVoteComp()->SetVote(MapIndex, Cast<APS>(PlayerState));
		}
	}
}

void APC::SendChatMessage(const FText& Message)
{
	ServerSendChatMessage(Message);
}

void APC::SendVote(int32 MapIndex)
{
	ServerSendVote(MapIndex);
}

void APC::ServerSendChatMessage_Implementation(const FText& Message)
{
	if(AGS* GS = Cast<AGS>(GetWorld()->GetGameState()))
	{
		if (GS->GetChatComp())
		{
			GS->GetChatComp()->PostMessage_(Message, Cast<APS>(PlayerState));
		}
	}
}

void APC::ToggleTabMenu()
{
	/** toggle flag  */
	bToggleTabMenu = ! bToggleTabMenu;

	if (NSHUD)
	{
		NSHUD->ToggleHUD( ! bToggleTabMenu );
		NSHUD->ToggleTabMenu(bToggleTabMenu);
	}
	
}

void APC::EnableChat()
{
	if (NSHUD)
	{
		NSHUD->ToggleChat(true);
	}
}

void APC::DisableChat()
{
	if (NSHUD && NSHUD->IsChatActive())
	{
		NSHUD->ToggleChat(false);
	}
}

void APC::TogglePauseMenu()
{
	if (MainMenuWidget)
	{
		MainMenuWidget->RemoveFromParent();
		MainMenuWidget = nullptr;
		
		/** game only  */
		FInputModeGameOnly InputModeGameOnly;
		SetInputMode(InputModeGameOnly);
		bShowMouseCursor = false;
	} 
	else
	{
		if (MainMenuWidgetClass)
		{
			MainMenuWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);
			if (MainMenuWidget)
			{
				MainMenuWidget->AddToViewport(1);
				
				/** UI only  */
				FInputModeUIOnly InputModeUIOnly;
				InputModeUIOnly.SetWidgetToFocus(MainMenuWidget->TakeWidget());
				InputModeUIOnly.SetLockMouseToViewportBehavior(EMouseLockMode::LockOnCapture);
				SetInputMode(InputModeUIOnly);
				bShowMouseCursor = true;
			}
		}
	}	
}

void APC::RoundEnded(FString Reason)
{
	GetWorldTimerManager().ClearTimer(RespawnTImer);
	if(AGM* GM = Cast<AGM>(GetWorld()->GetAuthGameMode()))
	{
		GM->OnRoundEnded.RemoveDynamic(this, &APC::RoundEnded);
	}
}

void APC::Respawn()
{
	/** respawn only if we are spectating now  */
	if(AGameSpectator* GameSpectator = Cast<AGameSpectator>(GetPawn()))
	{
		ServerRespawn();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("! GameSpectator"));
	}
}

void APC::ServerRespawn_Implementation()
{
	if (Role == ROLE_Authority)
	{
		if (AGM* GM = Cast<AGM>(GetWorld()->GetAuthGameMode()))
		{
			if (AGS* GS = Cast<AGS>(GetWorld()->GetGameState()))
			{
				ERoundInProgress RoundInProgress = GS->RoundInProgress;
				if (RoundInProgress == ERoundInProgress::GS_Game)
				{
					GM->SpawnPlayer(this);
					GetWorldTimerManager().ClearTimer(RespawnTImer);
				}
			}
		}
	}
}

void APC::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APC, RespawnTImer, COND_OwnerOnly);
}
