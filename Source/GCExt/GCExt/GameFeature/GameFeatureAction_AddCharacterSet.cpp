// Copyright (C) 2024 owoDra

#include "GameFeatureAction_AddCharacterSet.h"

#include "CharacterInitStateComponent.h"
#include "CharacterSet.h"

#include "Character/GFCPawn.h"

#include "Components/GameFrameworkComponentManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_AddCharacterSet)


#define LOCTEXT_NAMESPACE "GameFeatures"

#if WITH_EDITOR
EDataValidationResult UGameFeatureAction_AddCharacterSet::IsDataValid(TArray<FText>& ValidationErrors)
{
	auto Result{ CombineDataValidationResults(Super::IsDataValid(ValidationErrors), EDataValidationResult::Valid) };

	if (PawnClass.IsNull())
	{
		Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);

		ValidationErrors.Add(FText::FromString(TEXT("Null PawnClass has set.")));
	}

	if (CharacterSet.IsNull())
	{
		Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);

		ValidationErrors.Add(FText::FromString(TEXT("Null CharacterSet has set.")));
	}

	return Result;
}
#endif


void UGameFeatureAction_AddCharacterSet::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	auto& ActiveData{ ContextData.FindOrAdd(Context) };

	if (!ensure(ActiveData.ExtensionRequestHandles.IsEmpty()))
	{
		Reset(ActiveData);
	}

	Super::OnGameFeatureActivating(Context);
}

void UGameFeatureAction_AddCharacterSet::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	auto* ActiveData{ ContextData.Find(Context) };

	if (ensure(ActiveData))
	{
		Reset(*ActiveData);
	}
}

void UGameFeatureAction_AddCharacterSet::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
{
	auto* World{ WorldContext.World() };
	const auto bIsGameWorld{ World ? World->IsGameWorld() : false };

	auto GameInstance{ WorldContext.OwningGameInstance };
	auto* Manager{ UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance) };

	auto& ActiveData{ ContextData.FindOrAdd(ChangeContext) };

	if (Manager && bIsGameWorld)
	{
		auto AddAbilitiesDelegate
		{
			UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(this, &ThisClass::HandlePawnExtension, ChangeContext)
		};

		auto ExtensionRequestHandle
		{
			Manager->AddExtensionHandler(PawnClass, AddAbilitiesDelegate)
		};

		ActiveData.ExtensionRequestHandles.Add(ExtensionRequestHandle);
	}
}


void UGameFeatureAction_AddCharacterSet::Reset(FPerContextData& ActiveData)
{
	ActiveData.ExtensionRequestHandles.Empty();
}

void UGameFeatureAction_AddCharacterSet::HandlePawnExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext)
{
	auto* AsPawn{ CastChecked<APawn>(Actor) };
	auto& ActiveData{ ContextData.FindOrAdd(ChangeContext) };

	if (bWaitPlayerState)
	{
		if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) || (EventName == AGFCPawn::NAME_PlayerStateReady))
		{
			AddCharacterSetForPawn(AsPawn, ActiveData);
		}
	}
	else
	{
		if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) || (EventName == UGameFrameworkComponentManager::NAME_GameActorReady))
		{
			AddCharacterSetForPawn(AsPawn, ActiveData);
		}
	}
}

void UGameFeatureAction_AddCharacterSet::AddCharacterSetForPawn(APawn* Pawn, FPerContextData& ActiveData)
{
	if (Pawn->HasAuthority())
	{
		if (auto* Component{ Pawn->FindComponentByClass<UCharacterInitStateComponent>() })
		{
			const auto* LoadedCharacterSet
			{
				CharacterSet.IsValid() ? CharacterSet.Get() : CharacterSet.LoadSynchronous()
			};

			TArray<FPendingCharacterRecipeHandle> DummyHundles;
			CharacterSet->AddCharacterRecipes(Component, DummyHundles);

			if (bCommitImmediately)
			{
				Component->CommitPendingCharacterRecipes();
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
