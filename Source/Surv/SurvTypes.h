#pragma once

#include "SurvTypes.generated.h"

USTRUCT()
struct FCharacterStatus {

	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStatus")
		float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStatus")
		float Air;

	float AirLossPerSecond;
	float SprintingAirLossPerSecond;

	//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStatus")
	//		float Hunger;

	//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStatus")
	//		float Thirst;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStatus")
		float Power;

	//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStatus")
	//		float Temperature;

	//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStatus")
	//		float Poison;

	//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStatus")
	//		float SomethingElse;



	//Constructor
	FCharacterStatus() {
		Health = 100.0f;
		Power = 100.0f;
		Air = 100.0f;
		AirLossPerSecond = 1.0f;
		SprintingAirLossPerSecond = AirLossPerSecond + 0.5f;
	}

};


UENUM(BlueprintType)
enum class EItemType : uint8 {
	IT_INVENTORY UMETA(DisplayName="Inventory"),
	IT_CARRYABLE UMETA(DisplayName="Carryable"),
	IT_CONSUMABLE UMETA(DisplayName="Consumable")
};

USTRUCT(BlueprintType)
struct FItemData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemStructure")
		UClass* bItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemStructure")
		UTexture2D* bItemImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemStructure")
		FString bItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemStructure")
		FText bItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemStructure")
		int32 bItemInventoryLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemStructure")
		TArray<EItemType> bItemType;


	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemStructure")
		//int32 bItemStackSize;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemStructure")
		//bool bHotbarPlaceable;

	//Constructor
	FItemData()
	{
		bItemClass = NULL;
		bItemImage = NULL;
		bItemName = NULL;
		bItemDescription = FText::GetEmpty();
		bItemInventoryLocation = -1;
	}
	//Constructor
	FItemData(FItemData* otherItem)
	{
		bItemImage = otherItem->bItemImage;
		bItemName = otherItem->bItemName;
		bItemDescription = otherItem->bItemDescription;
		bItemInventoryLocation = otherItem->bItemInventoryLocation;
		for (EItemType itemType : otherItem->bItemType) {
			bItemType.Add(itemType);
		}
	}

};

USTRUCT(BlueprintType)
struct FInventoryData
{
	GENERATED_USTRUCT_BODY()

public:


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		int32 TotalSlots;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
	//	TArray<FItemData> Inventory;


	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemStructure")
	//int32 bItemStackSize;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemStructure")
	//bool bHotbarPlaceable;

	//Constructor
	FInventoryData()
	{

	}

	//Constructor
	FInventoryData(FInventoryData* otherInventory)
	{
		TotalSlots = otherInventory->TotalSlots;
		//	for (FItemData item : otherItem->Inventory) {
		//		Inventory.Add(FItemData(item));
			//}
	}


};



UENUM(BlueprintType)
enum class EIventoryItemQuality : uint8 {
	POOR UMETA(DisplayName = "Poor"),
	COMMON UMETA(DisplayName = "Common"),
	UNCOMMON UMETA(DisplayName = "Uncommon"),
	RARE UMETA(DisplayName = "Rare"),
	EPIC UMETA(DisplayName = "Epic"),
	LEGENDARY UMETA(DisplayName = "Legendary")
};

UENUM(BlueprintType)
enum class EInventoryItemType : uint8 {
	EQUIPMENT UMETA(DisplayName = "Equipment"),
	CARRYABLE UMETA(DisplayName = "Carryable"),
	CONSUMABLE UMETA(DisplayName = "Consumable")
};

USTRUCT(BlueprintType)
struct FInventoryContainerInfo {

	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		uint8 SlotsPerRow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		bool IsStorageContainer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		uint8 StorageInventorySize;

	FInventoryContainerInfo() {

	}

};

USTRUCT(BlueprintType)
struct FItem {

	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		FName ItemID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		int32 Amount;

	FItem() {

	}

};

USTRUCT(BlueprintType)
struct FItemInformation {

	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		FName ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		UTexture2D* Icon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		EIventoryItemQuality Quality;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		EInventoryItemType ItemType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		int32 Amount;

	FItemInformation() {

	}

};

USTRUCT(BlueprintType)
struct FInventoryItem {

	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		FName ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		UTexture2D* Icon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		FString Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		EIventoryItemQuality Quality;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		EInventoryItemType ItemType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		int32 Amount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		bool IsStackable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		int32 MaxStackSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		bool IsDroppable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		UStaticMesh* WorldMesh;

	FInventoryItem() {

	}

	FInventoryItem(FInventoryItem* Other) {
		ID = Other->ID;
		Icon = Other->Icon;
		Name = Other->Name;
		Description = Other->Description;
		Quality = Other->Quality;
		ItemType = Other->ItemType;
		Amount = Other->Amount;
		IsStackable = Other->IsStackable;
		MaxStackSize = Other->MaxStackSize;
		IsDroppable = Other->IsDroppable;
		WorldMesh = Other->WorldMesh;
	}

	bool IsNull() {
		return ID.IsNone() || !ID.IsValid();
	}

};

USTRUCT(BlueprintType)
struct FLootList {

	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		FName ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		bool Unique;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		int32 MinAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		int32 MaxAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		float DropChance;

	FLootList() {

	}

};

USTRUCT(BlueprintType)
struct FToolTipInfo {

	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		UTexture2D* Icon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		FString Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		EIventoryItemQuality Quality;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		EInventoryItemType ItemType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		bool IsStackable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		int32 Amount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		int32 MaxStackSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		bool IsDroppable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryStructure")
		UStaticMesh* WorldMesh;

	FToolTipInfo() {

	}

};