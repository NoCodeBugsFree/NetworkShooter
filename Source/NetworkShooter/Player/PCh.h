// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PCh.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDead);

UCLASS()
class NETWORKSHOOTER_API APCh : public ACharacter
{
	GENERATED_BODY()

	/* fps camera   */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent;

	/* arms skeletal mesh  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* NewArms;
	
	/** health component   */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UHealthComp* HealthComp;

	/** weapon manager  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UWeaponManager* WeaponManager;
	
public:
	
	APCh();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** called to ask server to drop current weapon */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void DropWeapon();

	/** called to set player team color  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SetPlayerTeamColor();

	/** player color according his team  */
	UPROPERTY(Replicated)
	FLinearColor PlayerColor = FLinearColor::Red;

	/* called to  */
	UFUNCTION(NetMulticast, Reliable) // Unreliable
	void MulticastRagdoll();
	void MulticastRagdoll_Implementation();

	/** broadcasts whenever this character is dead  */
	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher")
	FOnCharacterDead OnCharacterDead;
	
protected:
	
	virtual void BeginPlay() override;

private:

	/** time to destroy this character after death  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float DestroyTime = 3.f;

	/** minimum amount of stamina to be able to use it  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float SprintMinStamina = 10.f;

	/** sprint speed (requires at least SprintMinStamina to start)
		constantly drains stamina
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float SprintSpeed = 900.f;

	/** initial jog speed  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float DefaultWalkSpeed = 600.f;

	/** time to destroy this character after death  */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float AimPitch;

	/** shows whether is  or not  */
	UPROPERTY(BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bSecondaryActionToggled : 1;

	/** interaction distance (5m default) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float InteractionDistance = 500.f;
	
	// -----------------------------------------------------------------------------------
	
	/** select specific weapon  */

	/** called to select main weapon */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SelectMainWeapon();
	
	/** called to select second weapon */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SelectSecondWeapon();

	/** called to melee main weapon */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SelectMeleeWeapon();

	/** called to select grenade */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SelectGrenade();

	/** called to select special weapon */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SelectSpecial();

	/** scroll weapon up/down  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void ScrollWeaponUp();
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void ScrollWeaponDown();

	/** called to crouch  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void StartCrouch();

	/** called to uncrouch  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void StopCrouch();

	/** called to move forward/backward  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void MoveForward(float Value);

	/** called to move right/left  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void MoveRight(float Value);

	/** called to look up/down  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void LookUp(float Value);

	/** called to turn around  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void Turn(float Value);

	/** asks server to start fire  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void StartFire();
	/** asks server to stop fire  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void StopFire();

	/** called to ask server to use secondary weapon ability (if specified) */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void WeaponSecondary();

	/** called to ask server to reload the weapon  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void Reload();

	/** called to ask server to interact */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void Interact();

	/* called to do interaction on server */
	UFUNCTION(Server, Reliable, WithValidation) // Unreliable
	void ServerInteract();
	bool ServerInteract_Validate(){ return true; }
	void ServerInteract_Implementation();

	/** called to ask server to start/stop sprinting */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void StartSprint();

	UFUNCTION(BlueprintCallable, Category = "AAA")
	void StopSprint();

	/* called to start/stop sprinting */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSprint(bool bStart);
	bool ServerSprint_Validate(bool bStart){ return true; }
	void ServerSprint_Implementation(bool bStart);

	/** called to line trace to interact */
	UFUNCTION(BlueprintPure, Category = "AAA")
	bool InteractTrace(FHitResult& Hit) const;

	/** called whenever this character is dead */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void CharacterDead();

	/** called to stop sprinting when stamina is ended */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void StaminaEnded();
	
public:	

	FORCEINLINE bool CanSprint() const;
	FORCEINLINE class UWeaponManager* GetWeaponManager() const { return WeaponManager; }
	FORCEINLINE class USkeletalMeshComponent* GetArms() const { return NewArms; }
	FORCEINLINE class USkeletalMeshComponent* GetVisibleMesh() const { return IsLocallyControlled() ? NewArms : GetMesh(); }
	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	FORCEINLINE float GetAimPitch() const { return AimPitch; };
	FORCEINLINE class UHealthComp* GetHealthComponent() const { return HealthComp; }
};
