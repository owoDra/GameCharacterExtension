// Copyright (C) 2024 owoDra

#include "CharacterInitStateComponent.h"

#include "Recipe/CharacterRecipe.h"
#include "GCExtLogs.h"

#include "InitState/InitStateTags.h"

#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/Pawn.h"
#include "Engine/ActorChannel.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterInitStateComponent)


UCharacterInitStateComponent::UCharacterInitStateComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UCharacterInitStateComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased	= true;
	Params.Condition = COND_None;
	Params.RepNotifyCondition = REPNOTIFY_Always;
	DOREPLIFETIME_WITH_PARAMS_FAST(UCharacterInitStateComponent, ActiveCharacterRecipes, Params);
}


#pragma region Init State Flows

void UCharacterInitStateComponent::OnRegister()
{
	// This component can only be added to classes derived from APawn

	auto* Pawn{ GetPawn<APawn>() };
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("CharacterInitStateComponent on [%s] can only be added to Pawn actors."), *GetNameSafe(GetOwner()));

	ActiveCharacterRecipes.RegisterOwner(Pawn, this);

	Super::OnRegister();
}

void UCharacterInitStateComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ReleaseCharacterRecipes();

	Super::EndPlay(EndPlayReason);
}


bool UCharacterInitStateComponent::CanChangeInitStateToDataAvailable(UGameFrameworkComponentManager* Manager) const
{
	// Return false if not yet committed

	if (ActiveCharacterRecipes.GetCurrentApplicationState() != ECharacterRecipesApplicationState::Complete)
	{
		return false;
	}

	// Returns true if all conditions are met

	return true;
}

void UCharacterInitStateComponent::HandleChangeInitStateToSpawned(UGameFrameworkComponentManager* Manager)
{
	// Add a default CharacterRecipe at this time.

	AddDefaultCharacterRecipeToPendingList();

	if (bAutoCommitCharacterRecipes)
	{
		CommitPendingCharacterRecipes();
	}
}

#pragma endregion


#pragma region Character Recipes

FPendingCharacterRecipeHandle UCharacterInitStateComponent::AddPendingCharacterRecipe(const TSubclassOf<UCharacterRecipe>& InClass)
{
	// Suspend if has no authority

	if (!HasAuthority())
	{
		return FPendingCharacterRecipeHandle();
	}

	// Suspend if already commited

	if (ActiveCharacterRecipes.GetCurrentApplicationState() != ECharacterRecipesApplicationState::PreCommit)
	{
		return FPendingCharacterRecipeHandle();
	}

	return ActiveCharacterRecipes.AddPendingCharacterRecipe(InClass);
}

TArray<FPendingCharacterRecipeHandle> UCharacterInitStateComponent::AddMultipePendingCharacterRecipes(const TArray<TSubclassOf<UCharacterRecipe>>& InClasses)
{
	// Suspend if has no authority

	if (!HasAuthority())
	{
		return TArray<FPendingCharacterRecipeHandle>();
	}

	// Suspend if already commited

	if (ActiveCharacterRecipes.GetCurrentApplicationState() != ECharacterRecipesApplicationState::PreCommit)
	{
		return TArray<FPendingCharacterRecipeHandle>();
	}

	TArray<FPendingCharacterRecipeHandle> OutHandles;

	for (const auto& RecipeClass : InClasses)
	{
		OutHandles.Emplace(ActiveCharacterRecipes.AddPendingCharacterRecipe(RecipeClass));
	}

	return OutHandles;
}

void UCharacterInitStateComponent::RemovePendingCharacterRecipe(const FPendingCharacterRecipeHandle& InHandle)
{
	// Suspend if has no authority

	if (!HasAuthority())
	{
		return;
	}

	// Suspend if already commited

	if (ActiveCharacterRecipes.GetCurrentApplicationState() != ECharacterRecipesApplicationState::PreCommit)
	{
		return;
	}

	ActiveCharacterRecipes.RemovePendingCharacterRecipe(InHandle);
}

void UCharacterInitStateComponent::RemoveMultipePendingCharacterRecipes(const TArray<FPendingCharacterRecipeHandle>& InHandles)
{
	// Suspend if has no authority

	if (!HasAuthority())
	{
		return;
	}

	// Suspend if already commited

	if (ActiveCharacterRecipes.GetCurrentApplicationState() != ECharacterRecipesApplicationState::PreCommit)
	{
		return;
	}

	for (const auto& Handle : InHandles)
	{
		ActiveCharacterRecipes.RemovePendingCharacterRecipe(Handle);
	}
}

void UCharacterInitStateComponent::ClearPendingCharacterRecipes()
{
	// Suspend if has no authority

	if (!HasAuthority())
	{
		return;
	}

	// Suspend if already commited

	if (ActiveCharacterRecipes.GetCurrentApplicationState() != ECharacterRecipesApplicationState::PreCommit)
	{
		return;
	}

	ActiveCharacterRecipes.ClearPendingCharacterRecipes();
}

void UCharacterInitStateComponent::CommitPendingCharacterRecipes()
{
	// Suspend if has no authority

	if (!HasAuthority())
	{
		return;
	}

	// Suspend if already commited

	if (ActiveCharacterRecipes.GetCurrentApplicationState() != ECharacterRecipesApplicationState::PreCommit)
	{
		return;
	}

	ActiveCharacterRecipes.CommitPendingCharacterRecipes();

	HandleAllRecipesCommitted();
}

void UCharacterInitStateComponent::HandleAllRecipesCommitted()
{
	ActiveCharacterRecipes.ApplicationState = ECharacterRecipesApplicationState::Commited;
	ActiveCharacterRecipes.ExecuteCharacterRecipeSetup();

	CheckDefaultInitialization();
}

void UCharacterInitStateComponent::OnRep_CommitRecipes()
{
	HandleAllRecipesCommitted();
}

void UCharacterInitStateComponent::AddDefaultCharacterRecipeToPendingList()
{
	AddMultipePendingCharacterRecipes(DefaultCharacterRecipes);
}

void UCharacterInitStateComponent::ReleaseCharacterRecipes()
{
	ActiveCharacterRecipes.ReleaseCharacterRecipes();
}


void UCharacterInitStateComponent::HandleRecipeSetupFinished(const FActiveCharacterRecipeHandle& Handle)
{
	ActiveCharacterRecipes.AddActiveRecipeHandlePendingFinish(Handle);

	if (!DelayedCheckRecipeSetupFinishedTimerHandle.IsValid())
	{
		if (auto* World{ GetWorld() })
		{
			DelayedCheckRecipeSetupFinishedTimerHandle =
				World->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleDelayedCheckRecipeSetupFinished);
		}
	}
}

void UCharacterInitStateComponent::HandleDelayedCheckRecipeSetupFinished()
{
	ActiveCharacterRecipes.MarkActiveRecipeHandlePendingFinish();
	DelayedCheckRecipeSetupFinishedTimerHandle.Invalidate();
	CheckDefaultInitialization();
}

#pragma endregion
