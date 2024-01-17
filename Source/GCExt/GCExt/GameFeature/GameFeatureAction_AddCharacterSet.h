// Copyright (C) 2024 owoDra

#pragma once

#include "GameFeature/GameFeatureAction_WorldActionBase.h"

#include "GameFeatureAction_AddCharacterSet.generated.h"

class UCharacterSet;


/**
 * GameFeatureAction class to add specified CharacterSet to CharacterSetComponent of Pawn or Character
 */
UCLASS(meta = (DisplayName = "GF Add Character Set"))
class UGameFeatureAction_AddCharacterSet final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()
public:
	UGameFeatureAction_AddCharacterSet() {}

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
	UPROPERTY(EditAnywhere, Category = "CharacterSet", meta = (AssetBundles = "Client,Server"))
	TSoftObjectPtr<const UCharacterSet> CharacterSet;

	UPROPERTY(EditAnywhere, Category = "CharacterSet", meta = (AssetBundles = "Client,Server"))
	bool bCommitImmediately{ true };

public:
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;

	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;

private:
	void Reset(FPerContextData& ActiveData);
	void HandlePawnExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);
	void AddCharacterSetForPawn(APawn* Pawn, FPerContextData& ActiveData);

};
