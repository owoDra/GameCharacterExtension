// Copyright (C) 2023 owoDra

#include "GameFeatureAction_AddCharacterData.h"

#include "CharacterDataComponent.h"

#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_AddCharacterData)


#define LOCTEXT_NAMESPACE "GameFeatures"

#if WITH_EDITOR
EDataValidationResult UGameFeatureAction_AddCharacterData::IsDataValid(TArray<FText>& ValidationErrors)
{
	auto Result{ CombineDataValidationResults(Super::IsDataValid(ValidationErrors), EDataValidationResult::Valid) };

	if (CharacterData.IsNull())
	{
		Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);

		ValidationErrors.Add(FText::FromString(TEXT("Null CharacterData has set.")));
	}

	return Result;
}
#endif


void UGameFeatureAction_AddCharacterData::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	auto& ActiveData{ ContextData.FindOrAdd(Context) };

	if (!ensure(ActiveData.ExtensionRequestHandles.IsEmpty()))
	{
		Reset(ActiveData);
	}

	Super::OnGameFeatureActivating(Context);
}

void UGameFeatureAction_AddCharacterData::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	auto* ActiveData{ ContextData.Find(Context) };

	if (ensure(ActiveData))
	{
		Reset(*ActiveData);
	}
}

void UGameFeatureAction_AddCharacterData::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
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


void UGameFeatureAction_AddCharacterData::Reset(FPerContextData& ActiveData)
{
	ActiveData.ExtensionRequestHandles.Empty();
}

void UGameFeatureAction_AddCharacterData::HandlePawnExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext)
{
	auto* AsPawn{ CastChecked<APawn>(Actor) };
	auto& ActiveData{ ContextData.FindOrAdd(ChangeContext) };

	if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) || (EventName == UGameFrameworkComponentManager::NAME_GameActorReady))
	{
		AddCharacterDataForPawn(AsPawn, ActiveData);
	}
}

void UGameFeatureAction_AddCharacterData::AddCharacterDataForPawn(APawn* Pawn, FPerContextData& ActiveData)
{
	if (auto* Component{ Pawn->FindComponentByClass<UCharacterDataComponent>() })
	{
		const auto* LoadedCharacterData
		{
			CharacterData.IsValid() ? CharacterData.Get() : CharacterData.LoadSynchronous()
		};

		Component->SetCharacterData(LoadedCharacterData);
	}
}

#undef LOCTEXT_NAMESPACE
