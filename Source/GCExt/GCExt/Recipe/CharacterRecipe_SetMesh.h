// Copyright (C) 2024 owoDra

#pragma once

#include "Recipe/CharacterRecipe.h"

#include "Recipe/CharacterSetMeshTypes.h"

#include "CharacterRecipe_SetMesh.generated.h"


/**
 * Recipe class to Set mesh for Pawn
 */
UCLASS()
class UCharacterRecipe_SetMesh final : public UCharacterRecipe
{
	GENERATED_BODY()
public:
	UCharacterRecipe_SetMesh();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Set Mesh")
	TArray<FMeshToSetMesh> MeshesToSetMesh;

protected:
	virtual void StartSetupNonInstanced_Implementation(FCharacterRecipePawnInfo Info) const override;

};
