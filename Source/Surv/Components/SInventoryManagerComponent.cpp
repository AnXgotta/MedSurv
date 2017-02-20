// Fill out your copyright notice in the Description page of Project Settings.

#include "Surv.h"
#include "SInventoryManagerComponent.h"


// Sets default values for this component's properties
USInventoryManagerComponent::USInventoryManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USInventoryManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USInventoryManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void USInventoryManagerComponent::Server_InitInventory_Implementation(int32 InventorySize) {
	Inventory.Empty();
	Inventory.Init(FInventoryItem(), InventorySize);
}

FInventoryItem USInventoryManagerComponent::GetInventoryItem_Implementation(int32 InventorySlot) {
	if(!Inventory.IsValidIndex(InventorySlot)) {
		return FInventoryItem();
	}
	return Inventory[InventorySlot];
}

bool USInventoryManagerComponent::SetInventoryItem_Implementation(int32 InventorySlot, FInventoryItem InventoryItem) {
	Inventory.Insert(InventoryItem, InventorySlot);
	return true;
}

void USInventoryManagerComponent::ClearInventoryItem_Implementation(int32 InventorySlot) {
	if (!Inventory.IsValidIndex(InventorySlot)) {
		return;
	}
	Inventory.Insert(FInventoryItem(), InventorySlot);
}

int32 USInventoryManagerComponent::GetEmptyInventorySpace_Implementation() {
	for (uint8 InventorySlotIndex = 0; InventorySlotIndex < Inventory.Num(); InventorySlotIndex++) {
		if (Inventory[InventorySlotIndex].IsNull()) {
			return InventorySlotIndex;
		}
	}
	return -1;
}

int32 USInventoryManagerComponent::GetInventoryItemCount_Implementation() {
	int32 EmptyInventorySpace = 0;
	for (int32 InventorySlotIndex = 0; InventorySlotIndex < Inventory.Num(); InventorySlotIndex++) {
		if (Inventory[InventorySlotIndex].IsNull()) {
			EmptyInventorySpace++;
		}
	}
	return EmptyInventorySpace;
}

void USInventoryManagerComponent::IncreaseInventorySize_Implementation(int32 Amount) {
	for (int32 Index = 0; Index < Amount; Index++) {
		Inventory.Add(FInventoryItem());
	}
}

void USInventoryManagerComponent::DecreaseInventorySize_Implementation(int32 Amount) {
	int32 NewInventoryMaxSize = (Inventory.Num() - 1) - Amount;
	if (NewInventoryMaxSize < 0) {
		return;
	}
	for (int32 Index = (Inventory.Num() - 1); Index > NewInventoryMaxSize; Index--) {
		Inventory.RemoveAt(Index);
	}
}

void USInventoryManagerComponent::LoadInventoryItems_Implementation(int32 InventorySize, const TArray<FInventoryItem>& InventoryItems) {
	Inventory.Empty();
	Inventory.Init(FInventoryItem(), InventorySize);
	for (int32 Index = 0; Index < InventoryItems.Num(); Index++) {
		Inventory.Insert(FInventoryItem(InventoryItems[Index]), Index);
	}
}

TArray<FInventoryItem> USInventoryManagerComponent::GetInventoryItems_Implementation() {
	return Inventory;
}









void USInventoryManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USInventoryManagerComponent, Inventory);

}
