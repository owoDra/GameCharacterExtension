// Copyright (C) 2024 owoDra

#pragma once

#include "Recipe/CharacterRecipePolicyTypes.h"
#include "Recipe/ActiveCharacterRecipeHandle.h"

#include "CharacterRecipe.generated.h"

class APawn;
class UCharacterInitStateComponent;


/**
 * Information on the Pawn to which the CharacterRecipe is applied
 */
USTRUCT(BlueprintType)
struct FCharacterRecipePawnInfo
{
	GENERATED_BODY()
public:
	FCharacterRecipePawnInfo() {}
	FCharacterRecipePawnInfo(const FActiveCharacterRecipeHandle& InHandle, APawn* InPawn, UCharacterInitStateComponent* InInitStateComponent)
		: Handle(InHandle), Pawn(InPawn), InitStateComponent(InInitStateComponent)
	{}

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FActiveCharacterRecipeHandle Handle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TWeakObjectPtr<APawn> Pawn{ nullptr };

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TWeakObjectPtr<UCharacterInitStateComponent> InitStateComponent{ nullptr };

};


/**
 * Base class for building game characters
 * 
 * Tips:
 *	By deriving from this class, it is possible to set up the character's mesh, assign animations, 
 *	and implement other additional abilities and data processing.
 * 
 *	Character setup process starts with StartSetup() and ends with FinishSetup()
 */
UCLASS(Abstract, Blueprintable)
class GCEXT_API UCharacterRecipe : public UObject
{
	GENERATED_BODY()
public:
	UCharacterRecipe(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	//////////////////////////////////////////////////////////////////////////////////
	// Policies
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Policies")
	ECharacterRecipeInstancingPolicy InstancingPolicy{ ECharacterRecipeInstancingPolicy::Instanced };

	UPROPERTY(EditDefaultsOnly, Category = "Policies")
	ECharacterRecipeNetExecutionPolicy NetExecutionPolicy{ ECharacterRecipeNetExecutionPolicy::Both };

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Policies")
	ECharacterRecipeInstancingPolicy GetInstancingPolicy() const { return InstancingPolicy; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Policies")
	ECharacterRecipeNetExecutionPolicy GetNetExecutionPolicy() const { return NetExecutionPolicy; }


	//////////////////////////////////////////////////////////////////////////////////
	// Instanced
protected:
	//
	// Whether the processing of this CharacterRecipe is complete or not.
	//
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Info")
	FCharacterRecipePawnInfo PawnInfo;

public:
	/**
	 * Executed when all CharacterRecipes are added and setup begins.
	 */
	void HandleStartSetup(const FCharacterRecipePawnInfo& Info);

	/**
	 * Executed when the character destroys
	 */
	void HandleDestroy();

protected:
	/**
	 * Executed when all CharacterRecipes are added and setup begins.
	 * 
	 * Tips:
	 *	If the InstancingPolicy is "Instanced", this function will start the setup process
	 * 
	 * Note:
	 *	Always call FinishSetup at the end of processing to notify finish.
	 *	Also, if you have any binding to a delegate, etc., please unbind it before FinishSetup.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Setup")
	void StartSetup(const FCharacterRecipePawnInfo& Info);
	virtual void StartSetup_Implementation(const FCharacterRecipePawnInfo& Info) {}

	/**
	 * Executed when the character destroys
	 *
	 * Tips:
	 *	This function is executed only when InstancyngPolicy is "Instanced"
	 *
	 * Note:
	 *	if you have any binding to a delegate, etc., please unbind it.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Setup")
	void OnDestroy();
	virtual void OnDestroy_Implementation() {}

	/**
	 * Notify the InitState component that the setup process is finished
	 */
	UFUNCTION(BlueprintCallable, Category = "Setup")
	virtual void FinishSetup();

protected:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DeterminesOutputType = "InClass"))
	APawn* GetTypedPawn(TSubclassOf<APawn> InClass = nullptr) const;

	template<typename T>
	T* GetTypedPawn() const
	{
		return Cast<T>(GetTypedPawn(T::StaticClass()));
	}


	//////////////////////////////////////////////////////////////////////////////////
	// Non Instanced
public:
	/**
	 * Executed when all CharacterRecipes are added and setup begins.
	 */
	void HandleStartSetupNonInstanced(const FCharacterRecipePawnInfo& Info) const;

protected:
	/**
	 * Executed when all CharacterRecipes are added and setup begins.
	 * 
	 * Tips:
	 *	If the InstancingPolicy is "NonInstanced", this function will perform the setup process
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Setup")
	void StartSetupNonInstanced(FCharacterRecipePawnInfo Info) const;
	virtual void StartSetupNonInstanced_Implementation(FCharacterRecipePawnInfo Info) const {}

};
