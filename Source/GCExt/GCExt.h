// Copyright (C) 2024 owoDra

#pragma once

#include "Modules/ModuleManager.h"

/**
 *  Modules for the main features of the GameCharacterCore plugin
 */
class FGCExtModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

};
