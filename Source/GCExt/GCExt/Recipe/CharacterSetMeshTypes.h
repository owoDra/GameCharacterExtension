// Copyright (C) 2024 owoDra

#pragma once

#include "GameplayTagContainer.h"

#include "CharacterSetMeshTypes.generated.h"

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
