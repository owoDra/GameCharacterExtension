// Copyright (C) 2024 owoDra

#include "GameFeatureAction_AddCharacterSet.h"

#include "CharacterInitStateComponent.h"
#include "CharacterSet.h"

#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_AddCharacterSet)


#define LOCTEXT_NAMESPACE "GameFeatures"

#if WITH_EDITOR
EDataValidationResult UGameFeatureAction_AddCharacterSet::IsDataValid(TArray<FText>& ValidationErrors)
{
	auto Result{ CombineDataValidationResults(Super::IsDataValid(ValidationErrors), EDataValidationResult::Valid) };

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
	auto GameInstance{ WorldContext.OwningGameInstance };
	auto& ActiveData{ ContextData.FindOrAdd(ChangeContext) };

	if ((GameInstance != nullptr) && (World != nullptr) && World->IsGameWorld())
	{
		if (auto* Manager{ UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance) })
		{
			auto AddAbilitiesDelegate
			{ 
				UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(this, &ThisClass::HandlePawnExtension, ChangeContext) 
			};

			auto ExtensionRequestHandle
			{ 
				Manager->AddExtensionHandler(APawn::StaticClass(), AddAbilitiesDelegate) 
			};

			ActiveData.ExtensionRequestHandles.Add(ExtensionRequestHandle);
		}
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

	if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) || (EventName == UGameFrameworkComponentManager::NAME_GameActorReady))
	{
		AddCharacterSetForPawn(AsPawn, ActiveData);
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
