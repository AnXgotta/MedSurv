// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SurvItemMaster.h"
#include "SurvItemInteractable.generated.h"

/**
 * 
 */
UCLASS()
class SURV_API ASurvItemInteractable : public ASurvItemMaster
{
	GENERATED_BODY()
	
	
public:
	// Sets default values for this actor's properties
	ASurvItemInteractable();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemState")
		bool IsItemTraced;

	UFUNCTION()
		void OnItemTracedBegin();

	UFUNCTION()
		void OnItemTracedEnd();

	UFUNCTION()
		void OnItemCarryBegin();

	UFUNCTION()
		void OnItemCarryEnd();


};
