// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PC.generated.h"

/**   */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPossessed, class APCh*, PCh);

/**
 * 
 */
UCLASS()
class NETWORKSHOOTER_API APC : public APlayerController
{
	GENERATED_BODY()
	
public:

	APC();

	virtual void SetupInputComponent() override;

	virtual void BeginPlay() override;

	/** initialize possessed pawn color  */
	virtual void Possess(APawn* Pawn) override;
	
	/** broadcasts to subscribers that character was possessed */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void CharacterPossessed(class APCh* PCh);

	/** enables respawn timer, subscribes to GM round ended delegate  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void LaunchRespawnTimer(float Time);

	/** asks server to send a vote for specified map index */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SendVote(int32 MapIndex);

	/* server votes for specified map index  */
	UFUNCTION(Server, Reliable, WithValidation) // Unreliable
	void ServerSendVote(int32 MapIndex);
	bool ServerSendVote_Validate(int32 MapIndex){ return true; }
	void ServerSendVote_Implementation(int32 MapIndex);

	/** asks server to send a chat message */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SendChatMessage(const FText& Message);

	/* server sends a chat message */
	UFUNCTION(Server, Reliable, WithValidation) // Unreliable
	void ServerSendChatMessage(const FText& Message);
	bool ServerSendChatMessage_Validate(const FText& Message){ return true; }
	void ServerSendChatMessage_Implementation(const FText& Message);
	
	/** broadcasts whenever controlled character is possessed */
	UPROPERTY(BlueprintAssignable)
	FOnPossessed OnPossessed;

protected:

	/** called to toggle pause menu */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void TogglePauseMenu();

private:

	/** called to clear respawn timer, and clear delegates binding */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void RoundEnded(FString Reason);

	/** called to ask server a fast respawn if we are spectating right now */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void Respawn();
	
	/** called to toggle tab menu  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void ToggleTabMenu();

	/** called to enable chat widget  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void EnableChat();

	/** called to disable chat widget  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void DisableChat();
	
	/* server respawn player */
	UFUNCTION(Server, Reliable, WithValidation) // Unreliable
	void ServerRespawn();
	bool ServerRespawn_Validate(){ return true; }
	void ServerRespawn_Implementation();

	UPROPERTY(Replicated)
	FTimerHandle RespawnTImer;

	/** HUD reference  */
	UPROPERTY(BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class ANSHUD* NSHUD;

	/** menu widget class */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> MainMenuWidgetClass;

	/** current menu widget reference */
	UPROPERTY()
	class UUserWidget* MainMenuWidget;

	/** shows whether tab menu is active or not  */
	UPROPERTY(BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	bool bToggleTabMenu = false;
	
};
