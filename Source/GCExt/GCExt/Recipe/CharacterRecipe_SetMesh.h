// Copyright (C) 2024 owoDra

#pragma once

#include "Recipe/CharacterRecipe.h"

#include "GameplayTagContainer.h"

#include "CharacterRecipe_SetMesh.generated.h"

class USkeletalMesh;
class UAnimInstance;


/**
 * Entry data of Mesh to be changed
 */
USTRUCT(BlueprintType)
struct GCEXT_API FMeshToSetMesh
{
	GENERATED_BODY()
public:
	FMeshToSetMesh() {}

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Categories = "MeshType"))
	FGameplayTag MeshTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (InlineEditConditionToggle))
	bool bShouldChangeMesh{ false };

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bShouldChangeMesh"))
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh{ nullptr };

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (InlineEditConditionToggle))
	bool bShouldChangeAnimInstance{ false };

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bShouldChangeAnimInstance"))
	TSoftClassPtr<UAnimInstance> AnimInstance{ nullptr };

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (InlineEditConditionToggle))
	bool bShouldChangeLocation{ false };

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bShouldChangeLocation"))
	FVector NewLocation{ 0.0f, 0.0f, -90.0f };

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (InlineEditConditionToggle))
	bool bShouldChangeRotation{ false };

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bShouldChangeRotation"))
	FRotator NewRotation{ 0.0f, -90.0f, 0.0f };

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (InlineEditConditionToggle))
	bool bShouldChangeScale{ false };

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bShouldChangeScale"))
	FVector NewScale{ FVector::OneVector };

};


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
