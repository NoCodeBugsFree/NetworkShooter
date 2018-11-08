// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "NSHUD.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOOTER_API ANSHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	/** called to enable/disable HUD menu widget */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "AAA")
	void ToggleHUD(bool bEnable);

	/** called to enable/disable tab menu widget */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "AAA")
	void ToggleTabMenu(bool bEnable);

	/** called to enable/disable spectate widget */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "AAA")
	void ToggleSpectate(bool bEnable);

	/** called to enable/disable pre round widget*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "AAA")
	void TogglePreRound(bool bEnable);

	/** called to enable/disable */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "AAA")
	void TogglePostRound(bool bEnable);

	/** called to create chat widget */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "AAA")
	void CreateChat();

	/** called to enable/disable chat widget */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "AAA")
	void ToggleChat(bool bActive);

	/** shows whether chat is active or not  */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "AAA")
	bool IsChatActive() const;

	/** called to set new input mode and widget to focus if needed */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "AAA")
	void SetInputMode(bool bUI, class UUserWidget* WidgetToFocus);
	
	/** called whenever owner player is dead */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AAA")
	void PlayerDead(class APCh* InstigatorCharecter, class AActor* DeathCauser, class APS* PS);
	virtual void PlayerDead_Implementation(class APCh* InstigatorCharecter, class AActor* DeathCauser, class APS* PS);
	
protected:

	virtual void BeginPlay() override;

private:

	/** called whenever round is started, enables required widgets */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void RoundStarted();

	/** called whenever round is ended, disables required widgets */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void RoundEnded(FString Reason);

	/** called whenever owner player is respawned, enables required widgets  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void PlayerAlive();
	
	/** game state reference  */
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class AGS* GS;
};
