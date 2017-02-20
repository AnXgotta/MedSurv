
// Fill out your copyright notice in the Description page of Project Settings.

#include "Surv.h"
#include "Engine.h"
#include "SurvObjectCarryComponent.h"


USurvObjectCarryComponent::USurvObjectCarryComponent()
{
	MaxPickupDistance = 600;
	RotateSpeed = 10.0f;

	bUsePawnControlRotation = true;
	bDoCollisionTest = false;

	SetIsReplicated(true);
}


void USurvObjectCarryComponent::TickComponent(float DeltaSeconds, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	if (APawn* OwningPawn = Cast<APawn>(GetOwner()))
	{
		if (OwningPawn->IsLocallyControlled())
		{
			Super::TickComponent(DeltaSeconds, TickType, ThisTickFunction);
		}
		else
		{
			/* NOTE: Slightly changed code from base implementation (USpringArmComponent) to use RemoteViewPitch instead of non-replicated ControlRotation */
			if (bUsePawnControlRotation)
			{
				{
					/* Re-map uint8 to 360 degrees */
					const float PawnViewPitch = (OwningPawn->RemoteViewPitch / 255.f)*360.f;
					if (PawnViewPitch != GetComponentRotation().Pitch)
					{
						FRotator NewRotation = GetComponentRotation();
						NewRotation.Pitch = PawnViewPitch;
						SetWorldRotation(NewRotation);
					}
				}
			}

			UpdateDesiredArmLocation(bDoCollisionTest, bEnableCameraLag, bEnableCameraRotationLag, DeltaSeconds);
		}
	}
}


void USurvObjectCarryComponent::Pickup()
{
	/* Drop if we are already carrying an Actor */
	if (GetIsCarryingActor())
	{
		Drop();
		return;
	}

	if (GetOwner()->Role < ROLE_Authority)
	{
		ServerPickup();
		return;
	}

	AActor* FocusActor = GetActorInView();
	OnPickupMulticast(FocusActor);
}


void USurvObjectCarryComponent::Drop()
{
	if (GetOwner()->Role < ROLE_Authority)
	{
		ServerDrop();
	}

	OnDropMulticast();
}


AActor* USurvObjectCarryComponent::GetActorInView()
{
	PrintString(this, "GAIV 0", FColor::Orange);
	APawn* PawnOwner = Cast<APawn>(GetOwner());
	AController* Controller = PawnOwner->Controller;
	if (Controller == nullptr)
	{
		return nullptr;
	}
	PrintString(this, "GAIV 1", FColor::Orange);
	FVector CamLoc;
	FRotator CamRot;
	Controller->GetPlayerViewPoint(CamLoc, CamRot);

	const FVector TraceStart = CamLoc;
	const FVector Direction = CamRot.Vector();
	const FVector TraceEnd = TraceStart + (Direction * MaxPickupDistance);

	FCollisionQueryParams TraceParams(TEXT("TraceActor"), true, PawnOwner);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bTraceComplex = false;
	const FName TraceTag("Pickup");
	GetWorld()->DebugDrawTraceTag = TraceTag;
	TraceParams.TraceTag = TraceTag;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_GameTraceChannel1, TraceParams);

	/* Check to see if we hit a staticmesh component that has physics simulation enabled */
	UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(Hit.GetComponent());
	if (MeshComp && MeshComp->IsSimulatingPhysics())
	{
		PrintString(this, "GAIV 2", FColor::Orange);
		return Hit.GetActor();
	}
	PrintString(this, "GAIV 3", FColor::Orange);
	return nullptr;
}


AActor* USurvObjectCarryComponent::GetCarriedActor()
{
	USceneComponent* ChildComp = GetChildComponent(0);
	if (ChildComp)
	{
		return ChildComp->GetOwner();
	}

	return nullptr;
}


UStaticMeshComponent* USurvObjectCarryComponent::GetCarriedMeshComp()
{
	USceneComponent* ChildComp = GetChildComponent(0);
	if (ChildComp)
	{
		AActor* OwningActor = ChildComp->GetOwner();
		if (OwningActor)
		{
			return Cast<UStaticMeshComponent>(OwningActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		}
	}

	return nullptr;
}


void USurvObjectCarryComponent::Throw()
{
	if (!GetIsCarryingActor())
		return;

	if (GetOwner()->Role < ROLE_Authority)
	{
		ServerThrow();
		return;
	}

	/* Grab a reference to the MeshComp before dropping the object */
	UStaticMeshComponent* MeshComp = GetCarriedMeshComp();
	if (MeshComp)
	{
		/* Detach and re-enable collision */
		OnDropMulticast();

		APawn* OwningPawn = Cast<APawn>(GetOwner());
		if (OwningPawn)
		{
			/* Re-map uint8 to 360 degrees */
			const float PawnViewPitch = (OwningPawn->RemoteViewPitch / 255.f)*360.f;

			FRotator NewRotation = GetComponentRotation();
			NewRotation.Pitch = PawnViewPitch;

			/* Apply physics impulse, ignores mass */
			MeshComp->AddImpulse(NewRotation.Vector() * 1000, NAME_None, true);
		}
	}
}


bool USurvObjectCarryComponent::GetIsCarryingActor()
{
	return GetChildComponent(0) != nullptr;
}


void USurvObjectCarryComponent::Rotate(float DirectionYaw, float DirectionRoll)
{
	if (GetOwner()->Role < ROLE_Authority)
	{
		ServerRotate(DirectionYaw, DirectionRoll);
	}

	OnRotateMulticast(DirectionYaw, DirectionRoll);
}


void USurvObjectCarryComponent::RotateActorAroundPoint(AActor* RotateActor, FVector RotationPoint, FRotator AddRotation)
{
	FVector Loc = RotateActor->GetActorLocation() - RotationPoint;
	FVector RotatedLoc = AddRotation.RotateVector(Loc);

	FVector NewLoc = RotationPoint + RotatedLoc;

	/* Compose the rotators, use Quats to avoid gimbal lock */
	FQuat AQuat = FQuat(RotateActor->GetActorRotation());
	FQuat BQuat = FQuat(AddRotation);

	FRotator NewRot = FRotator(BQuat*AQuat);

	RotateActor->SetActorLocationAndRotation(NewLoc, NewRot);
}


void USurvObjectCarryComponent::OnPickupMulticast_Implementation(AActor* FocusActor)
{
	PrintString(this, "OPMC 0", FColor::Purple);
	if (FocusActor && FocusActor->IsRootComponentMovable())
	{
		PrintString(this, "OPMC 2", FColor::Purple);
		/* Find the static mesh (if any) to disable physics simulation while carried
		Filter by objects that are physically simulated and can therefor be picked up */
		UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(FocusActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (MeshComp && MeshComp->IsSimulatingPhysics())
		{
			PrintString(this, "OPMC 3", FColor::Purple);
			MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			MeshComp->SetSimulatePhysics(false);
			MeshComp->SetEnableGravity(false);
		}
		PrintString(this, "OPMC 4", FColor::Purple);
		
		FocusActor->AttachToComponent(this, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), USpringArmComponent::SocketName);
	}
}


void USurvObjectCarryComponent::OnDropMulticast_Implementation()
{
	AActor* CarriedActor = GetCarriedActor();
	if (CarriedActor)
	{
		/* Find the static mesh (if any) to re-enable physics simulation */
		UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(CarriedActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (MeshComp)
		{
			MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			MeshComp->SetSimulatePhysics(true);
			MeshComp->SetEnableGravity(true);
		}

		CarriedActor->GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}
}


void USurvObjectCarryComponent::OnRotateMulticast_Implementation(float DirectionYaw, float DirectionRoll)
{
	AActor* CarriedActor = GetCarriedActor();
	if (CarriedActor)
	{
		/* Retrieve the object center */
		FVector RootOrigin = GetCarriedActor()->GetRootComponent()->Bounds.Origin;
		FRotator DeltaRot = FRotator(0, DirectionYaw * RotateSpeed, DirectionRoll * RotateSpeed);

		RotateActorAroundPoint(CarriedActor, RootOrigin, DeltaRot);
	}
}


void USurvObjectCarryComponent::ServerDrop_Implementation()
{
	Drop();
}


bool USurvObjectCarryComponent::ServerDrop_Validate()
{
	return true;
}


void USurvObjectCarryComponent::ServerThrow_Implementation()
{
	Throw();
}


bool USurvObjectCarryComponent::ServerThrow_Validate()
{
	return true;
}


void USurvObjectCarryComponent::ServerPickup_Implementation()
{
	Pickup();
}


bool USurvObjectCarryComponent::ServerPickup_Validate()
{
	return true;
}


void USurvObjectCarryComponent::ServerRotate_Implementation(float DirectionYaw, float DirectionRoll)
{
	Rotate(DirectionYaw, DirectionRoll);
}


bool USurvObjectCarryComponent::ServerRotate_Validate(float DirectionYaw, float DirectionRoll)
{
	return true;
}

//########################## debug

void USurvObjectCarryComponent::PrintString(UObject* WorldContextObject, const FString& InString, FColor color)
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST) // Do not Print in Shipping or Test

	WorldContextObject = GEngine->GetWorldFromContextObject(WorldContextObject, false);
	FString Prefix;
	if (WorldContextObject)
	{
		UWorld *World = WorldContextObject->GetWorld();
		if (World->WorldType == EWorldType::PIE)
		{
			switch (World->GetNetMode())
			{
			case NM_Client:
				Prefix = FString::Printf(TEXT("Client %d: "), GPlayInEditorID - 1);
				break;
			case NM_DedicatedServer:
			case NM_ListenServer:
				Prefix = FString::Printf(TEXT("Server: "));
				break;
			}
		}
	}

	const FString FinalString = Prefix + InString;

	// Also output to the screen, if possible

	GEngine->AddOnScreenDebugMessage((uint64)-1, 7.0, color, FinalString);


#endif
}