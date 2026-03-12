// Copyright (c) 2026 Jonas Reich & Contributors

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

namespace OUUTags::Util
{
#if WITH_EDITOR
	OUUTAGS_API FString MakeFilterStringFromContainer(const FGameplayTagContainer& GameplayTags);
#endif
} // namespace OUUTags::Util
