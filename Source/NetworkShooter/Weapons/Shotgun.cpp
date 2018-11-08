// Fill out your copyright notice in the Description page of Project Settings.

#include "Shotgun.h"

AShotgun::AShotgun()
{
	TotalAmmo = 32;
	ReloadTime = 2.f;
	ProjectileClass = nullptr;
	SpreadAngle = 5.f;
	ShootDistance = 10000.f;
	Recoil = 10.f;
	WeaponType = EWeaponType::WT_Main;
	Damage = 10.f;
	CurrentAmmo = 8;
	UseRate = 2.f;
}

void AShotgun::SpawnProjectile()
{
	for (int Index = 0; Index < ShotgunSpreadTraces; Index++)
	{
		Super::SpawnProjectile();
	}
}

void AShotgun::FireTrace()
{
	for (int Index = 0; Index < ShotgunSpreadTraces; Index++)
	{
		Super::FireTrace();
	}
}
