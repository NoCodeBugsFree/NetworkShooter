// Fill out your copyright notice in the Description page of Project Settings.

#include "MinimapHelper.h"
#include "Components/BoxComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "ConstructorHelpers.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PCh.h"
#include "Components/BillboardComponent.h"

AMinimapHelper::AMinimapHelper()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	/* scene component  */
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	/* box collision  */
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(RootComponent);
	BoxComponent->SetGenerateOverlapEvents(false);
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoxComponent->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	
	/* scene capture component 2D  */
	SceneCaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent2D"));
	SceneCaptureComponent2D->SetupAttachment(BoxComponent);
	SceneCaptureComponent2D->ProjectionType = ECameraProjectionMode::Orthographic;
	SceneCaptureComponent2D->CaptureSource = ESceneCaptureSource::SCS_SceneColorHDRNoAlpha;
	SceneCaptureComponent2D->bCaptureEveryFrame = false;
	SceneCaptureComponent2D->bCaptureOnMovement = false;

	/**  set the texture */
	static ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> Texture(TEXT("/Game/BP/Tools/Minimap/T_MiniMap"));
	if (Texture.Succeeded())
	{
		SceneCaptureComponent2D->TextureTarget = Texture.Object;
	}

	/* billboard  */
	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("BillboardComponent"));
	BillboardComponent->SetupAttachment(SceneCaptureComponent2D);
	BillboardComponent->SetRelativeScale3D(FVector(10.f, 10.f, 10.f));
}

void AMinimapHelper::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	/** called to set box extent according MapSize  */
	BoxComponent->SetBoxExtent(FVector(2.f, MapSize / 2.f, MapSize / 2.f));

	/** update ortho width  */
	SceneCaptureComponent2D->OrthoWidth = MapSize;
}

void AMinimapHelper::CaptureMapTexture()
{
	SceneCaptureComponent2D->CaptureScene();
}

FVector2D AMinimapHelper::ConvertWordlToMapLocation(FVector WorldLocation)
{
	FVector ToRotate = FVector(MapSize / -2.f, MapSize / 2.f, 0.f);
	FVector RotatedVector = GetActorRotation().RotateVector(ToRotate);
	FVector ToUnrotate = (GetActorLocation() - RotatedVector) - WorldLocation;
	FVector To2D = GetActorRotation().UnrotateVector(ToUnrotate);
	FVector2D Result(To2D.X, To2D.Y * -1.f);
	
	return Result;
}

TMap<class APCh*, FVector2D> AMinimapHelper::GetPlayerCoordinates()
{
	TMap<class APCh*, FVector2D> Result;
	
	/** TODO no GetAllActorsOfClass per tick 
	*	create replicated array of players during player spawning (update it each respawn)
	*	draw other team player only if you see him
	*/
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APCh::StaticClass(), FoundActors);
	
	for (AActor* Actor : FoundActors)
	{
		APCh* TestActor = Cast<APCh>(Actor);
		if (TestActor)
		{
			Result.Add(TestActor, ConvertWordlToMapLocation(TestActor->GetActorLocation()));
		}
	}

	return Result;
}
