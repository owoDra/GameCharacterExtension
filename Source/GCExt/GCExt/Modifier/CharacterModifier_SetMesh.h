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
