// Copyright (c) 2026 Jonas Reich & Contributors

#include "GameplayTags/OUUScopedAddGameplayTag.h"
#include "OUUGameplayTagStealer.h"

//----------------------------------------------------------------------------------------------------------------------
// FOUUScopedAddGameplayTag
//----------------------------------------------------------------------------------------------------------------------
FOUUScopedAddGameplayTag::FOUUScopedAddGameplayTag(FGameplayTagContainer& InContainer)
	: FOUUScopedAddGameplayTag(InContainer, [&]() {})
{
}

//----------------------------------------------------------------------------------------------------------------------
FOUUScopedAddGameplayTag::FOUUScopedAddGameplayTag(FGameplayTagContainer& InContainer, const FGameplayTag& Tag)
	: FOUUScopedAddGameplayTag(InContainer, [&]() { InContainer.AddTag(Tag); })
{
}

//----------------------------------------------------------------------------------------------------------------------
FOUUScopedAddGameplayTag::FOUUScopedAddGameplayTag(FGameplayTagContainer& InContainer,
												   const FGameplayTagContainer& Tags)
	: FOUUScopedAddGameplayTag(InContainer, [&]() { InContainer.AppendTags(Tags); })
{
}

//----------------------------------------------------------------------------------------------------------------------
FOUUScopedAddGameplayTag::~FOUUScopedAddGameplayTag()
{
	auto& Stealer = static_cast<FGameplayTagContainerStealer&>(Container);

#if DO_CHECK
	checkf(Stealer.StealTags().Num() >= NewNumTags && Stealer.StealParentTags().Num() >= NewNumParentTags,
		   TEXT("Elements were removed from the gameplay tag container while within FOUUScopedAddGameplayTag, this is "
				"not allowed!"));
#endif

	// New tags and parent tags will always be added to the back of these arrays, so we can just reset them to the old
	// size in order to remove the added tags, no need for expensive lookups or refilling the parent tag array.
	Stealer.StealTags().SetNum(PrevNumTags, EAllowShrinking::No);
	Stealer.StealParentTags().SetNum(PrevNumParentTags, EAllowShrinking::No);
}

//----------------------------------------------------------------------------------------------------------------------
template <typename InitFuncT>
FORCEINLINE FOUUScopedAddGameplayTag::FOUUScopedAddGameplayTag(FGameplayTagContainer& InContainer,
															   const InitFuncT& InitFunc)
	: Container(InContainer)
{
	auto& Stealer = static_cast<FGameplayTagContainerStealer&>(Container);
	PrevNumTags = Stealer.StealTags().Num();
	PrevNumParentTags = Stealer.StealParentTags().Num();

	InitFunc();

#if DO_CHECK
	NewNumTags = Stealer.StealTags().Num();
	NewNumParentTags = Stealer.StealParentTags().Num();
#endif
}
