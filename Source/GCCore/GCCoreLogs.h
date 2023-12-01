// Copyright (C) 2023 owoDra

#pragma once

#include "Logging/LogMacros.h"

GCCORE_API DECLARE_LOG_CATEGORY_EXTERN(LogGCC, Log, All);

#if !UE_BUILD_SHIPPING

#define GCCLOG(FormattedText, ...) UE_LOG(LogGCC, Log, FormattedText, __VA_ARGS__)

#define GCCENSURE(InExpression) ensure(InExpression)
#define GCCENSURE_MSG(InExpression, InFormat, ...) ensureMsgf(InExpression, InFormat, __VA_ARGS__)
#define GCCENSURE_ALWAYS_MSG(InExpression, InFormat, ...) ensureAlwaysMsgf(InExpression, InFormat, __VA_ARGS__)

#define GCCCHECK(InExpression) check(InExpression)
#define GCCCHECK_MSG(InExpression, InFormat, ...) checkf(InExpression, InFormat, __VA_ARGS__)

#else

#define GCCLOG(FormattedText, ...)

#define GCCENSURE(InExpression) InExpression
#define GCCENSURE_MSG(InExpression, InFormat, ...) InExpression
#define GCCENSURE_ALWAYS_MSG(InExpression, InFormat, ...) InExpression

#define GCCCHECK(InExpression) InExpression
#define GCCCHECK_MSG(InExpression, InFormat, ...) InExpression

#endif