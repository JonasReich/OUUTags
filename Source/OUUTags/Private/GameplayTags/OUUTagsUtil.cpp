// Copyright (c) 2026 Jonas Reich & Contributors

#include "GameplayTags/OUUTagsUtil.h"

namespace OUUTags::Util
{
	FString MakeFilterStringFromContainer(const FGameplayTagContainer& GameplayTags)
	{
		FString Result;
		int i = 0;
		for (auto& Tag : GameplayTags)
		{
			Result += Tag.ToString();
			if (i < GameplayTags.Num() - 1)
			{
				Result += TEXT(",");
			}
			++i;
		}
		return Result;
	}
} // namespace OUUTags::Util
