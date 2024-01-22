// Copyright (C) 2024 owoDra

#include "CharacterRecipe_SetMesh.h"

#include "CharacterInitStateComponent.h"
#include "GCExtLogs.h"

#include "Character/CharacterMeshAccessorInterface.h"

#include "GameFramework/Pawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterRecipe_SetMesh)


UCharacterRecipe_SetMesh::UCharacterRecipe_SetMesh()
{
	InstancingPolicy = ECharacterRecipeInstancingPolicy::NonInstanced;
	NetExecutionPolicy = ECharacterRecipeNetExecutionPolicy::Both;

#if WITH_EDITOR
	StaticClass()->FindPropertyByName(FName{ TEXTVIEW("InstancingPolicy") })->SetPropertyFlags(CPF_DisableEditOnTemplate);
	StaticClass()->FindPropertyByName(FName{ TEXTVIEW("NetExecutionPolicy") })->SetPropertyFlags(CPF_DisableEditOnTemplate);
#endif
}


void UCharacterRecipe_SetMesh::StartSetupNonInstanced_Implementation(FCharacterRecipePawnInfo Info) const
{
	for (const auto& MeshToSet : MeshesToSetMesh)
	{
		if (auto* Mesh{ ICharacterMeshAccessorInterface::Execute_GetMeshByTag(Info.Pawn.Get(), MeshToSet.MeshTag)})
		{
			UE_LOG(LogGameExt_CharacterRecipe, Log, TEXT("+Modify Mesh (Name: %s)"), *GetNameSafe(Mesh));

			// Change Mesh

			if (MeshToSet.bShouldChangeMesh)
			{
				auto* LoadedSkeltalMesh
				{
					MeshToSet.SkeletalMesh.IsNull() ? nullptr :
					MeshToSet.SkeletalMesh.IsValid() ? MeshToSet.SkeletalMesh.Get() : MeshToSet.SkeletalMesh.LoadSynchronous()
				};

				UE_LOG(LogGameExt_CharacterRecipe, Log, TEXT("++SkeltalMesh (Name: %s)"), *GetNameSafe(LoadedSkeltalMesh));

				Mesh->SetSkeletalMesh(LoadedSkeltalMesh);
			}

			// Change AnimInstance

			if (MeshToSet.bShouldChangeAnimInstance)
			{
				auto* LoadedAnimInstanceClass
				{
					MeshToSet.AnimInstance.IsNull() ? nullptr :
					MeshToSet.AnimInstance.IsValid() ? MeshToSet.AnimInstance.Get() : MeshToSet.AnimInstance.LoadSynchronous()
				};

				UE_LOG(LogGameExt_CharacterRecipe, Log, TEXT("++AnimInstance (Name: %s)"), *GetNameSafe(LoadedAnimInstanceClass));

				Mesh->SetAnimInstanceClass(LoadedAnimInstanceClass);
			}

			// Change Location

			if (MeshToSet.bShouldChangeLocation)
			{
				UE_LOG(LogGameExt_CharacterRecipe, Log, TEXT("++SetLocation (%s)"), *MeshToSet.NewLocation.ToString());

				Mesh->SetRelativeLocation(MeshToSet.NewLocation);
			}

			// Change Rotation

			if (MeshToSet.bShouldChangeRotation)
			{
				UE_LOG(LogGameExt_CharacterRecipe, Log, TEXT("++SetRotation (%s)"), *MeshToSet.NewRotation.ToString());

				Mesh->SetRelativeRotation(MeshToSet.NewRotation);
			}

			// Change Scale

			if (MeshToSet.bShouldChangeScale)
			{
				UE_LOG(LogGameExt_CharacterRecipe, Log, TEXT("++SetScale (%s)"), *MeshToSet.NewScale.ToString());

				Mesh->SetRelativeScale3D(MeshToSet.NewScale);
			}
		}
	}
}
