// Copyright (C) 2024 owoDra

#include "GECharacterEditor.h"

#include "AssetTypeActions/AssetTypeActions_CharacterRecipeBlueprint.h"
#include "AssetTypeActions/AssetTypeActions_CharacterSet.h"

IMPLEMENT_MODULE(FGECharacterEditorModule, GECharacterEditor)


void FGECharacterEditorModule::StartupModule()
{
	RegisterAssetTypeActions();
}

void FGECharacterEditorModule::ShutdownModule()
{
	UnregisterAssetTypeActions();
}


void FGECharacterEditorModule::RegisterAssetTypeActionCategory()
{
	static const FName CategoryKey{ TEXT("GameCharacter") };
	static const FText CategoryDisplayName{ NSLOCTEXT("GameXXX","GameCharacterCategory", "Character") };

	Category = IAssetTools::Get().RegisterAdvancedAssetCategory(CategoryKey, CategoryDisplayName);
}

void FGECharacterEditorModule::RegisterAssetTypeActions()
{
	RegisterAssetTypeActionCategory();

	RegisterAsset<FAssetTypeActions_CharacterRecipeBlueprint>(RegisteredAssetTypeActions);
	RegisterAsset<FAssetTypeActions_CharacterSet>(RegisteredAssetTypeActions);
}

void FGECharacterEditorModule::UnregisterAssetTypeActions()
{
	UnregisterAssets(RegisteredAssetTypeActions);
}

void FGECharacterEditorModule::UnregisterAssets(TArray<TSharedPtr<FAssetTypeActions_Base>>& RegisteredAssets)
{
	const auto* AssetToolsPtr{ FModuleManager::GetModulePtr<FAssetToolsModule>(NAME_AssetToolsModule) };
	if (!AssetToolsPtr)
	{
		return;
	}

	auto& AssetTools{ AssetToolsPtr->Get() };
	for (auto& AssetTypeActionIt : RegisteredAssets)
	{
		if (AssetTypeActionIt.IsValid())
		{
			AssetTools.UnregisterAssetTypeActions(AssetTypeActionIt.ToSharedRef());
		}
	}
}
