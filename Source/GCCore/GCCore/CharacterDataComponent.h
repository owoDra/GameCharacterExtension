// Copyright (C) 2023 owoDra

#pragma once

#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"

#include "CharacterModifierContainer.h"

#include "Delegates/Delegate.h"

#include "CharacterDataComponent.generated.h"

class UCharacterData;


/**
 * Components that manage Pawn or Character data
 * 
 * Tips:
 *	In general, the data handled by the CharacterDataComponent is specific to that character,
 *	and when the CharacterData is changed, the old data is discarded.
 *  Therefore, in games with multiple characters, 
 *	data shared among characters should not be managed by Pawn or Character and its Component,
 *	but by PlayerState, etc.
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class GCCORE_API UCharacterDataComponent
	: public UPawnComponent
	, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
public:
	UCharacterDataComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//
	// Function name used to add this component
	//
	static const FName NAME_ActorFeatureName;


protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;

public:
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;

protected:
	//
	// Data used to spawn Character operated by players or AI
	//
	UPROPERTY(ReplicatedUsing = "OnRep_CharacterData")
	TObjectPtr<const UCharacterData> CharacterData;

	//
	// List of CharacterModifierInstance currently applied to Pawn or Character
	//
	UPROPERTY(Replicated)
	FCharacterModifierContainer ModifierContainer;

protected:
	UFUNCTION()
	virtual void OnRep_CharacterData();

	/**
	 * Set the current character data
	 */
	virtual void UpdateCharacterData();

public:
	/**
	 * Set the current character data
	 */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable)
	void SetCharacterData(const UCharacterData* NewCharacterData);


protected:
	//
	// Delegate notifying that CharacterData has been applied and the initialization process has been completed.
	//
	FSimpleMulticastDelegate OnCharacterDataInitializedDelegate;

public:
	/**
	 * Register a callback to inform completion of initialization of CharacterData
	 */
	void OnCharacterDataInitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);

};
