// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Surv.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "Engine.h"
#include "SurvCharacterMovementComponent.h"
#include "SurvItemInteractable.h"
#include "SurvCharacter.h"

//////////////////////////////////////////////////////////////////////////
// ASurvCharacter

ASurvCharacter::ASurvCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USurvCharacterMovementComponent>(ASurvCharacter::CharacterMovementComponentName))
{
	
	// Set size for collision capsule
	capsuleComponent = GetCapsuleComponent();
	capsuleComponent->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->GravityScale = 1.5f;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	


	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent); // The camera follows at this distance behind the character	

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	InteractionRaycastOrigin = CreateDefaultSubobject<USceneComponent>(TEXT("IternationRaycastOrigin"));
	InteractionRaycastOrigin->SetupAttachment(RootComponent);


	CarryAttachPoint = CreateDefaultSubobject<USceneComponent>(TEXT("CarryAttachPoint"));
	CarryAttachPoint->SetupAttachment(CameraBoom);
	
	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	PhysicsHandle->LinearDamping = 5000.0f;
	PhysicsHandle->LinearStiffness = 10000.0f;
	PhysicsHandle->AngularDamping = 5000.0f;
	PhysicsHandle->AngularStiffness = 10000.0f;

	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void ASurvCharacter::PostInitializeComponents() {
	Super::PostInitializeComponents();

	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	CameraBoom->TargetOffset = FVector(0.0f, 0.0f, 50.0f);

	InteractionRaycastOrigin->SetRelativeLocation(FVector(0.0f, 0.0f, capsuleComponent->GetScaledCapsuleHalfHeight()));
	InteractionRaycastOrigin->bVisible = false;
	
	CarryAttachPoint->SetRelativeLocation(CarryAttachPointDefault);

	if (Role = ROLE_Authority) {
		Inventory.Init(FItemData(), InventoryMaxSize);
	}

	bReplicates = true;
	bReplicateMovement = true;

}


//////////////////////////////////////////////////////////////////////////
// Input

void ASurvCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASurvCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASurvCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &ASurvCharacter::Turn);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASurvCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &ASurvCharacter::LookUp);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASurvCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ASurvCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ASurvCharacter::TouchStopped);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ASurvCharacter::OnInteractPress);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &ASurvCharacter::OnInteractRelease);

	PlayerInputComponent->BindAction("Carry", IE_Pressed, this, &ASurvCharacter::OnHandleCarryPress);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASurvCharacter::OnSprintPress);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASurvCharacter::OnSprintRelease);

	PlayerInputComponent->BindAction("Inventory", IE_Pressed, this, &ASurvCharacter::OnInventoryAction);


}


void ASurvCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ASurvCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ASurvCharacter::Turn(float Rate)
{
	if (bIsInventoryOpen) {
		return;
	}
	APawn::AddControllerYawInput(Rate);
}

void ASurvCharacter::LookUp(float Rate)
{
	if (bIsInventoryOpen) {
		return;
	}
	APawn::AddControllerPitchInput(Rate);
}

void ASurvCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASurvCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ASurvCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		if (capsuleComponent) {
			// get forward vector
			const FVector Direction = capsuleComponent->GetForwardVector();
			AddMovementInput(Direction, Value);
		}
	}
}

void ASurvCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		if (capsuleComponent) {
			// get right vector
			const FVector Direction = capsuleComponent->GetRightVector();
			AddMovementInput(Direction, Value);
		}
	}
}


void ASurvCharacter::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	TraceForInteraction();
	OnCarryingObject();
	UpdatePlayerAir(DeltaSeconds);
}

//############################# PLAYER STATUS

void ASurvCharacter::OnRep_PlayerStatusChanged() {
	EventPlayerStatusChanged();
}


void ASurvCharacter::UpdatePlayerAir(float DeltaTime) {
	if (Role < ROLE_Authority) {
		return;
	}
	
	float reduceAirValue = 0.0f;
	
	if (bIsSprinting) {
		reduceAirValue = DeltaTime * CharacterStatus.SprintingAirLossPerSecond;
	}
	else {
		reduceAirValue = DeltaTime * CharacterStatus.AirLossPerSecond;
	}
	

	CharacterStatus.Air = FMath::Max(0.0f, CharacterStatus.Air - reduceAirValue);

	if (CharacterStatus.Air < 25.0f) {
		// do things
	}
	else if (CharacterStatus.Air < 15.0f) {
		// do things
	}
	else if (CharacterStatus.Air <= 0.0f) {
		// do things for dead
	}

}

//############################# MOVEMENT

void ASurvCharacter::OnSprintPress() {
	bIsSprinting = true;
	if (Role < ROLE_Authority) {
		Server_OnSprintAction(true);
		return;
	}
}
void ASurvCharacter::OnSprintRelease(){
	bIsSprinting = false;
	if (Role < ROLE_Authority) {
		Server_OnSprintAction(false);
		return;
	}
}

bool ASurvCharacter::Server_OnSprintAction_Validate(bool bSprintPress) {
	return true;
}

void ASurvCharacter::Server_OnSprintAction_Implementation(bool bSprintPress) {
	if (bSprintPress) {
		OnSprintPress();
	} else {
		OnSprintRelease();
	}
}

//############################# INTERACTION

void ASurvCharacter::TraceForInteraction() {

	if (!IsLocallyControlled()) {
		return;
	}

	UWorld* world = GetWorld();

	if (!world || !capsuleComponent || !FollowCamera) {
		return;
	}

	if (bIsCarrying) {
		if (CurrentlyTracedItem) {
			CurrentlyTracedItem->OnItemTracedEnd();
			CurrentlyTracedItem = NULL;
		}
		return;
	}


	FCollisionQueryParams traceParams(FName(TEXT("Item Interaction Trace")), true, this);
	const FName TraceTag("InteracTraceTag");
	//world->DebugDrawTraceTag = TraceTag;
	traceParams.TraceTag = TraceTag;

	FHitResult hitResult = FHitResult(ForceInit);

	FVector originLocation = InteractionRaycastOrigin->GetComponentLocation();

	// trace for interactable items
	if (GetWorld()->LineTraceSingleByChannel(hitResult, originLocation, originLocation + (FollowCamera->GetForwardVector().GetSafeNormal() * ItemInteractionRange), ECC_GameTraceChannel1, traceParams)) {
		if (hitResult.GetActor()->IsA(ASurvItemInteractable::StaticClass())) {
			if (CurrentlyTracedItem) {
				ASurvItemInteractable* item = Cast<ASurvItemInteractable>(hitResult.GetActor());
				if (CurrentlyTracedItem == item) {
					return;
				}
				else {
					ClearTracedItem();
					SetTracedItem(Cast<ASurvItemInteractable>(hitResult.GetActor()));
				}
			}
			SetTracedItem(Cast<ASurvItemInteractable>(hitResult.GetActor()));
		} else {
			ClearTracedItem();
		}
	}
	else {
		ClearTracedItem();
	}
}

void ASurvCharacter::SetTracedItem(ASurvItemInteractable* newItem) {
	if (!newItem) {
		return;
	}
	CurrentlyTracedItem = newItem;
	CurrentlyTracedItem->OnItemTracedBegin();
	PrintString(this, "ItemInteractionBegin", FColor::Blue);
}

void ASurvCharacter::ClearTracedItem() {
	if (!CurrentlyTracedItem) {
		return;
	}
	PrintString(this, "ItemInteractionEnd", FColor::Blue);
	CurrentlyTracedItem->OnItemTracedEnd();
	CurrentlyTracedItem = NULL;

}

void ASurvCharacter::OnInteractPress() {

	if (bIsCarrying) {
		return;
	}

	if (Role < ROLE_Authority) {
		Server_OnInteractPress();
		return;
	}

	CharacterStatus.Health -= 25.0f;
	CharacterStatus.Power -= 30.0f;
	FCollisionQueryParams traceParams(FName(TEXT("Item Interaction Trace")), true, this);
	const FName TraceTag("InteracTraceTag");
	//world->DebugDrawTraceTag = TraceTag;
	traceParams.TraceTag = TraceTag;

	FHitResult hitResult = FHitResult(ForceInit);

	FVector originLocation = InteractionRaycastOrigin->GetComponentLocation();

	// trace for interactable items
	if (GetWorld()->LineTraceSingleByChannel(hitResult, originLocation, originLocation + (FollowCamera->GetForwardVector().GetSafeNormal() * ItemInteractionRange), ECC_GameTraceChannel1, traceParams)) {
		PrintString(this, "ServerTrace", FColor::Red);
		if (hitResult.GetActor()->IsA(ASurvItemInteractable::StaticClass())) {
			AddItemToInventory(Cast<ASurvItemInteractable>(hitResult.GetActor()));			
		}
		else {
			PrintString(this, "PostTrace - Item Not IsA", FColor::Red);
		}
	}

	
	PrintString(this, "OnInteract", FColor::Blue);
}

void ASurvCharacter::OnInteractRelease() {
	Server_OnInteractRelease();
}

bool ASurvCharacter::Server_OnInteractPress_Validate() {
	return true;
}

void ASurvCharacter::Server_OnInteractPress_Implementation() {
	OnInteractPress();
}

bool ASurvCharacter::Server_OnInteractRelease_Validate() {
	return true;
}

void ASurvCharacter::Server_OnInteractRelease_Implementation() {

}

void ASurvCharacter::OnHandleCarryPress() {
	if (bIsCarrying) {
		PrintString(this, "CarryRelease", FColor::Purple);
		OnCarryRelease();
	}
	else {
		PrintString(this, "CarryPress", FColor::Purple);
		OnCarryPress();
	}
}

void ASurvCharacter::OnCarryPress() {
	if (Role < ROLE_Authority) {
		Server_OnCarryPress();
		return;
	}

	FCollisionQueryParams traceParams(FName(TEXT("Item Carry Trace")), true, this);
	const FName TraceTag("InteracTraceTag");
	//world->DebugDrawTraceTag = TraceTag;
	traceParams.TraceTag = TraceTag;

	FHitResult hitResult = FHitResult(ForceInit);

	FVector originLocation = InteractionRaycastOrigin->GetComponentLocation();

	// trace for caryyable items
	if (GetWorld()->LineTraceSingleByChannel(hitResult, originLocation, originLocation + (FollowCamera->GetForwardVector().GetSafeNormal() * ItemInteractionRange), ECC_GameTraceChannel1, traceParams)) {
		PrintString(this, "ServerCarryTrace", FColor::Red);
		if (hitResult.GetActor()->IsA(ASurvItemInteractable::StaticClass())) {
			ASurvItemInteractable* item = Cast<ASurvItemInteractable>(hitResult.GetActor());
			if (item->ItemData.bItemType.Contains(EItemType::IT_CARRYABLE)) {
				UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(item->GetComponentByClass(UStaticMeshComponent::StaticClass()));
				if (MeshComp->IsSimulatingPhysics())
				{
					// other carry conditions here
					// weight, etc.
					MeshComp->SetEnableGravity(false);
					bIsCarrying = true;
					PhysicsHandle->GrabComponentAtLocationWithRotation(MeshComp, NAME_None, CarryAttachPoint->GetComponentLocation(), CarryAttachPoint->GetComponentRotation());
					CurrentlyCarriedObject = item;
					CurrentlyCarriedObject->OnItemCarryBegin();
					Multicast_OnCarryPress(CurrentlyCarriedObject);
				}
			}
			else {
				Client_OnItemNotCarryable();
			}
		}
		else {
			PrintString(this, "PostTrace - Item Not IsA", FColor::Red);
		}
	}
	
}

void ASurvCharacter::Multicast_OnCarryPress_Implementation(ASurvItemInteractable* CarryItem) {
	CurrentlyCarriedObject = CarryItem;
	UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(CurrentlyCarriedObject->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (MeshComp) {
		MeshComp->SetEnableGravity(false);
	}
}

void ASurvCharacter::OnCarryRelease() {
	if (Role < ROLE_Authority) {
		Server_OnCarryRelease();
		return;
	}
	bIsCarrying = false;
	UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(CurrentlyCarriedObject->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (MeshComp)
	{
		MeshComp->SetEnableGravity(true);
		MeshComp->WakeAllRigidBodies();
	}
	CurrentlyCarriedObject->OnItemCarryEnd();
	PhysicsHandle->ReleaseComponent();
	CurrentlyCarriedObject = NULL;
	Multicast_OnCarryRelease();
}

void ASurvCharacter::Multicast_OnCarryRelease_Implementation() {
	if (!CurrentlyCarriedObject) {
		return;
	}
	UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(CurrentlyCarriedObject->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (MeshComp)
	{
		MeshComp->SetEnableGravity(true);
		MeshComp->WakeAllRigidBodies();
	}
	CurrentlyCarriedObject = NULL;
}

void ASurvCharacter::OnCarryingObject() {
	if (!bIsCarrying || Role < ROLE_Authority) {
		return;
	}
	PhysicsHandle->SetTargetLocationAndRotation(CarryAttachPoint->GetComponentLocation(), CarryAttachPoint->GetComponentRotation());
}

bool ASurvCharacter::Server_OnCarryPress_Validate() {
	return true;
}

void ASurvCharacter::Server_OnCarryPress_Implementation() {
	OnCarryPress();
}

bool ASurvCharacter::Server_OnCarryRelease_Validate() {
	return true;
}

void ASurvCharacter::Server_OnCarryRelease_Implementation() {
	OnCarryRelease();
}

void ASurvCharacter::Client_OnItemNotCarryable_Implementation() {
	PrintString(this, "Item Not Carryable", FColor::Blue);
}



void ASurvCharacter::OnSpawnInventoryItem() {
	if (Role < ROLE_Authority) {
		Server_OnSpawnInventoryItem();
		return;
	}

	PrintString(this, "SpawnItem Attmpt", FColor::Red);
	if (Inventory.Num() <= 0) {
		PrintString(this, "Unable to spawn item - inventory empty", FColor::Red);
		return;
	}
	FItemData ItemData = Inventory.Pop();
	ASurvItemInteractable* newItem = GetWorld()->SpawnActor<ASurvItemInteractable>(ItemData.bItemClass, InteractionRaycastOrigin->GetComponentLocation() + (FollowCamera->GetForwardVector().GetSafeNormal() * ItemInteractionRange), FRotator(0.0f, 0.0f, 0.0f));
	
	newItem->Initialize(ItemData);
	PrintString(this, (FollowCamera->GetForwardVector().GetSafeNormal() * ItemInteractionRange).ToString(), FColor::Red);

}

bool ASurvCharacter::Server_OnSpawnInventoryItem_Validate() {
	return true;
}

void ASurvCharacter::Server_OnSpawnInventoryItem_Implementation() {
	OnSpawnInventoryItem();
}


//######################################### INVENTORY

void ASurvCharacter::OnInventoryAction() {
	bIsInventoryOpen ? CloseInventoryUI() : OpenInventoryUI();
	
}

bool ASurvCharacter::Server_SetInventoryOpen_Validate(bool IsInventoryOpen) {
	return true;
}

void ASurvCharacter::Server_SetInventoryOpen_Implementation(bool IsInventoryOpen) {
	bIsInventoryOpen = IsInventoryOpen;
}

void ASurvCharacter::AddItemToInventory(ASurvItemInteractable* NewItem) {
	if (IsInventoryFull()) {
		Client_OnInventoryFull();
		return;
	}
	int32 OpenSlot = GetFirstOpenInventorySlot();
	if (OpenSlot < 0) {
		return;
	}
	NewItem->ItemData.bItemInventoryLocation = OpenSlot;
	PrintString(this, "OnAddedToInventory", FColor::Red);
	Inventory[OpenSlot] = FItemData(NewItem->ItemData);
	NewItem->OnAddedToInventory();
}



void ASurvCharacter::RemoveItemFromPosition(int32 ItemPosition) {

	if (Role < ROLE_Authority) {
		Server_RemoveItemFromPosition(ItemPosition);
		return;
	}

	FItemData ItemData(Inventory[ItemPosition]);
	Inventory[ItemPosition] = FItemData();
	ASurvItemInteractable* newItem = GetWorld()->SpawnActor<ASurvItemInteractable>(ItemData.bItemClass, InteractionRaycastOrigin->GetComponentLocation() + (FollowCamera->GetForwardVector().GetSafeNormal() * ItemInteractionRange), FRotator(0.0f, 0.0f, 0.0f));
	newItem->Initialize(ItemData);
}


bool ASurvCharacter::Server_RemoveItemFromPosition_Validate(int32 ItemPosition) {
	return true;
}

void ASurvCharacter::Server_RemoveItemFromPosition_Implementation(int32 ItemPosition) {
	RemoveItemFromPosition(ItemPosition);
}

int32 ASurvCharacter::GetFirstOpenInventorySlot() {
	for (int i = 0; i < Inventory.Num(); i++) {
		if (Inventory[i].bItemInventoryLocation == -1) {
			return i;
		}
	}
	return -1;
}

bool ASurvCharacter::IsInventoryFull() {
	for (int i = 0; i < Inventory.Num(); i++) {
		if (Inventory[i].bItemInventoryLocation == -1) {
			return false;
		}
	}
	return true;
}


void ASurvCharacter::OpenInventoryUI() {
	if (!IsLocallyControlled()) {
		return;
	}
	Server_SetInventoryOpen(true);
	EventInventoryOpenedClosed(true);
	bIsInventoryOpen = true;
	Cast<APlayerController>(Controller)->bShowMouseCursor = true;
	Cast<APlayerController>(Controller)->bEnableClickEvents = true;
	Cast<APlayerController>(Controller)->bEnableMouseOverEvents = true;
}

void ASurvCharacter::CloseInventoryUI() {
	if (!IsLocallyControlled()) {
		return;
	}
	Server_SetInventoryOpen(false);
	EventInventoryOpenedClosed(false);
	bIsInventoryOpen = false;
	Cast<APlayerController>(Controller)->bShowMouseCursor = false;
	Cast<APlayerController>(Controller)->bEnableClickEvents = false;
	Cast<APlayerController>(Controller)->bEnableMouseOverEvents = false;
}

void ASurvCharacter::OnRep_InventoryDataChanged() {
	EventInventoryDataChanged();
}

void ASurvCharacter::Client_OnInventoryFull_Implementation() {
	PrintString(this, "Unable to Add item - inventory full", FColor::Blue);
}

void ASurvCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const {

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASurvCharacter, CharacterStatus);
	DOREPLIFETIME(ASurvCharacter, Inventory);
	DOREPLIFETIME(ASurvCharacter, bIsCarrying);
	DOREPLIFETIME(ASurvCharacter, bIsInventoryOpen);
	DOREPLIFETIME(ASurvCharacter, bIsSprinting);
	
}



//########################## debug

void ASurvCharacter::PrintString(UObject* WorldContextObject, const FString& InString, FColor color)
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

	UE_LOG(LogBlueprintUserMessages, Log, TEXT("%s"), *FinalString);


#endif
}