// Copyright (C) 2024 owoDra

#include "PendingCharacterRecipeHandle.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PendingCharacterRecipeHandle)


void FPendingCharacterRecipeHandle::GenerateNewHandle()
{
	// Must be in C++ to avoid duplicate statics accross execution units

	static int32 GHandle{ 1 };
	Handle = GHandle++;
}
