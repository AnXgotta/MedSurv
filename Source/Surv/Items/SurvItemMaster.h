// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SurvTypes.h"
#include "SurvItemMaster.generated.h"

UCLASS()
class SURV_API ASurvItemMaster : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASurvItemMaster();

	// actionable as a placed item in the world
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
		FItemData ItemData;

	UFUNCTION()
		void OnAddedToInventory();

	UFUNCTION()
		void Initialize(FItemData NewItemData);
	
};
