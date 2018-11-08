// Fill out your copyright notice in the Description page of Project Settings.

#include "CameraShakeDamage.h"

UCameraShakeDamage::UCameraShakeDamage()
{
	RotOscillation.Pitch.Amplitude = 1.f;
	RotOscillation.Pitch.Frequency = 10.f;

	RotOscillation.Yaw.Amplitude = 1.f;
	RotOscillation.Yaw.Frequency = 10.f;

	RotOscillation.Roll.Amplitude = 1.f;
	RotOscillation.Roll.Frequency = 10.f;

	FOVOscillation.Amplitude = 1.f;
	FOVOscillation.Frequency = 10.f;

	LocOscillation.X.Amplitude = 1.f;
	LocOscillation.X.Frequency = 10.f;

	OscillationDuration = 0.5f;
}
