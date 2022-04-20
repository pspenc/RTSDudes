// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataTable.h"
#include "VisualLogger/VisualLogger.h"




#define COLLISION_DAMAGE ECollisionChannel::ECC_EngineTraceChannel3



//Log for data related stuff, like initializing a unit from data table
DECLARE_LOG_CATEGORY_EXTERN(RTS_Log, Display, All);
DECLARE_LOG_CATEGORY_EXTERN(RTS_VLog, Display, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDataSystem, Display, All);
DECLARE_LOG_CATEGORY_EXTERN(VLogDataSystem, Display, All);
DECLARE_LOG_CATEGORY_EXTERN(LogStateSystem, Display, All);
DECLARE_LOG_CATEGORY_EXTERN(VLogStateSystem, Display, All);
DECLARE_LOG_CATEGORY_EXTERN(LogUnitSystem, Display, All);
DECLARE_LOG_CATEGORY_EXTERN(VLogUnitSystem, Display, All);

#if NO_LOGGING || !PLATFORM_DESKTOP

// Without logging enabled we pass ability system through to UE_LOG which only handles Fatal verbosity in NO_LOGGING
#define LOG(Verbosity, Format, ...) \
{ \
    UE_LOG(RTS_Log, Verbosity, Format, ##__VA_ARGS__); \
}

#define VLOG(Actor, Verbosity, Format, ...) \
{ \
    UE_LOG(RTS_VLog, Verbosity, Format, ##__VA_ARGS__); \
    UE_VLOG(Actor, RTS_VLog, Verbosity, Format, ##__VA_ARGS__); \
}
#define LOG_DATA(Verbosity, Format, ...) \
{ \
    UE_LOG(LogDataSystem, Verbosity, Format, ##__VA_ARGS__); \
}

#define VLOG_DATA(Actor, Verbosity, Format, ...) \
{ \
    UE_LOG(VLogDataSystem, Verbosity, Format, ##__VA_ARGS__); \
    UE_VLOG(Actor, VLogDataSystem, Verbosity, Format, ##__VA_ARGS__); \
}

#define LOG_UNIT(Verbosity, Format, ...) \
{ \
    UE_LOG(LogUnitSystem, Verbosity, Format, ##__VA_ARGS__); \
}

#define VLOG_UNIT(Actor, Verbosity, Format, ...) \
{ \
    UE_LOG(VLogUnitSystem, Verbosity, Format, ##__VA_ARGS__); \
    UE_VLOG(Actor, VLogUnitSystem, Verbosity, Format, ##__VA_ARGS__); \
}

#define LOG_STATE(Verbosity, Format, ...) \
{ \
    UE_LOG(LogStateSystem, Verbosity, Format, ##__VA_ARGS__); \
}

#define VLOG_STATE(Actor, Verbosity, Format, ...) \
{ \
    UE_LOG(VLogStateSystem, Verbosity, Format, ##__VA_ARGS__); \
    UE_VLOG(Actor, VLogStateSystem, Verbosity, Format, ##__VA_ARGS__); \
}

#else

#define LOG(Verbosity, Format, ...) \
{ \
    UE_LOG(RTS_Log, Verbosity, Format, ##__VA_ARGS__); \
}

#define VLOG(Actor, Verbosity, Format, ...) \
{ \
    UE_LOG(RTS_VLog, Verbosity, Format, ##__VA_ARGS__); \
    UE_VLOG(Actor, RTS_VLog, Verbosity, Format, ##__VA_ARGS__); \
}


#define LOG_DATA(Verbosity, Format, ...) \
{ \
    UE_LOG(LogDataSystem, Verbosity, Format, ##__VA_ARGS__); \
}

#define VLOG_DATA(Actor, Verbosity, Format, ...) \
{ \
    UE_LOG(VLogDataSystem, Verbosity, Format, ##__VA_ARGS__); \
    UE_VLOG(Actor, VLogDataSystem, Verbosity, Format, ##__VA_ARGS__); \
}

#define LOG_UNIT(Verbosity, Format, ...) \
{ \
    UE_LOG(LogUnitSystem, Verbosity, Format, ##__VA_ARGS__); \
}

#define VLOG_UNIT(Actor, Verbosity, Format, ...) \
{ \
    UE_LOG(VLogUnitSystem, Verbosity, Format, ##__VA_ARGS__); \
    UE_VLOG(Actor, VLogUnitSystem, Verbosity, Format, ##__VA_ARGS__); \
}

#define LOG_STATE(Verbosity, Format, ...) \
{ \
    UE_LOG(LogStateSystem, Verbosity, Format, ##__VA_ARGS__); \
}

#define VLOG_STATE(Actor, Verbosity, Format, ...) \
{ \
    UE_LOG(VLogStateSystem, Verbosity, Format, ##__VA_ARGS__); \
    UE_VLOG(Actor, VLogStateSystem, Verbosity, Format, ##__VA_ARGS__); \
}

#endif //NO_LOGGING

FORCEINLINE FString GetBoolText(const bool bCondition)
{
	return bCondition ? TEXT("true") : TEXT("false");
}
