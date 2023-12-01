// Copyright (C) 2023 owoDra

#pragma once

#include "GameFramework/Pawn.h"

#include "GamePawn.generated.h"

class UObject;

/** 
 * Minimal class that supports extension by game feature plugins 
 */
UCLASS(Blueprintable)
class GCCORE_API AGamePawn : public APawn
{
	GENERATED_BODY()
public:
	AGamePawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	//
	// Components to manage the initialization of basic Character data and components
	//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	TObjectPtr<UCharacterDataComponent> CharacterDataComponent;

protected:
	/**
	 * Callback function to inform completion of initialization of CharacterData
	 */
	virtual void OnCharacterDataInitialized()
	{
		BP_OnCharacterDataInitialized();
	}

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnCharacterDataInitialized();

public:
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};
