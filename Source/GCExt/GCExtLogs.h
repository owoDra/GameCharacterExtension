// Copyright (C) 2024 owoDra

#pragma once

#include "Logging/LogMacros.h"

GCEXT_API DECLARE_LOG_CATEGORY_EXTERN(LogGCE, Log, All);

#if !UE_BUILD_SHIPPING

#define GCELOG(FormattedText, ...) UE_LOG(LogGCE, Log, FormattedText, __VA_ARGS__)

#define GCEENSURE(InExpression) ensure(InExpression)
#define GCEENSURE_MSG(InExpression, InFormat, ...) ensureMsgf(InExpression, InFormat, __VA_ARGS__)
#define GCEENSURE_ALWAYS_MSG(InExpression, InFormat, ...) ensureAlwaysMsgf(InExpression, InFormat, __VA_ARGS__)

#define GCECHECK(InExpression) check(InExpression)
#define GCECHECK_MSG(InExpression, InFormat, ...) checkf(InExpression, InFormat, __VA_ARGS__)

#else

#define GCELOG(FormattedText, ...)

#define GCEENSURE(InExpression) InExpression
#define GCEENSURE_MSG(InExpression, InFormat, ...) InExpression
#define GCEENSURE_ALWAYS_MSG(InExpression, InFormat, ...) InExpression

#define GCECHECK(InExpression) InExpression
#define GCECHECK_MSG(InExpression, InFormat, ...) InExpression

#endif