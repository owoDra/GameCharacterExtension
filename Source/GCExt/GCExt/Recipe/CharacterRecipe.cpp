// Copyright (C) 2024 owoDra

#include "Recipe/CharacterRecipe.h"

#include "CharacterInitStateComponent.h"
#include "GCExtLogs.h"

#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterRecipe)


UCharacterRecipe::UCharacterRecipe(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UCharacterRecipe::HandleStartSetup(const FCharacterRecipePawnInfo& Info)
{
	check(Info.Handle.IsValid());
	check(Info.Pawn.IsValid());
	check(Info.InitStateComponent.IsValid());
	check(!HasAllFlags(RF_ClassDefaultObject));

	PawnInfo = Info;

	UE_LOG(LogGameExt_CharacterRecipe, Log, TEXT("| [%s][Instanced] Start Setup (%s)"), *Info.Handle.ToString(), *GetNameSafe(this));

	StartSetup(PawnInfo);
}

void UCharacterRecipe::HandleDestroy()
{
	UE_LOG(LogGameExt_CharacterRecipe, Log, TEXT("| [%s][Instanced] Destroy (%s)"), *PawnInfo.Handle.ToString(), *GetNameSafe(this));

	OnDestroy();
}

void UCharacterRecipe::FinishSetup()
{
	UE_LOG(LogGameExt_CharacterRecipe, Log, TEXT("| [%s][Instanced] Finish Setup (%s)"), *PawnInfo.Handle.ToString(), *GetNameSafe(this));

	if (PawnInfo.InitStateComponent.IsValid())
	{
		PawnInfo.InitStateComponent->HandleRecipeSetupFinished(PawnInfo.Handle);
	}
}


APawn* UCharacterRecipe::GetTypedPawn(TSubclassOf<APawn> InClass) const
{
	return PawnInfo.Pawn.Get();
}


void UCharacterRecipe::HandleStartSetupNonInstanced(const FCharacterRecipePawnInfo& Info) const
{
	check(Info.Handle.IsValid());
	check(Info.Pawn.IsValid());
	check(Info.InitStateComponent.IsValid());
	check(HasAllFlags(RF_ClassDefaultObject));

	UE_LOG(LogGameExt_CharacterRecipe, Log, TEXT("| [%s][NonInstanced] Start Setup (%s)"), *Info.Handle.ToString(), *GetNameSafe(this));

	StartSetupNonInstanced(Info);

	Info.InitStateComponent->HandleRecipeSetupFinished(Info.Handle);
}
