// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SurvItemInteractable.h"
#include "SurvTypes.h"
#include "UnrealNetwork.h"
#include "SurvInventory.generated.h"

/**
 * 
 */
UCLASS()
class SURV_API ASurvInventory : public AActor
{
	GENERATED_BODY()

public:
	ASurvInventory();

private:
	UPROPERTY(Replicated)
	int32 MaxInventorySlots;
	UPROPERTY(Replicated)
	TArray<FItemData> Inventory;


public:

	void Initialize(int32 InventorySlots, AActor* Parent);


	bool AddItem(ASurvItemInteractable* NewItem);
	bool RemoveItem(ASurvItemInteractable* NewItem);
	


	FORCEINLINE int32 GetMaxInventorySlots() { return MaxInventorySlots; }

	FORCEINLINE TArray<FItemData> GetInventory() { return Inventory; }
};
