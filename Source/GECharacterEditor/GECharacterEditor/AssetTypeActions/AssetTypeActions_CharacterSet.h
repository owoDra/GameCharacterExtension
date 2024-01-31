// Copyright (C) 2024 owoDra

#pragma once

#include "AssetTypeActions_Base.h"

class UCharacterSet;


/**
 * Class to add a CharacterSet creation item to the context menu of an asset creation
 */
class FAssetTypeActions_CharacterSet : public FAssetTypeActions_Base
{
public:
	virtual ~FAssetTypeActions_CharacterSet() override = default;

public:
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual uint32 GetCategories() override;
	virtual UClass* GetSupportedClass() const override;

};
