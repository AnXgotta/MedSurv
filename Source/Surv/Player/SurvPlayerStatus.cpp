
// Fill out your copyright notice in the Description page of Project Settings.

#include "Surv.h"
#include "SurvPlayerStatus.h"


// Sets default values
ASurvPlayerStatus::ASurvPlayerStatus()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASurvPlayerStatus::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASurvPlayerStatus::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}
