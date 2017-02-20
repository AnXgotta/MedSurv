// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "UnrealNetwork.h"
#include "SurvTypes.h"
#include "SurvCharacter.generated.h"

UCLASS(config=Game)
class ASurvCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Interaction Raycast Origin */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* InteractionRaycastOrigin;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* CarryAttachPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
		class UPhysicsHandleComponent* PhysicsHandle;

public:
	ASurvCharacter(const class FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaSeconds) override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction)
		float ItemInteractionRange = 300.0f;

	FVector CarryAttachPointDefault = FVector(750.0f, 0.0f, -50.0f);

private:

	UCapsuleComponent* capsuleComponent;

	
	FVector ItemInteractionTraceEnd;
	float LongInteractionLength = 1.0f;
	float CurrentInteractionTime = 0.0f;

protected:

	// ################################## PLAYER STATUS

	// Character Health/Status management
	UPROPERTY(ReplicatedUsing = OnRep_PlayerStatusChanged)
		struct FCharacterStatus CharacterStatus;

	UFUNCTION(BlueprintCallable, Category = "PlayerStatus")
		void OnRep_PlayerStatusChanged();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "PlayerStatus")
		void EventPlayerStatusChanged();

	void UpdatePlayerAir(float DeltaTime);

	//##################################### DEFAULTS

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	void Turn(float Rate);
	void LookUp(float Rate);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	//################# MOVEMENT
	
		float CarryingSpeedModifier = 0.25f;
		float IsInventoryOpenSpeedModifier = 0.0f;
		float SprintingSpeedModifier = 1.5f;
		
		UPROPERTY(Replicated)
		bool bIsSprinting = false;
		
		void OnSprintPress();
		void OnSprintRelease();

		UFUNCTION(Server, Reliable, WithValidation)
			void Server_OnSprintAction(bool bSprintPress);

	//################# INVENTORY

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_InventoryDataChanged)
		TArray<struct FItemData> Inventory;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void OnRep_InventoryDataChanged();

	UPROPERTY(BlueprintReadWrite)
		int32 InventoryMaxSize = 12;

	UPROPERTY(Replicated)
		bool bIsInventoryOpen = false;

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_SetInventoryOpen(bool IsInventoryOpen);

	void OnInventoryAction();

	void AddItemToInventory(ASurvItemInteractable* NewItem);

public:

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RemoveItemFromPosition(int32 ItemPosition);

protected:

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_RemoveItemFromPosition(int32 ItemPosition);

	int32 GetFirstOpenInventorySlot();

	bool IsInventoryFull();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void OpenInventoryUI();
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void CloseInventoryUI();

	UFUNCTION(Client, Reliable, Category = "Inventory")
		void Client_OnInventoryFull();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Inventory")
		void EventInventoryChanged();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Inventory")
		void EventInventoryDataChanged();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Inventory")
		void EventInventoryOpenedClosed(bool Open);

	//################# INTERACTION

	
	class ASurvItemInteractable* CurrentlyTracedItem;

	void SetTracedItem(ASurvItemInteractable* newItem);
	void ClearTracedItem();

	void TraceForInteraction();

	void OnInteractPress();
	void OnInteractRelease();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_OnInteractPress();
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_OnInteractRelease();

	ASurvItemInteractable* CurrentlyCarriedObject;

	void OnRep_CurrentlyCarriedObject();

	UPROPERTY(Replicated)
	bool bIsCarrying = false;

	void OnHandleCarryPress();

	void OnCarryPress();
	void OnCarryRelease();

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_OnCarryPress();

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_OnCarryRelease();

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_OnCarryPress(ASurvItemInteractable* CarryItem);

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_OnCarryRelease();

	void OnCarryingObject();

	UFUNCTION(Client, Reliable, Category = "Inventory")
		void Client_OnItemNotCarryable();


	void OnSpawnInventoryItem();

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_OnSpawnInventoryItem();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable, Category = PlayerStatus)
		FORCEINLINE struct FCharacterStatus GetCharacterStatus() const { return CharacterStatus; }

	UFUNCTION(BlueprintCallable, Category = Inventory)
		FORCEINLINE TArray<FItemData> GetInventory() const { return Inventory; }

	UFUNCTION(BlueprintCallable, Category = Movement)
		FORCEINLINE bool GetIsCarrying() { return bIsCarrying; }

	UFUNCTION(BlueprintCallable, Category = Movement)
		FORCEINLINE float GetCarryingSpeedModifier() { return CarryingSpeedModifier; }

	UFUNCTION(BlueprintCallable, Category = Movement)
		FORCEINLINE bool GetIsSprinting() { return bIsSprinting; }

	UFUNCTION(BlueprintCallable, Category = Movement)
		FORCEINLINE float GetSprintingSpeedModifier() { return SprintingSpeedModifier; }

	UFUNCTION(BlueprintCallable, Category = Movement)
		FORCEINLINE bool GetIsInventoryOpen() { return bIsInventoryOpen; }

	UFUNCTION(BlueprintCallable, Category = Movement)
		FORCEINLINE float GetInventoryOpenSpeedModifier() { return IsInventoryOpenSpeedModifier; }


	void ASurvCharacter::PrintString(UObject* WorldContextObject, const FString& InString, FColor color);

};

