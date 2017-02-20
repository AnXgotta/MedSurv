// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SpringArmComponent.h"
#include "SurvObjectCarryComponent.generated.h"

/**
 * 
 */
UCLASS()
class SURV_API USurvObjectCarryComponent : public USpringArmComponent
{
	GENERATED_BODY()
	
		USurvObjectCarryComponent();

private:

	AActor* GetActorInView();

	AActor* GetCarriedActor();

	/* Return the StaticMeshComponent of the carried Actor */
	UStaticMeshComponent* GetCarriedMeshComp();

protected:


	void TickComponent(float DeltaSeconds, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	/* Rotation speed */
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
		float RotateSpeed;

	void RotateActorAroundPoint(AActor* RotateActor, FVector RotationPoint, FRotator AddRotation);

public:

	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
		float MaxPickupDistance;

	/* Attach the current view focus object that is allowed to be picked up to the spring arm */
	void Pickup();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerPickup();

	UFUNCTION(Reliable, NetMulticast)
		void OnPickupMulticast(AActor* FocusActor);

	/* Release the currently attached object, if not blocked by environment */
	void Drop();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerDrop();

	UFUNCTION(Reliable, NetMulticast)
		void OnDropMulticast();

	/* Throw any currently carried Actor in the current view direction */
	void Throw();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerThrow();

	/* Is currently holding an Actor */
	bool GetIsCarryingActor();

	/* Rotate the carried Actor */
	void Rotate(float DirectionYaw, float DirectionRoll);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerRotate(float DirectionYaw, float DirectionRoll);

	UFUNCTION(Reliable, NetMulticast)
		void OnRotateMulticast(float DirectionYaw, float DirectionRoll);
	
	void PrintString(UObject* WorldContextObject, const FString& InString, FColor color);

};
