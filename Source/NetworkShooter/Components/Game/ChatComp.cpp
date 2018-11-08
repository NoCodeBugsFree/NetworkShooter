// Fill out your copyright notice in the Description page of Project Settings.

#include "ChatComp.h"
#include "Player/PS.h"

UChatComp::UChatComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicated(true);
}

void UChatComp::PostMessage_(const FText& Message, APS* PS)
{
	MulticastAllMessagePosted(Message, PS);
}

void UChatComp::MulticastAllMessagePosted_Implementation(const FText& Messge, APS* PS)
{
	OnMessagePosted.Broadcast(Messge, PS);
}
