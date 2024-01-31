// Copyright (C) 2024 owoDra

#include "AssetTypeActions_CharacterSet.h"

#include "CharacterInitStateComponent.h"
#include "CharacterSet.h"
#include "GECharacterEditor.h"


#pragma region AssetTypeAction

FText FAssetTypeActions_CharacterSet::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_CharacterSet", "Character Set");
}

FColor FAssetTypeActions_CharacterSet::GetTypeColor() const
{
	return FColor(153, 0, 51);
}

uint32 FAssetTypeActions_CharacterSet::GetCategories()
{
	return FGECharacterEditorModule::Category;
}

UClass* FAssetTypeActions_CharacterSet::GetSupportedClass() const
{
	return UCharacterSet::StaticClass();
}

#pragma endregion
