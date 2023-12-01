// Copyright (C) 2023 owoDra

#include "GamePawn.h"

#include "CharacterDataComponent.h"

#include "Components/GameFrameworkComponentManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GamePawn)


AGamePawn::AGamePawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CharacterDataComponent = CreateDefaultSubobject<UCharacterDataComponent>(TEXT("CharacterDataComponent"));
	CharacterDataComponent->OnCharacterDataInitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnCharacterDataInitialized));
}


void AGamePawn::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AGamePawn::BeginPlay()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);

	Super::BeginPlay();
}

void AGamePawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}
