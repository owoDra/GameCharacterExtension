// Copyright (C) 2023 owoDra

#include "CharacterData.h"

#include "CharacterModifier.h"
#include "CharacterModifierSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterData)


UCharacterData::UCharacterData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UCharacterData::GetModifiers(TArray<UCharacterModifier*>& OutArray) const
{
	OutArray.Empty();

	for (const auto& ModifierSet : ModifierSets)
	{
		OutArray.Append(ModifierSet->GetModifiers());
	}

	OutArray.Append(Modifiers);
}
