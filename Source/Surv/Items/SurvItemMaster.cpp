// Fill out your copyright notice in the Description page of Project Settings.

#include "Surv.h"
#include "Engine.h"
#include "SurvItemMaster.h"


// Sets default values
ASurvItemMaster::ASurvItemMaster()
{
	Mesh = CreateOptionalDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->bCastDynamicShadow = true;
	Mesh->CastShadow = true;
	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(true);
	Mesh->SetupAttachment(RootComponent);
	bNetLoadOnClient = true;
	bReplicates = true;
	bReplicateMovement = true;
	//PrimaryActorTick.bCanEverTick = true;
}

void ASurvItemMaster::Initialize(FItemData NewItemData) {
	ItemData = FItemData(NewItemData);
}

// called from server
void ASurvItemMaster::OnAddedToInventory() {
	if (!IsPendingKill()) {
		Destroy();
	}
}
