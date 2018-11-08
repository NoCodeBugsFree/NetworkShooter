// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComp.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOwnerCharacterDead);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStaminaEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnClientDamageReceived, float, Damage, FVector, DamageVector);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class NETWORKSHOOTER_API UHealthComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UHealthComp();

	/** called to start sprinting and drain stamina  */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "Gameplay")
	void StartUsingStamina();

	/** called to stop sprinting and start regenerating stamina */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "Gameplay")
	void StopUsingStamina();

	/** broadcasts whenever owning character is dead */
	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher")
	FOnOwnerCharacterDead OnOwnerCharacterDead;

	/** broadcasts whenever stamina is depleted */
	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher")
	FOnStaminaEnded OnStaminaEnded;

	/** broadcasts whenever we hit by other actor to notify damage direction */
	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher")
	FOnClientDamageReceived OnClientDamageReceived;
	
protected:
	
	virtual void BeginPlay() override;

private:

	/** called whenever owner player takes damage  */
	UFUNCTION()
	void OnOwnerTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	/** called whenever owner player takes point damage */
	UFUNCTION()
	void OnOwnerTakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);

	/** called to notify the player about successful hit other player */
	UFUNCTION(Client, Reliable)
	void ClientNotifyDamage(float Damage, FVector DamageVector);
	void ClientNotifyDamage_Implementation(float Damage, FVector DamageVector);

	/** owner player reference  */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class APCh* PCh;

	/** current health amount */
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float Health = 100.f;

	/** maximum health amount  */
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 100.f;

	/** current armor amount  */
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float Armor = 100.f;

	/** maximum armor amount  */
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float MaxArmor = 100.f;

	/** current stamina amount  */
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float Stamina = 100.f;

	/** maximum stamina amount  */
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float MaxStamina = 100.f;

	UPROPERTY()
	FTimerHandle DecreaseStaminaTimer;

	UPROPERTY()
	FTimerHandle StaminaRegenTimer;
	
	UPROPERTY()
	FTimerHandle StartRegenTimer;

	/** stamina drain timer rate  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float StaminaDecreaseDelay = 0.2f;

	/** stamina drain value rate per DecreaseStaminaTimer tick  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float StaminaDecreaseAmount = 20.f;

	/** stamina regen amount per StaminaRegenTimer tick  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float StaminaRegenAmount = 5.f;

	/** stamina starts regenerate not instantly, but by this delay  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float StartRegenStaminaDelay = 1.f;

	/** shows whether damage was notified once or not  */
	UPROPERTY(BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bDamageNotified : 1;

	/** shows whether owner player is dead or not */
	UPROPERTY(ReplicatedUsing = OnRep_Dead, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bDead : 1;
	
	/* called whenever bDead updated */
	UFUNCTION()
	virtual void OnRep_Dead();

	/** called to actually apply the damage and calculate reminders */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void ApplyDamage(float DamageAmount);

	/** called by DecreaseStaminaTimer to do drain stamina */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void DecreaseStamina();
	
	/** called to starts regeneration  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void StartRegenerateStamina();

	/** called by StaminaRegenTimer to do stamina regen  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void RegenerateStamina();

	/** called to stop regenerate stamina  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void StopRegenerateStamina();

	/** called to show whether we should damage teammate or not */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	bool ShouldDamageTeammate(class APC* InstigatorPC) const;
	
public:
	FORCEINLINE float GetStamina() const { return Stamina; };
	FORCEINLINE float GetMaxStamina() const { return MaxStamina; };
	FORCEINLINE bool  IsDead() const { return bDead; };
	FORCEINLINE float GetHealth() const { return Health; };
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; };
	FORCEINLINE float GetArmor() const { return Armor; };
	FORCEINLINE float GetMaxArmor() const { return MaxArmor; };
};
