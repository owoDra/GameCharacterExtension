// Copyright (C) 2024 owoDra

#pragma once

#include "PendingCharacterRecipeHandle.generated.h"


/**
 * Handle that points to a specific pending character recipe.
 * 
 * Tips:
 *	These are globally unique
 */
USTRUCT(BlueprintType)
struct FPendingCharacterRecipeHandle
{
	GENERATED_BODY()
public:
	FPendingCharacterRecipeHandle() : Handle(INDEX_NONE) {}

	/** 
	 * Sets this to a valid handle 
	 */
	void GenerateNewHandle();

private:
	//
	// Unique handle indexing
	//
	UPROPERTY()
	int32 Handle;

public:
	bool operator==(const FPendingCharacterRecipeHandle& Other) const { return Handle == Other.Handle; }
	bool operator!=(const FPendingCharacterRecipeHandle& Other) const { return Handle != Other.Handle; }

public:
	/** 
	 * True if GenerateNewHandle was called on this handle 
	 */
	bool IsValid() const { return Handle != INDEX_NONE; }

	/**
	 * Returns a hash of this handle
	 */
	friend uint32 GetTypeHash(const FPendingCharacterRecipeHandle& ActiveRecipeHandle) { return ::GetTypeHash(ActiveRecipeHandle.Handle); }

	/**
	 * Return this handle as string.
	 */
	FString ToString() const { return IsValid() ? FString::FromInt(Handle) : TEXT("Invalid"); }

};
