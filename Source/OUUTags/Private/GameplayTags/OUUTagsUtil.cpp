// Copyright (c) 2026 Jonas Reich & Contributors

#include "GameplayTags/OUUTagsUtil.h"

#include "GameplayTagsManager.h"
#include "OUUGameplayTagStealer.h"

//----------------------------------------------------------------------------------------------------------------------
// Utilities
//----------------------------------------------------------------------------------------------------------------------
namespace
{
//----------------------------------------------------------------------------------------------------------------------
bool (*GetFilterFunction(const FGameplayTagContainer& Whitelist,
						 const FGameplayTagContainer& Blacklist,
						 const bool ExactMatchesOnly))(const FGameplayTag&,
													   const FGameplayTagContainer&,
													   const FGameplayTagContainer&)
{
	static const auto FilterByWhitelist = [](const FGameplayTag& Tag,
											 const FGameplayTagContainer& Whitelist,
											 const FGameplayTagContainer& Blacklist) -> bool {
		return Tag.MatchesAny(Whitelist);
	};
	static const auto FilterByWhitelistExact = [](const FGameplayTag& Tag,
												  const FGameplayTagContainer& Whitelist,
												  const FGameplayTagContainer& Blacklist) -> bool {
		return Tag.MatchesAnyExact(Whitelist);
	};
	static const auto FilterByBlacklist = [](const FGameplayTag& Tag,
											 const FGameplayTagContainer& Whitelist,
											 const FGameplayTagContainer& Blacklist) -> bool {
		return Tag.MatchesAny(Blacklist) == false;
	};
	static const auto FilterByBlacklistExact = [](const FGameplayTag& Tag,
												  const FGameplayTagContainer& Whitelist,
												  const FGameplayTagContainer& Blacklist) -> bool {
		return Tag.MatchesAnyExact(Blacklist) == false;
	};
	static const auto FilterAll = [](const FGameplayTag& Tag,
									 const FGameplayTagContainer& Whitelist,
									 const FGameplayTagContainer& Blacklist) -> bool {
		return Tag.MatchesAny(Whitelist) && Tag.MatchesAny(Blacklist) == false;
	};
	static const auto FilterAllExact = [](const FGameplayTag& Tag,
										  const FGameplayTagContainer& Whitelist,
										  const FGameplayTagContainer& Blacklist) -> bool {
		return Tag.MatchesAnyExact(Whitelist) && Tag.MatchesAnyExact(Blacklist) == false;
	};

	if (Whitelist.IsEmpty())
	{
		if (Blacklist.IsEmpty())
		{
			return nullptr;
		}

		return ExactMatchesOnly ? FilterByBlacklistExact : FilterByBlacklist;
	}

	if (Blacklist.IsEmpty())
	{
		return ExactMatchesOnly ? FilterByWhitelistExact : FilterByWhitelist;
	}

	return ExactMatchesOnly ? FilterAllExact : FilterAll;
}

//----------------------------------------------------------------------------------------------------------------------
template <bool IsStable>
FORCEINLINE void FilterInPlaceInternal(FGameplayTagContainer& ContainerToFilter,
									   const FGameplayTagContainer& Whitelist,
									   const FGameplayTagContainer& Blacklist,
									   const bool ExactMatchesOnly)
{
	const auto FilterFunc = GetFilterFunction(Whitelist, Blacklist, ExactMatchesOnly);
	if (FilterFunc == nullptr)
	{
		return;
	}

	FGameplayTagContainerStealer& Stealer = static_cast<FGameplayTagContainerStealer&>(ContainerToFilter);
	bool AnyRemoved = false;
	// Iterate in reverse, this allows us to copy around fewer gameplay tags if there are many removals.
	for (int32 i = Stealer.StealTags().Num() - 1; i >= 0; --i)
	{
		if ((*FilterFunc)(Stealer.StealTags()[i], Whitelist, Blacklist) == false)
		{
			if constexpr (IsStable)
			{
				Stealer.StealTags().RemoveAt(i, EAllowShrinking::No);
			}
			else
			{
				Stealer.StealTags().RemoveAtSwap(i, EAllowShrinking::No);
			}
			AnyRemoved = true;
		}
	}

	if (AnyRemoved)
	{
		ContainerToFilter.FillParentTags();
	}
}

} // namespace

namespace OUUTags::Util
{
#if WITH_EDITOR
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
#endif

	//------------------------------------------------------------------------------------------------------------------
	void AppendToEmptyContainer(FGameplayTagContainer& ContainerToAppendTo,
								const FGameplayTagContainer& TagsToAppend)
	{
		checkf(ContainerToAppendTo.IsEmpty(),
			   TEXT("AppendToEmptyContainer must only be called on empty containers, duh!"));

		auto& SourceStealer = static_cast<const FGameplayTagContainerStealer&>(TagsToAppend);
		auto& TargetStealer = static_cast<FGameplayTagContainerStealer&>(ContainerToAppendTo);

		// Directly append tags to the target container. This is faster than calling AppendTags since we know the target
		// is empty so we can avoid checking for duplicates.
		TargetStealer.StealTags().Append(SourceStealer.StealTags());
		TargetStealer.StealParentTags().Append(SourceStealer.StealParentTags());
	}

	//------------------------------------------------------------------------------------------------------------------
	void FilterInPlace(FGameplayTagContainer& ContainerToFilter,
					   const FGameplayTagContainer& Whitelist,
					   const FGameplayTagContainer& Blacklist,
					   const bool ExactMatchesOnly)
	{
		FilterInPlaceInternal<false>(ContainerToFilter, Whitelist, Blacklist, ExactMatchesOnly);
	}

	//------------------------------------------------------------------------------------------------------------------
	void FilterInPlaceStable(FGameplayTagContainer& ContainerToFilter,
							 const FGameplayTagContainer& Whitelist,
							 const FGameplayTagContainer& Blacklist,
							 const bool ExactMatchesOnly)
	{
		FilterInPlaceInternal<true>(ContainerToFilter, Whitelist, Blacklist, ExactMatchesOnly);
	}

	//------------------------------------------------------------------------------------------------------------------
	void AppendFilteredTags(FGameplayTagContainer& ContainerToAppendTo,
							const FGameplayTagContainer& TagsToAppend,
							const FGameplayTagContainer& Whitelist,
							const FGameplayTagContainer& Blacklist,
							const bool ExactMatchesOnly)
	{
		const auto FilterFunc = GetFilterFunction(Whitelist, Blacklist, ExactMatchesOnly);
		if (FilterFunc == nullptr)
		{
			OUUTags::Util::AppendToContainer(ContainerToAppendTo, TagsToAppend);
			return;
		}

		if (ContainerToAppendTo.IsEmpty())
		{
			// If the container to append to is empty, we can use AddTagFast since we can assume that TagsToAppend
			// contains no duplicate tags.
			for (const auto& Tag : TagsToAppend)
			{
				if ((*FilterFunc)(Tag, Whitelist, Blacklist))
				{
					ContainerToAppendTo.AddTagFast(Tag);
				}
			}
		}
		else
		{
			for (const auto& Tag : TagsToAppend)
			{
				if ((*FilterFunc)(Tag, Whitelist, Blacklist))
				{
					ContainerToAppendTo.AddTag(Tag);
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	const FGameplayTagContainer& GetSingleTagContainer(const FGameplayTag& Tag)
	{
		// The code below is basically a copy of UGameplayTagsManager::GetSingleTagContainerPtr.
		auto& Manager = UGameplayTagsManager::Get();
		auto Node = Manager.FindTagNode(Tag);
		if (Node)
		{
			return Node->GetSingleTagContainer();
		}

#if WITH_EDITOR
		// Check redirector
		if (GIsEditor && Tag.IsValid())
		{
			FGameplayTag RedirectedTag = Tag;

			Manager.RedirectSingleGameplayTag(RedirectedTag, nullptr);

			Node = Manager.FindTagNode(RedirectedTag);

			if (Node)
			{
				return Node->GetSingleTagContainer();
			}
		}
#endif

		return FGameplayTagContainer::EmptyContainer;
	}
} // namespace OUUTags::Util
