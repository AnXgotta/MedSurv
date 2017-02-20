// Fill out your copyright notice in the Description page of Project Settings.

#include "Surv.h"
#include "SurvItemInteractable.h"



ASurvItemInteractable::ASurvItemInteractable() {
	
	Mesh->SetCollisionProfileName(TEXT("InteractableItem"));

	
}


//######################### INTERACTION

void ASurvItemInteractable::OnItemTracedBegin() {
	IsItemTraced = true;
}

void ASurvItemInteractable::OnItemTracedEnd() {
	IsItemTraced = false;
}

void ASurvItemInteractable::OnItemCarryBegin() {
	OnItemTracedEnd();
}

void ASurvItemInteractable::OnItemCarryEnd() {

}