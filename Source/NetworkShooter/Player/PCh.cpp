// Fill out your copyright notice in the Description page of Project Settings.

#include "PCh.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/Player/HealthComp.h"
#include "Components/Player/WeaponManager.h"
#include "Net/UnrealNetwork.h"
#include "Player/TeamPS.h"
#include "Weapons/FireWeapon.h"
#include "Components/CapsuleComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"

/** TODO Bugs
*	comment + git
*   UMapVoteComp::GetVotedMap() bug here /** REWRITE VOTE COMP!  */


APCh::APCh()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCapsuleHalfHeight(90.f);
	GetCapsuleComponent()->SetCapsuleRadius(30.f);
	
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	
	/* fps camera   */
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(RootComponent);
	CameraComponent->PostProcessSettings.AutoExposureMinBrightness = 1.f;
	CameraComponent->SetRelativeLocation(FVector(0.f, 0.f, 70.f));
	CameraComponent->bUsePawnControlRotation = true;

	/* arms skeletal mesh  */
	NewArms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("NewArms"));
	NewArms->SetupAttachment(CameraComponent);
	NewArms->SetRelativeLocation(FVector(0.0f, 0.0f, -160.f));
	NewArms->CastShadow = false;
	NewArms->bEnablePhysicsOnDedicatedServer = false;
	
	/** set up character movement component  */
	/** enable crouching  */
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 150.f;
	GetCharacterMovement()->CrouchedHalfHeight = 50.f;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	
	/** health component   */
	HealthComp = CreateDefaultSubobject<UHealthComp>(TEXT("HealthComp"));

	/** health component   */
	WeaponManager = CreateDefaultSubobject<UWeaponManager>(TEXT("WeaponManager"));
}

void APCh::BeginPlay()
{
	Super::BeginPlay();
	
	if (Role == ROLE_Authority)
	{
		/** setup DefaultWalkSpeed  */
		DefaultWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
		/** bind delegates */
		HealthComp->OnOwnerCharacterDead.AddDynamic(this, &APCh::CharacterDead);
		HealthComp->OnStaminaEnded.AddDynamic(this, &APCh::StaminaEnded);
	}

	/** broadcast that character was possessed  */
	if(APC* PC = Cast<APC>(GetController()))
	{
		PC->CharacterPossessed(this);
	}

	/** remove meshes from Dedicated Server   */
	if (GetWorld() && GetWorld()->GetNetMode() == NM_DedicatedServer)
	{
		GetMesh()->DestroyComponent();
		NewArms->DestroyComponent();
	}
	
	/** remove redundant meshes on clients  */
	SetPlayerTeamColor();
	if (IsLocallyControlled())
	{
		GetMesh()->DestroyComponent();
	}
	else
	{
		NewArms->DestroyComponent();
	}
}

void APCh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Role == ROLE_Authority)
	{
		/** replicate aim pitch  */
		if (GetController())
		{
		 	FVector ForwardVector = FRotationMatrix(GetControlRotation()).GetScaledAxis(EAxis::X);
			AimPitch = FRotationMatrix::MakeFromX(ForwardVector).Rotator().Pitch;
		}
	}
}

void APCh::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/** movement  */
	PlayerInputComponent->BindAxis("MoveForward", this, &APCh::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APCh::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &APCh::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &APCh::Turn);

	/** crouch  */
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APCh::StartCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &APCh::StopCrouch);

	/** jump  */
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APCh::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APCh::StopJumping);

	/** fire  */
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APCh::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &APCh::StopFire);

	/** WeaponSecondary  */
	PlayerInputComponent->BindAction("WeaponSecondary", IE_Pressed, this, &APCh::WeaponSecondary);

	/** Reload */
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APCh::Reload);

	/** Interact  */
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APCh::Interact);

	/** DropWeapon */
	PlayerInputComponent->BindAction("DropWeapon", IE_Pressed, this, &APCh::DropWeapon);

	/** Sprint  */
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APCh::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APCh::StopSprint);
	
	/** select weapon  */
	PlayerInputComponent->BindAction("SelectMainWeapon", IE_Pressed, this, &APCh::SelectMainWeapon);
	PlayerInputComponent->BindAction("SelectSecondWeapon", IE_Pressed, this, &APCh::SelectSecondWeapon);
	PlayerInputComponent->BindAction("SelectMeleeWeapon", IE_Pressed, this, &APCh::SelectMeleeWeapon);
	PlayerInputComponent->BindAction("SelectGrenade", IE_Pressed, this, &APCh::SelectGrenade);
	PlayerInputComponent->BindAction("SelectSpecial", IE_Pressed, this, &APCh::SelectSpecial);

	/** scroll weapon  */
	PlayerInputComponent->BindAction("ScrollWeaponUp", IE_Pressed, this, &APCh::ScrollWeaponUp);
	PlayerInputComponent->BindAction("ScrollWeaponDown", IE_Pressed, this, &APCh::ScrollWeaponDown);

	/** drop weapon  */
	PlayerInputComponent->BindAction("DropWeapon", IE_Pressed, this, &APCh::DropWeapon);
}

void APCh::SelectMainWeapon()
{
	WeaponManager->SelectWeapon(EWeaponType::WT_Main);
}

void APCh::SelectSecondWeapon()
{
	WeaponManager->SelectWeapon(EWeaponType::WT_Secondary);
}

void APCh::SelectMeleeWeapon()
{
	WeaponManager->SelectWeapon(EWeaponType::WT_Melee);
}

void APCh::SelectGrenade()
{
	WeaponManager->SelectWeapon(EWeaponType::WT_Grenade);
}

void APCh::SelectSpecial()
{
	WeaponManager->SelectWeapon(EWeaponType::WT_Special);
}

void APCh::ScrollWeaponUp()
{
	WeaponManager->ScrollWeapon(false);
}

void APCh::ScrollWeaponDown()
{
	WeaponManager->ScrollWeapon(true);
}

void APCh::StartCrouch()
{
	Crouch();
}

void APCh::StopCrouch()
{
	UnCrouch();
}

void APCh::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector(), Value);
}

void APCh::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
}

void APCh::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void APCh::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void APCh::StartFire()
{
	if (WeaponManager->GetCurrentWeapon())
	{
		WeaponManager->GetCurrentWeapon()->ToggleUseWeapon(true);
	}
}

void APCh::StopFire()
{
	if (WeaponManager->GetCurrentWeapon())
	{
		WeaponManager->GetCurrentWeapon()->ToggleUseWeapon(false);
	}
}

void APCh::Reload()
{
	if(AFireWeapon* FireWeapon = Cast<AFireWeapon>(WeaponManager->GetCurrentWeapon()))
	{
		FireWeapon->ReloadWeapon();
	}
}

void APCh::Interact()
{
	FHitResult Hit;
	if (InteractTrace(Hit))
	{
		if (Hit.GetActor())
		{
			if (Hit.GetActor()->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
			{
				ServerInteract();
			}
		}
	}
}

void APCh::ServerInteract_Implementation()
{
	if (Role == ROLE_Authority)
	{
		FHitResult Hit;
		if (InteractTrace(Hit))
		{
			if (Hit.GetActor() && Hit.GetActor()->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
			{
				IInteractableInterface::Execute_Interact(Hit.GetActor(), this);
			}
		}
	}
}

void APCh::DropWeapon()
{
	WeaponManager->DropWeapon(WeaponManager->GetCurrentWeapon());
}

void APCh::MulticastRagdoll_Implementation()
{
	if (NewArms)
	{
		NewArms->SetVisibility(false);
	}

	if (GetMesh())
	{
		static FName CollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetCollisionProfileName(CollisionProfileName);

		SetActorEnableCollision(true);

		// rag doll  (init Physics)
		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->WakeAllRigidBodies();
		GetMesh()->bBlendPhysics = true;
	}

	// disable movement
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);

	// disable collisions on the capsule
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void APCh::StartSprint()
{
	ServerSprint(true);
}

void APCh::StopSprint()
{
	ServerSprint(false);
}

void APCh::ServerSprint_Implementation(bool bStart)
{
	if (Role == ROLE_Authority)
	{
		if (bStart) /** Start sprint  */
		{
			if (CanSprint())
			{
				GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
				if (HealthComp)
				{
					HealthComp->StartUsingStamina();
				}
			}
		}
		else /** Stop sprint  */
		{
			GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
			if (HealthComp)
			{
				HealthComp->StopUsingStamina();
			}
		}
	}
}

bool APCh::CanSprint() const
{
	return HealthComp->GetStamina() > SprintMinStamina;
}

void APCh::SetPlayerTeamColor()
{
	if(ATeamPS* TeamPS = Cast<ATeamPS>(PlayerState))
	{
		/** collects all meshes  */
		TArray<UMeshComponent*> Meshes;
		for (UActorComponent* Mesh : GetComponentsByClass(UMeshComponent::StaticClass()))
		{
			UMeshComponent* ThisMesh = Cast<UMeshComponent>(Mesh);
			if (ThisMesh)
			{
				Meshes.Push(ThisMesh);
			}
		}

		/** set team color  */
		for (UMeshComponent* Mesh : Meshes)
		{
			if (Mesh)
			{
				UMaterialInstanceDynamic* DMI = Mesh->CreateDynamicMaterialInstance(0);
				if (DMI)
				{
					DMI->SetVectorParameterValue("BodyColor", PlayerColor);
					/*ETeam Team = TeamPS->GetTeam();

					if (Team == ETeam::TeamA)
					{
						DMI->SetVectorParameterValue("BodyColor", FLinearColor::Red);
					}
					else
					{
						DMI->SetVectorParameterValue("BodyColor", FLinearColor::Blue);
					}*/
				}
			}
		}
	}
}

bool APCh::InteractTrace(FHitResult& Hit) const
{	
	float Distance = 10000.f;
	
	FVector Direction = FRotationMatrix(GetControlRotation()).GetScaledAxis(EAxis::X);
	FVector Start = CameraComponent->GetComponentLocation();
	FVector End = Start + (Direction * InteractionDistance);
	
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	CollisionQueryParams.bTraceComplex = false;

	return GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionQueryParams);
}

void APCh::CharacterDead()
{
	if (HealthComp->IsDead())
	{
		/** I am dead  */
		OnCharacterDead.Broadcast();

		/** disable movement  */
		GetCharacterMovement()->DisableMovement();
		
		/** destroy pawn  */
		SetLifeSpan(DestroyTime);
	}
}

void APCh::StaminaEnded()
{
	StopSprint();
}

void APCh::WeaponSecondary()
{
	/** toggle flag  */
	bSecondaryActionToggled = !bSecondaryActionToggled;
	if (WeaponManager->GetCurrentWeapon())
	{
		WeaponManager->GetCurrentWeapon()->ToggleSecondaryAction(bSecondaryActionToggled);
	}
}

void APCh::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APCh, AimPitch);
	
	DOREPLIFETIME_CONDITION(APCh, PlayerColor, COND_InitialOnly);
}