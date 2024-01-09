// Copyright (C) 2024 owoDra

#include "CharacterModifier.h"

#include "GCExtLogs.h"

#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterModifier)


bool UCharacterModifier::OnApply(APawn* Pawn) const
{
	check(Pawn);

	const auto bServer{ Pawn->HasAuthority() };
	const auto bClient{ !bServer };
	const auto bLocal{ Pawn->IsLocallyControlled() };

	UE_LOG(LogGCE, Log, TEXT("[%s | %s] bOnlyApplyLocal: %s | bApplyOnServer: %s | bApplyOnClient: %s | On Instance Apply(%s)"),
		bServer ? TEXT("SERVER") : TEXT("CLIENT"),
		bLocal ? TEXT("LOCAL") : TEXT("SIMU"),
		bOnlyApplyOnLocal ? TEXT("TRUE") : TEXT("FALSE"), 
		bApplyOnServer ? TEXT("TRUE") : TEXT("FALSE"),
		bApplyOnClient ? TEXT("TRUE") : TEXT("FALSE"),
		*GetNameSafe(this));

	if (bOnlyApplyOnLocal && bLocal)
	{
		UE_LOG(LogGCE, Log, TEXT("-> Only Apply On Local"));

		return true;
	}
	else if (!bOnlyApplyOnLocal && bApplyOnClient && bClient)
	{
		UE_LOG(LogGCE, Log, TEXT("-> Apply On Client"));

		return true;
	}
	else if (!bOnlyApplyOnLocal && bApplyOnServer && bServer)
	{
		UE_LOG(LogGCE, Log, TEXT("-> Apply On Server"));

		return true;
	}

	// UE_LOG(LogGCE, Log, TEXT("-x It will not apply on this role"));

	return false;
}
