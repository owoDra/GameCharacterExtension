// Copyright (C) 2023 owoDra

#include "CharacterModifier_AddComponent.h"

#include "GCExtLogs.h"

#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterModifier_AddComponent)


#pragma region CharacterModifierInstance

void UCharacterModifierInstance_AddComponent::OnApply(const UCharacterModifier* Data, APawn* Pawn)
{
	if (ensure(Data) && ensure(Pawn))
	{
		const auto* World{ Pawn->GetWorld() };
		const auto NetMode{ World->GetNetMode() };
		const auto bIsServer{ NetMode != NM_Client };
		const auto bIsClient{ NetMode != NM_DedicatedServer };

		UE_LOG(LogGCE, Log, TEXT("Adding components for %s to world %s (Client: %d, Server: %d)"), *GetPathNameSafe(Pawn), *World->GetDebugDisplayName(), bIsClient ? 1 : 0, bIsServer ? 1 : 0);

		const auto* Modifier{ CastChecked<UCharacterModifier_AddComponentBase>(Data) };
		
		if ((bIsServer && Modifier->bAddToServer) || (bIsClient && Modifier->bAddToClient))
		{
			if (auto* ComponentClass{ Modifier->GetComponentClass() })
			{
				UE_LOG(LogGCE, Log, TEXT("+Component (Name: %s)"), *GetNameSafe(ComponentClass));

				DynamicComponent = NewObject<UActorComponent>(Pawn, ComponentClass);
				DynamicComponent->RegisterComponent();
				SetupComponent(DynamicComponent, Data, Pawn);
			}
		}
	}
}

void UCharacterModifierInstance_AddComponent::OnRemoval(APawn* Pawn)
{
	if (DynamicComponent)
	{
		FinalizeComponent(DynamicComponent, Pawn);
		DynamicComponent->DestroyComponent();
		DynamicComponent = nullptr;
	}
}

#pragma endregion


#pragma region CharacterModifierBase

UClass* UCharacterModifier_AddComponentBase::GetInstanceClass() const
{
	return UCharacterModifierInstance_AddComponent::StaticClass();
}

#pragma endregion


#pragma region CharacterModifier

UClass* UCharacterModifier_AddComponent::GetComponentClass() const
{
	return ComponentClass.IsNull() ? nullptr : 
			ComponentClass.IsValid() ? ComponentClass.Get() : ComponentClass.LoadSynchronous();
}

#pragma endregion
