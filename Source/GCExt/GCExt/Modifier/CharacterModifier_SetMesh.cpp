// Copyright (C) 2023 owoDra

#include "CharacterModifier_SetMesh.h"

#include "CharacterMeshAccessorInterface.h"
#include "GCExtLogs.h"

#include "GameFramework/Pawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterModifier_SetMesh)


UCharacterModifier_SetMesh::UCharacterModifier_SetMesh()
{
	bOnlyApplyOnLocal = false;
	bApplyOnClient = true;
	bApplyOnServer = true;
}


bool UCharacterModifier_SetMesh::OnApply(APawn* Pawn) const
{
	const auto bCanApply{ Super::OnApply(Pawn) };

	if (bCanApply)
	{
		for (const auto& MeshToSet : MeshesToSetMesh)
		{
			if (auto* Mesh{ ICharacterMeshAccessorInterface::Execute_GetMeshByTag(Pawn, MeshToSet.MeshTag) })
			{
				auto* LoadedSkeltalMesh
				{
					MeshToSet.SkeletalMesh.IsNull() ? nullptr :
					MeshToSet.SkeletalMesh.IsValid() ? MeshToSet.SkeletalMesh.Get() : MeshToSet.SkeletalMesh.LoadSynchronous()
				};

				auto* LoadedAnimInstanceClass
				{
					MeshToSet.AnimInstance.IsNull() ? nullptr :
					MeshToSet.AnimInstance.IsValid() ? MeshToSet.AnimInstance.Get() : MeshToSet.AnimInstance.LoadSynchronous()
				};

				UE_LOG(LogGCE, Log, TEXT("++SkeltalMesh (Name: %s)"), *GetNameSafe(LoadedSkeltalMesh));
				UE_LOG(LogGCE, Log, TEXT("++AnimInstance (Name: %s)"), *GetNameSafe(LoadedAnimInstanceClass));

				Mesh->SetSkeletalMesh(LoadedSkeltalMesh);
				Mesh->SetAnimInstanceClass(LoadedAnimInstanceClass);
			}
		}
	}

	return bCanApply;
}
