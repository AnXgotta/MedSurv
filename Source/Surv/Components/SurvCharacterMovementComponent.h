
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "SurvCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class SURV_API USurvCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()


	
	
		virtual float GetMaxSpeed() const override;


public:

	UFUNCTION(BlueprintCallable, Category = "Movement")
		FORCEINLINE float GetFinalMaxSpeed() { return Super::GetMaxSpeed(); }
	
};
