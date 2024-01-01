// Copyright (C) 2023 owoDra

#pragma once

#include "CharacterModifier.h"

#include "GameplayTagContainer.h"

#include "CharacterModifier_SetMesh.generated.h"

class USkeletalMesh;
class UAnimInstance;


/**
 * Entry data of Mesh to be changed
 */
USTRUCT(BlueprintType)
struct FMeshToSetMesh
{
	GENERATED_BODY()
public:
	FMeshToSetMesh() {}

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag MeshTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh{ nullptr };

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftClassPtr<UAnimInstance> AnimInstance{ nullptr };

};


/**
 * Modifier class to Set mesh for Pawn
 */
UCLASS(meta = (DisplayName = "CM Set Mesh"))
class UCharacterModifier_SetMesh final : public UCharacterModifier
{
	GENERATED_BODY()
public:
	UCharacterModifier_SetMesh();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Set Mesh")
	TArray<FMeshToSetMesh> MeshesToSetMesh;

protected:
	virtual bool OnApply(APawn* Pawn) const override;

};
