// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MinimapHelper.generated.h"

UCLASS()
class NETWORKSHOOTER_API AMinimapHelper : public AActor
{
	GENERATED_BODY()
	
	/* scene component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Root;
	
	/* box collision to spread through the map to capture */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BoxComponent;
	
	/* scene capture component 2D  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USceneCaptureComponent2D* SceneCaptureComponent2D;

	/* billboard component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UBillboardComponent* BillboardComponent;	
	
public:	
	
	AMinimapHelper();
	
	/** called to get player coordinates to the minimap */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AAA")
	TMap<class APCh*, FVector2D> GetPlayerCoordinates();
	
	/** called to convert player 3D world location to 2D minimap location */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AAA")
	FVector2D ConvertWordlToMapLocation(FVector WorldLocation);

protected:

	/** called to set box extent according MapSize  */
	virtual void OnConstruction(const FTransform& Transform) override;

private:

	/** called to capture the scene in the editor */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Config")
	void CaptureMapTexture();

	/** current map extent  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float MapSize = 5500.f;

	/** the texture storage for the captured map image  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UTexture2D* MapTexture;
};
