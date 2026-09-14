// Copyright (c) 2026 Jonas Reich & Contributors

#pragma once

#include "GameplayTagContainer.h"

struct FGameplayTagContainerStealer : public FGameplayTagContainer
{
	FGameplayTagContainerStealer() = delete;
	FGameplayTagContainerStealer(FGameplayTagContainer&&) = delete;
	FGameplayTagContainerStealer(const FGameplayTagContainer&) = delete;
	
	FORCEINLINE TArray<FGameplayTag>& StealTags() { return GameplayTags; }
	FORCEINLINE const TArray<FGameplayTag>& StealTags() const { return GameplayTags; }
	FORCEINLINE TArray<FGameplayTag>& StealParentTags() { return ParentTags; }
	FORCEINLINE const TArray<FGameplayTag>& StealParentTags() const { return ParentTags; }
};
