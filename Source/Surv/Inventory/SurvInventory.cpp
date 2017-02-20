// Fill out your copyright notice in the Description page of Project Settings.

#include "Surv.h"
#include "SurvInventory.h"

ASurvInventory::ASurvInventory() {
	bNetLoadOnClient = false;
	bReplicates = true;
	bReplicateMovement = false;
}


void ASurvInventory::Initialize(int32 InventorySlots, AActor* Parent) {
	MaxInventorySlots = InventorySlots;
	this->SetOwner(Parent);
	this->AttachToComponent(Parent->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), NAME_None);
}

bool ASurvInventory::AddItem(ASurvItemInteractable* NewItem) {
	// if is full, return false;
	FItemData newItemInfo = FItemData(NewItem->ItemData);
	Inventory.Push(newItemInfo);
	return true;
}

bool ASurvInventory::RemoveItem(ASurvItemInteractable* NewItem) {
	//do nothing right now
	return true;
}


void ASurvInventory::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASurvInventory, MaxInventorySlots);
	DOREPLIFETIME(ASurvInventory, Inventory);

}