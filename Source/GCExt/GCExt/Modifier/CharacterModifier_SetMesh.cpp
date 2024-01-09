// Copyright (C) 2024 owoDra

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
				UE_LOG(LogGCE, Log, TEXT("+Modify Mesh (Name: %s)"), *GetNameSafe(Mesh));

				// Change Mesh

				if (MeshToSet.bShouldChangeMesh)
				{
					auto* LoadedSkeltalMesh
					{
						MeshToSet.SkeletalMesh.IsNull() ? nullptr :
						MeshToSet.SkeletalMesh.IsValid() ? MeshToSet.SkeletalMesh.Get() : MeshToSet.SkeletalMesh.LoadSynchronous()
					};

					UE_LOG(LogGCE, Log, TEXT("++SkeltalMesh (Name: %s)"), *GetNameSafe(LoadedSkeltalMesh));

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

					UE_LOG(LogGCE, Log, TEXT("++AnimInstance (Name: %s)"), *GetNameSafe(LoadedAnimInstanceClass));

					Mesh->SetAnimInstanceClass(LoadedAnimInstanceClass);
				}

				// Change Location

				if (MeshToSet.bShouldChangeLocation)
				{
					UE_LOG(LogGCE, Log, TEXT("++SetLocation (%s)"), *MeshToSet.NewLocation.ToString());

					Mesh->SetRelativeLocation(MeshToSet.NewLocation);
				}
				
				// Change Rotation

				if (MeshToSet.bShouldChangeRotation)
				{
					UE_LOG(LogGCE, Log, TEXT("++SetRotation (%s)"), *MeshToSet.NewRotation.ToString());

					Mesh->SetRelativeRotation(MeshToSet.NewRotation);
				}

				// Change Scale

				if (MeshToSet.bShouldChangeScale)
				{
					UE_LOG(LogGCE, Log, TEXT("++SetScale (%s)"), *MeshToSet.NewScale.ToString());

					Mesh->SetRelativeScale3D(MeshToSet.NewScale);
				}
			}
		}
	}

	return bCanApply;
}
