// Copyright (c) 2026 Jonas Reich & Contributors

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

struct FGameplayTag;
struct FGameplayTagContainer;

//----------------------------------------------------------------------------------------------------------------------

// Utility for performantly adding a gameplay tag (or multiple) to a container and removing it again at the end of the
// scope.
// WARNING: When using this, the container MUST ONLY be added to within the scope. Removing gameplay tags is NOT
// allowed!
struct OUUTAGS_API FOUUScopedAddGameplayTag
{
public:
	// Open a scope where tags may be added to the container. The adds will be undone at the end of the scope.
	FOUUScopedAddGameplayTag(FGameplayTagContainer& InContainer);
	// Open an add scope and immediately add the given tag.
	FOUUScopedAddGameplayTag(FGameplayTagContainer& InContainer, const FGameplayTag& Tag);
	// Open an add scope and immediately add the given tags.
	FOUUScopedAddGameplayTag(FGameplayTagContainer& InContainer, const FGameplayTagContainer& Tags);
	FOUUScopedAddGameplayTag(const FOUUScopedAddGameplayTag& Other) = delete;
	FOUUScopedAddGameplayTag(FOUUScopedAddGameplayTag&& Other) = delete;
	~FOUUScopedAddGameplayTag();

private:
	template <typename InitFuncT>
	FOUUScopedAddGameplayTag(FGameplayTagContainer& InContainer, const InitFuncT& InitFunc);

private:
	FGameplayTagContainer& Container;
	int32 PrevNumTags;
	int32 PrevNumParentTags;
#if DO_CHECK
	int32 NewNumTags;
	int32 NewNumParentTags;
#endif
};
