// Copyright (C) 2024 owoDra

#include "CharacterRecipeBlueprintFactory.h"

#include "Recipe/CharacterRecipeBlueprint.h"
#include "Recipe/CharacterRecipe.h"

#include "InputCoreTypes.h"
#include "UObject/Interface.h"
#include "Layout/Visibility.h"
#include "Input/Reply.h"
#include "Widgets/SWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Misc/MessageDialog.h"
#include "Modules/ModuleManager.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SWindow.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Input/SButton.h"
#include "Styling/AppStyle.h"

#include "ClassViewerModule.h"
#include "ClassViewerFilter.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "BlueprintEditorSettings.h"

#include "SlateOptMacros.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterRecipeBlueprintFactory)


#define LOCTEXT_NAMESPACE "UCharacterRecipeBlueprintFactory"

//////////////////////////////////////////////////////////////////////////////////
// Dialog to configure creation properties

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

class SCharacterRecipeBlueprintCreateDialog : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCharacterRecipeBlueprintCreateDialog){}

	SLATE_END_ARGS()

	/** 
	 * Constructs this widget with InArgs 
	 */
	void Construct(const FArguments& InArgs)
	{
		bOkClicked = false;
		ParentClass = UCharacterRecipe::StaticClass();

		ChildSlot
			[
				SNew(SBorder)
				.Visibility(EVisibility::Visible)
				.BorderImage(FAppStyle::GetBrush("Menu.Background"))
				[
					SNew(SBox)
					.Visibility(EVisibility::Visible)
					.WidthOverride(500.0f)
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.FillHeight(1)
						[
							SNew(SBorder)
							.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
							.Content()
							[
								SAssignNew(ParentClassContainer, SVerticalBox)
							]
						]

						// Ok/Cancel buttons
						+ SVerticalBox::Slot()
							.AutoHeight()
							.HAlign(HAlign_Right)
							.VAlign(VAlign_Bottom)
							.Padding(8)
							[
								SNew(SUniformGridPanel)
								.SlotPadding(FAppStyle::GetMargin("StandardDialog.SlotPadding"))
								.MinDesiredSlotWidth(FAppStyle::GetFloat("StandardDialog.MinDesiredSlotWidth"))
								.MinDesiredSlotHeight(FAppStyle::GetFloat("StandardDialog.MinDesiredSlotHeight"))
								+ SUniformGridPanel::Slot(0, 0)
								[
									SNew(SButton)
									.HAlign(HAlign_Center)
									.ContentPadding(FAppStyle::GetMargin("StandardDialog.ContentPadding"))
									.OnClicked(this, &SCharacterRecipeBlueprintCreateDialog::OkClicked)
									.Text(LOCTEXT("CreateCharacterRecipeBlueprintOk", "OK"))
								]
								+ SUniformGridPanel::Slot(1, 0)
									[
										SNew(SButton)
										.HAlign(HAlign_Center)
										.ContentPadding(FAppStyle::GetMargin("StandardDialog.ContentPadding"))
										.OnClicked(this, &SCharacterRecipeBlueprintCreateDialog::CancelClicked)
										.Text(LOCTEXT("CreateCharacterRecipeBlueprintCancel", "Cancel"))
									]
							]
					]
				]
			];

		MakeParentClassPicker();
	}

	/** 
	 * Sets properties for the supplied CharacterRecipeBlueprintFactory 
	 */
	bool ConfigureProperties(TWeakObjectPtr<UCharacterRecipeBlueprintFactory> InCharacterRecipeBlueprintFactory)
	{
		CharacterRecipeBlueprintFactory = InCharacterRecipeBlueprintFactory;

		TSharedRef<SWindow> Window = SNew(SWindow)
			.Title(LOCTEXT("CreateCharacterRecipeBlueprintOptions", "Create CharacterRecipe Blueprint"))
			.ClientSize(FVector2D(400, 700))
			.SupportsMinimize(false).SupportsMaximize(false)
			[
				AsShared()
			];

		PickerWindow = Window;

		GEditor->EditorAddModalWindow(Window);
		CharacterRecipeBlueprintFactory.Reset();

		return bOkClicked;
	}

private:
	class FCharacterRecipeBlueprintParentFilter : public IClassViewerFilter
	{
	public:
		//
		// All children of these classes will be included unless filtered out by another setting.
		//
		TSet<const UClass*> AllowedChildrenOfClasses;

		FCharacterRecipeBlueprintParentFilter() {}

		virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
		{
			// If it appears on the allowed child-of classes list (or there is nothing on that list)

			return InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InClass) != EFilterReturn::Failed;
		}

		virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const IUnloadedBlueprintData > InUnloadedClassData, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
		{
			// If it appears on the allowed child-of classes list (or there is nothing on that list)

			return InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InUnloadedClassData) != EFilterReturn::Failed;
		}
	};

	/** 
	 * Creates the combo menu for the parent class 
	 */
	void MakeParentClassPicker()
	{
		// Load the classviewer module to display a class picker

		auto& ClassViewerModule{ FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer") };

		// Fill in options

		FClassViewerInitializationOptions Options;
		Options.Mode = EClassViewerMode::ClassPicker;

		// Only allow parenting to base blueprints.

		Options.bIsBlueprintBaseOnly = true;

		TSharedPtr<FCharacterRecipeBlueprintParentFilter> Filter{ MakeShareable(new FCharacterRecipeBlueprintParentFilter) };

		// All child child classes of UCharacterRecipe are valid.

		Filter->AllowedChildrenOfClasses.Add(UCharacterRecipe::StaticClass());
		Options.ClassFilters.Add(Filter.ToSharedRef());

		ParentClassContainer->ClearChildren();
		ParentClassContainer->AddSlot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("ParentClass", "Parent Class:"))
				.ShadowOffset(FVector2D(1.0f, 1.0f))
			];

		ParentClassContainer->AddSlot()
			[
				ClassViewerModule.CreateClassViewer(Options, FOnClassPicked::CreateSP(this, &SCharacterRecipeBlueprintCreateDialog::OnClassPicked))
			];
	}

	/** 
	 * Handler for when a parent class is selected 
	 */
	void OnClassPicked(UClass* ChosenClass)
	{
		ParentClass = ChosenClass;
	}

	/** 
	 * Handler for when ok is clicked 
	 */
	FReply OkClicked()
	{
		if (CharacterRecipeBlueprintFactory.IsValid())
		{
			CharacterRecipeBlueprintFactory->BlueprintType = BPTYPE_Normal;
			CharacterRecipeBlueprintFactory->ParentClass = ParentClass.Get();
		}

		CloseDialog(true);

		return FReply::Handled();
	}

	void CloseDialog(bool bWasPicked = false)
	{
		bOkClicked = bWasPicked;
		if (PickerWindow.IsValid())
		{
			PickerWindow.Pin()->RequestDestroyWindow();
		}
	}

	/** 
	 * Handler for when cancel is clicked 
	 */
	FReply CancelClicked()
	{
		CloseDialog();
		return FReply::Handled();
	}

	FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
	{
		if (InKeyEvent.GetKey() == EKeys::Escape)
		{
			CloseDialog();
			return FReply::Handled();
		}
		return SWidget::OnKeyDown(MyGeometry, InKeyEvent);
	}

private:
	/** The factory for which we are setting up properties */
	TWeakObjectPtr<UCharacterRecipeBlueprintFactory> CharacterRecipeBlueprintFactory;

	/** A pointer to the window that is asking the user to select a parent class */
	TWeakPtr<SWindow> PickerWindow;

	/** The container for the Parent Class picker */
	TSharedPtr<SVerticalBox> ParentClassContainer;

	/** The selected class */
	TWeakObjectPtr<UClass> ParentClass;

	/** True if Ok was clicked */
	bool bOkClicked;
};

END_SLATE_FUNCTION_BUILD_OPTIMIZATION


//////////////////////////////////////////////////////////////////////////////////
// UCharacterRecipeBlueprintFactory

UCharacterRecipeBlueprintFactory::UCharacterRecipeBlueprintFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UCharacterRecipeBlueprint::StaticClass();
	ParentClass = UCharacterRecipe::StaticClass();
}


bool UCharacterRecipeBlueprintFactory::ConfigureProperties()
{
	TSharedRef<SCharacterRecipeBlueprintCreateDialog> Dialog{ SNew(SCharacterRecipeBlueprintCreateDialog) };
	return Dialog->ConfigureProperties(this);
};

UObject* UCharacterRecipeBlueprintFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	// Make sure we are trying to factory a gameplay ability blueprint, then create and init one

	check(Class->IsChildOf(UCharacterRecipeBlueprint::StaticClass()));

	// If they selected an interface, force the parent class to be UInterface

	if (BlueprintType == BPTYPE_Interface)
	{
		ParentClass = UInterface::StaticClass();
	}

	if ((ParentClass == nullptr) || !FKismetEditorUtilities::CanCreateBlueprintOfClass(ParentClass) || !ParentClass->IsChildOf(UCharacterRecipe::StaticClass()))
	{
		FFormatNamedArguments Args;
		Args.Add( TEXT("ClassName"), (ParentClass != NULL) ? FText::FromString( ParentClass->GetName() ) : LOCTEXT("Null", "(null)") );
		FMessageDialog::Open( EAppMsgType::Ok, FText::Format( LOCTEXT("CannotCreateCharacterRecipeBlueprint", "Cannot create a CharacterRecipe Blueprint based on the class '{ClassName}'."), Args ) );
		return nullptr;
	}
	else
	{
		return FKismetEditorUtilities::CreateBlueprint(ParentClass, InParent, Name, BlueprintType, UCharacterRecipeBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass(), CallingContext);
	}
}

UObject* UCharacterRecipeBlueprintFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return FactoryCreateNew(Class, InParent, Name, Flags, Context, Warn, NAME_None);
}

#undef LOCTEXT_NAMESPACE

