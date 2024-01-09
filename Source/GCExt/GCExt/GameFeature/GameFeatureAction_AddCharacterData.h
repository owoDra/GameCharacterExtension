// Copyright (C) 2024 owoDra

#pragma once

#include "GameFeature/GameFeatureAction_WorldActionBase.h"

#include "GameFeatureAction_AddCharacterData.generated.h"

class UCharacterData;


/**
 * GameFeatureAction class to add specified CharacterData to CharacterDataComponent of Pawn or Character
 */
UCLASS(meta = (DisplayName = "GF Add Character Data"))
class UGameFeatureAction_AddCharacterData final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()
public:
	UGameFeatureAction_AddCharacterData() {}

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
#endif // WITH_EDITOR

private:
	struct FPerContextData
	{
		TArray<TSharedPtr<FComponentRequestHandle>> ExtensionRequestHandles;
	};

	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;

protected:
	UPROPERTY(EditAnywhere, Category = "CharacterData", meta = (AssetBundles = "Client,Server"))
	TSoftObjectPtr<const UCharacterData> CharacterData;

public:
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;

	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;

private:
	void Reset(FPerContextData& ActiveData);
	void HandlePawnExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);
	void AddCharacterDataForPawn(APawn* Pawn, FPerContextData& ActiveData);

};
