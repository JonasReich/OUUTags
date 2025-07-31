// Copyright (c) 2023 Jonas Reich & Contributors

#include "GameplayTags/OUUGameplayTagLibrary.h"

#include "GameplayTagsManager.h"

FGameplayTag UOUUGameplayTagLibrary::GetParentTag(const FGameplayTag& Tag)
{
	return UGameplayTagsManager::Get().RequestGameplayTagDirectParent(Tag);
}

FGameplayTagContainer UOUUGameplayTagLibrary::GetChildTags(const FGameplayTag& Tag, int32 MaxRelativeTagDepth)
{
	auto Children = UGameplayTagsManager::Get().RequestGameplayTagChildren(Tag);
	if (MaxRelativeTagDepth <= 0)
	{
		return Children;
	}

	const auto RootDepth = GetTagDepth(Tag);
	FGameplayTagContainer FilteredResult;
	for (auto& ChildTag : Children)
	{
		const int32 RelativeChildDepth = GetTagDepth(ChildTag) - RootDepth;
		if (RelativeChildDepth <= MaxRelativeTagDepth)
		{
			FilteredResult.AddTag(ChildTag);
		}
	}
	return FilteredResult;
}

TArray<FGameplayTag> UOUUGameplayTagLibrary::GetAllTagsInContainer(const FGameplayTagContainer& TagContainer)
{
	TArray<FGameplayTag> Result;
	TagContainer.GetGameplayTagArray(OUT Result);
	return Result;
}

FGameplayTagContainer UOUUGameplayTagLibrary::CreateTagContainerFromArray(const TArray<FGameplayTag>& TagArray)
{
	return FGameplayTagContainer::CreateFromArray(TagArray);
}

int32 UOUUGameplayTagLibrary::GetTagDepth(const FGameplayTag& Tag)
{
	return GetTagComponents(Tag).Num();
}

FGameplayTag UOUUGameplayTagLibrary::GetTagUntilDepth(const FGameplayTag& Tag, int32 Depth)
{
	TArray<FName> Names = GetTagComponents(Tag);
	if (Depth > Names.Num())
	{
		return FGameplayTag::EmptyTag;
	}
	Names.SetNum(Depth, true);
	return CreateTagFromComponents(Names);
}

TArray<FName> UOUUGameplayTagLibrary::GetTagComponents(const FGameplayTag& Tag)
{
	TArray<FName> Names;
	UGameplayTagsManager::Get().SplitGameplayTagFName(Tag, OUT Names);
	return Names;
}

FGameplayTag UOUUGameplayTagLibrary::CreateTagFromComponents(const TArray<FName>& TagComponents)
{
	const auto TagString =
		FString::JoinBy(TagComponents, TEXT("."), [](const FName& Name) -> FString { return Name.ToString(); });
	return UGameplayTagsManager::Get().RequestGameplayTag(*TagString, false);
}
