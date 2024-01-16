// Copyright (C) 2024 owoDra

#include "CharacterSet.h"

#include "CharacterInitStateComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterSet)


UCharacterSet::UCharacterSet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UCharacterSet::AddCharacterRecipes(UCharacterInitStateComponent* InitStateComponent, TArray<FPendingCharacterRecipeHandle>& OutHandles) const
{
	if (InitStateComponent)
	{
		OutHandles = InitStateComponent->AddMultipePendingCharacterRecipes(CharacterRecipes);
	}
}
