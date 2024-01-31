// Copyright (C) 2024 owoDra

#include "AssetTypeActions_CharacterRecipeBlueprint.h"

#include "Recipe/CharacterRecipeBlueprint.h"
#include "Recipe/CharacterRecipe.h"
#include "Factory/CharacterRecipeBlueprintFactory.h"
#include "GECharacterEditor.h"

#include "Misc/MessageDialog.h"
#include "BlueprintEditor.h"
#include "Kismet2/BlueprintEditorUtils.h"


#define LOCTEXT_NAMESPACE "AssetTypeActions"

UClass* FAssetTypeActions_CharacterRecipeBlueprint::GetSupportedClass() const
{
	return UCharacterRecipeBlueprint::StaticClass();
}

FText FAssetTypeActions_CharacterRecipeBlueprint::GetName() const
{ 
	return LOCTEXT("AssetTypeActions_CharacterRecipeBlueprint", "Character Recipe"); 
}

FColor FAssetTypeActions_CharacterRecipeBlueprint::GetTypeColor() const
{
	return FColor(204, 102, 102);
}

uint32 FAssetTypeActions_CharacterRecipeBlueprint::GetCategories()
{
	return FGECharacterEditorModule::Category;
}

const TArray<FText>& FAssetTypeActions_CharacterRecipeBlueprint::GetSubMenus() const
{
	static const TArray<FText> SubMenus
	{
		LOCTEXT("SubMenus_CharacterRecipeBlueprint", "Recipe")
	};

	return SubMenus;
}


void FAssetTypeActions_CharacterRecipeBlueprint::OpenAssetEditor( const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor )
{
	auto Mode{ EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone };

	for (auto ObjIt{ InObjects.CreateConstIterator() }; ObjIt; ++ObjIt)
	{
		if (auto Blueprint{ Cast<UBlueprint>(*ObjIt) })
		{
			auto bLetOpen{ true };

			if (!Blueprint->ParentClass)
			{
				const auto MessageReplyType
				{
					FMessageDialog::Open(EAppMsgType::YesNo,
						LOCTEXT("FailedToLoadCharacterRecipeBlueprintWithContinue", "CharacterRecipe Blueprint could not be loaded because it derives from an invalid class.  Check to make sure the parent class for this blueprint hasn't been removed! Do you want to continue (it can crash the editor)?"))
				};

				bLetOpen = (EAppReturnType::Yes == MessageReplyType);
			}
		
			if (bLetOpen)
			{
				TSharedRef<FBlueprintEditor> NewEditor{ new FBlueprintEditor() };

				TArray<UBlueprint*> Blueprints;
				Blueprints.Add(Blueprint);

				NewEditor->InitBlueprintEditor(Mode, EditWithinLevelEditor, Blueprints, true);
			}
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("FailedToLoadCharacterRecipeBlueprint", "CharacterRecipe Blueprint could not be loaded because it derives from an invalid class.  Check to make sure the parent class for this blueprint hasn't been removed!"));
		}
	}
}


UFactory* FAssetTypeActions_CharacterRecipeBlueprint::GetFactoryForBlueprintType(UBlueprint* InBlueprint) const
{
	auto* CharacterRecipeBlueprintFactory{ NewObject<UCharacterRecipeBlueprintFactory>() };

	CharacterRecipeBlueprintFactory->ParentClass = TSubclassOf<UCharacterRecipe>(*InBlueprint->GeneratedClass);

	return CharacterRecipeBlueprintFactory;
}

#undef LOCTEXT_NAMESPACE
