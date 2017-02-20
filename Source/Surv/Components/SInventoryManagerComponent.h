// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SurvTypes.h"
#include "UnrealNetwork.h"
#include "Components/ActorComponent.h"
#include "SInventoryManagerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SURV_API USInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()


private:

	UPROPERTY(Replicated)
		TArray<FInventoryItem> Inventory;

public:	
	// Sets default values for this component's properties
	USInventoryManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
		void Server_InitInventory(int32 InventorySize);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
		FInventoryItem GetInventoryItem(int32 InventorySlot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
		bool SetInventoryItem(int32 InventorySlot, FInventoryItem InventoryItem);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
		void ClearInventoryItem(int32 InventorySlot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
		int32 GetEmptyInventorySpace();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
		int32 GetInventoryItemCount();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
		void IncreaseInventorySize(int32 Amount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
		void DecreaseInventorySize(int32 Amount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
		void LoadInventoryItems(int32 InventorySize, const TArray<FInventoryItem>& InventoryItems);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
		TArray<FInventoryItem> GetInventoryItems();

};
