// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ChatComp.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMessagePosted, FText, Message, class APS*, PS);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class NETWORKSHOOTER_API UChatComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UChatComp();

	/** broadcasts whenever current message is posted */
	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher")
	FOnMessagePosted OnMessagePosted;
	
	/** called to post a message */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void PostMessage_(const FText& Message, class APS* PS);

protected:
	
	/* called to multicast that message was posted */
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastAllMessagePosted(const FText& Messge, class APS* PS);
	void MulticastAllMessagePosted_Implementation(const FText& Messge, class APS* PS);
};
