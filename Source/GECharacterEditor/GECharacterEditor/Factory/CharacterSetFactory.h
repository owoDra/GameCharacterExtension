// Copyright (C) 2024 owoDra

#pragma once

#include "Factories/Factory.h"
#include "AssetTypeActions_Base.h"

#include "CharacterSetFactory.generated.h"

class UCharacterSet;


/**
 * Class for creating SettingData in the context menu of asset creation
 */
UCLASS()
class UCharacterSetFactory : public UFactory
{
public:
	GENERATED_BODY()
public:
	UCharacterSetFactory();

protected:
	UPROPERTY(EditAnywhere, Category = "DataAsset")
	TSubclassOf<UCharacterSet> DataAssetClass;

public:
	virtual FText GetDisplayName() const override;
	virtual bool ShouldShowInNewMenu() const override { return true; }
	virtual bool DoesSupportClass(UClass* Class) override;

	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

};
