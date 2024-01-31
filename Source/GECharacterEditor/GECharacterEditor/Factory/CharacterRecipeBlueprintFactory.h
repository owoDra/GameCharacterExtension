// Copyright (C) 2024 owoDra

#pragma once

#include "Factories/Factory.h"

#include "CharacterRecipeBlueprintFactory.generated.h"


UCLASS(HideCategories=Object, MinimalAPI)
class UCharacterRecipeBlueprintFactory : public UFactory
{
	GENERATED_BODY()
public:
	UCharacterRecipeBlueprintFactory(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	//
	// The type of blueprint that will be created
	//
	UPROPERTY(EditAnywhere, Category = "CharacterRecipeBlueprintFactory")
	TEnumAsByte<enum EBlueprintType> BlueprintType;

	//
	// The parent class of the created blueprint
	//
	UPROPERTY(EditAnywhere, Category = "CharacterRecipeBlueprintFactory")
	TSubclassOf<class UCharacterRecipe> ParentClass;

public:
	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

};
