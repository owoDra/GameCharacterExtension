﻿// Copyright (C) 2024 owoDra

#pragma once

#include "AssetTypeActions/AssetTypeActions_Blueprint.h"

#include "Toolkits/IToolkitHost.h"

class UFactory;


class FAssetTypeActions_CharacterRecipeBlueprint : public FAssetTypeActions_Blueprint
{
public:
	virtual UClass* GetSupportedClass() const override;
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual uint32 GetCategories() override;
	virtual const TArray<FText>& GetSubMenus() const override;
	
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;

	virtual UFactory* GetFactoryForBlueprintType(UBlueprint* InBlueprint) const override;

};
