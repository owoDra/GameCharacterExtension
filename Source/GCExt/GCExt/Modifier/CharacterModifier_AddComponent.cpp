// Copyright (C) 2023 owoDra

#include "CharacterModifier_AddComponent.h"

#include "GCExtLogs.h"

#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterModifier_AddComponent)


#pragma region CharacterModifierInstance

void UCharacterModifierInstance_AddComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, DynamicComponent);
}


void UCharacterModifierInstance_AddComponent::OnApply(const UCharacterModifier* Data, APawn* Pawn)
{
	if (ensure(Data) && ensure(Pawn))
	{
		const auto* World{ Pawn->GetWorld() };
		const auto NetMode{ World->GetNetMode() };
		const auto bIsServer{ NetMode != NM_Client };
		const auto bIsClient{ NetMode != NM_DedicatedServer };

		UE_LOG(LogGCE, Log, TEXT("Adding components for %s to world %s (Client: %d, Server: %d)"), *GetPathNameSafe(Pawn), *World->GetDebugDisplayName(), bIsClient ? 1 : 0, bIsServer ? 1 : 0);

		const auto* Modifier{ CastChecked<UCharacterModifier_AddComponent>(Data) };
		const auto ComponentSoftClass{ Modifier->ComponentClass };

		if (((bIsServer && Modifier->bAddToServer) || (bIsClient && Modifier->bAddToClient)) && 
			(!ComponentSoftClass.IsNull()))
		{
			auto* ComponentClass{ ComponentSoftClass.LoadSynchronous() };

			UE_LOG(LogGCE, Log, TEXT("+Component (Name: %s)"), *GetNameSafe(ComponentClass));

			DynamicComponent = NewObject<UActorComponent>(Pawn, ComponentClass);
			SetupComponent(DynamicComponent);
			DynamicComponent->RegisterComponent();
		}
	}
}

void UCharacterModifierInstance_AddComponent::OnRemoval(APawn* Pawn)
{
	if (DynamicComponent)
	{
		DynamicComponent->DestroyComponent();
	}
}

#pragma endregion


#pragma region CharacterModifier

UClass* UCharacterModifier_AddComponent::GetInstanceClass() const
{
	return UCharacterModifierInstance_AddComponent::StaticClass();
}

#pragma endregion
