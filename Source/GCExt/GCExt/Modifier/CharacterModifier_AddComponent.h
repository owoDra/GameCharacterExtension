// Copyright (C) 2023 owoDra

#pragma once

#include "CharacterModifier.h"
#include "CharacterModifierInstance.h"

#include "CharacterModifier_AddComponent.generated.h"

class UActorComponent;


/**
 * Modifier Instance class to add components to Pawn
 */
UCLASS()
class GCEXT_API UCharacterModifierInstance_AddComponent : public UCharacterModifierInstance
{
	GENERATED_BODY()

public:
	UCharacterModifierInstance_AddComponent() {}

protected:
	TObjectPtr<UActorComponent> DynamicComponent;

public:
	virtual void OnApply(const UCharacterModifier* Data, APawn* Pawn) override;
	virtual void OnRemoval(APawn* Pawn) override;

protected:
	//
	// Used to set initial values for created components
	//
	virtual void SetupComponent(UActorComponent* Compoenent, const UCharacterModifier* Data, APawn* Pawn) {}

	//
	// Used to deinitialize values for created components
	//
	virtual void FinalizeComponent(UActorComponent* Compoenent, APawn* Pawn) {}

};


/**
 * Base Modifier class to add components to Pawn
 */
UCLASS(Abstract)
class GCEXT_API UCharacterModifier_AddComponentBase : public UCharacterModifier
{
	GENERATED_BODY()

	friend class UCharacterModifierInstance_AddComponent;

public:
	UCharacterModifier_AddComponentBase() {}

protected:
	UPROPERTY(EditDefaultsOnly, Category = "ComponentToAdd")
	bool bAddToClient{ true };

	UPROPERTY(EditDefaultsOnly, Category = "ComponentToAdd")
	bool bAddToServer{ true };

protected:
	virtual UClass* GetInstanceClass() const override;

	virtual UClass* GetComponentClass() const PURE_VIRTUAL(, return nullptr;);

};


/**
 * Modifier class to add components to Pawn
 */
UCLASS(meta = (DisplayName = "CM Add Component"))
class UCharacterModifier_AddComponent final : public UCharacterModifier_AddComponentBase
{
	GENERATED_BODY()

	friend class UCharacterModifierInstance_AddComponent;

public:
	UCharacterModifier_AddComponent() {}

protected:
	UPROPERTY(EditDefaultsOnly, Category = "ComponentToAdd")
	TSoftClassPtr<UActorComponent> ComponentClass{ nullptr };

protected:
	virtual UClass* GetComponentClass() const override;

};
