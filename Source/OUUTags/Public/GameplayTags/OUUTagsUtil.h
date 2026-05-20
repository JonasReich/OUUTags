// Copyright (c) 2026 Jonas Reich & Contributors

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

namespace OUUTags::Util
{
#if WITH_EDITOR
	OUUTAGS_API FString MakeFilterStringFromContainer(const FGameplayTagContainer& GameplayTags);
#endif

	// Copy the contents of TagsToAssign to ContainerToAssignTo. Unlike the default operator=, this will not reset the
	// existing memory allocation of ContainerToAssignTo.
	OUUTAGS_API void AssignToContainer(FGameplayTagContainer& ContainerToAssignTo,
									   const FGameplayTagContainer& TagsToAssign);

	// Like FGameplayTagContainer::AppendTags, but uses optimized AppendToEmptyContainer call if the container to append
	// to is empty.
	OUUTAGS_API void AppendToContainer(FGameplayTagContainer& ContainerToAppendTo,
									   const FGameplayTagContainer& TagsToAppend);

	// Optimized version of FGameplayTagContainer::AppendTags for containers which are known to be empty.
	OUUTAGS_API void AppendToEmptyContainer(FGameplayTagContainer& ContainerToAppendTo,
											const FGameplayTagContainer& TagsToAppend);

	// Filter the contents of the given container in-place without creating a new copy. Note: This can change the order
	// of tags in the container.
	OUUTAGS_API void FilterInPlace(FGameplayTagContainer& ContainerToFilter,
								   const FGameplayTagContainer& Whitelist,
								   const FGameplayTagContainer& Blacklist,
								   const bool ExactMatchesOnly = false);
	// Like FilterInPlace, but does not change the order of non-removed tags. May be slightly slower.
	OUUTAGS_API void FilterInPlaceStable(FGameplayTagContainer& ContainerToFilter,
										 const FGameplayTagContainer& Whitelist,
										 const FGameplayTagContainer& Blacklist,
										 const bool ExactMatchesOnly = false);
	// Append only tags matching the given filter to the container.
	OUUTAGS_API void AppendFilteredTags(FGameplayTagContainer& ContainerToAppendTo,
										const FGameplayTagContainer& TagsToAppend,
										const FGameplayTagContainer& Whitelist,
										const FGameplayTagContainer& Blacklist,
										const bool ExactMatchesOnly = false);

	// Get a preconstructed container instance containing only the given gameplay tag.
	// Unlike FGameplayTag::GetSingleTagContainer, this function does not create an unnecessary copy of the container.
	// Note: The returned container reference will be invalidated in DoneAddingNativeTags and when gameplay tags are
	// edited in the editor.
	OUUTAGS_API const FGameplayTagContainer& GetSingleTagContainer(const FGameplayTag& Tag);
} // namespace OUUTags::Util

FORCEINLINE void OUUTags::Util::AssignToContainer(FGameplayTagContainer& ContainerToAssignTo,
												  const FGameplayTagContainer& TagsToAssign)
{
	ContainerToAssignTo.Reset();
	AppendToEmptyContainer(ContainerToAssignTo, TagsToAssign);
}

FORCEINLINE void OUUTags::Util::AppendToContainer(FGameplayTagContainer& ContainerToAppendTo,
												  const FGameplayTagContainer& TagsToAppend)
{
	if (ContainerToAppendTo.IsEmpty())
	{
		AppendToEmptyContainer(ContainerToAppendTo, TagsToAppend);
	}
	else
	{
		ContainerToAppendTo.AppendTags(TagsToAppend);
	}
}
