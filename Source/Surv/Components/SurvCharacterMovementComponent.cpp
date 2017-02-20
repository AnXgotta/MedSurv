
// Fill out your copyright notice in the Description page of Project Settings.

#include "Surv.h"
#include "SurvCharacter.h"
#include "SurvCharacterMovementComponent.h"


float USurvCharacterMovementComponent::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	ASurvCharacter* SurvCharacterOwner = Cast<ASurvCharacter>(PawnOwner);
	if (SurvCharacterOwner)
	{
		if (SurvCharacterOwner->GetIsCarrying())
		{
			MaxSpeed *= SurvCharacterOwner->GetCarryingSpeedModifier();
		}
		if (SurvCharacterOwner->GetIsInventoryOpen())
		{
			MaxSpeed *= SurvCharacterOwner->GetInventoryOpenSpeedModifier();
		}
		if (SurvCharacterOwner->GetIsSprinting()) {
			MaxSpeed *= SurvCharacterOwner->GetSprintingSpeedModifier();
		}
		
	}


	return MaxSpeed;
}
