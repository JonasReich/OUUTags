// Copyright (c) 2026 Jonas Reich & Contributors

#pragma once

#include "CoreMinimal.h"

#include "Concepts/ConvertibleTo.h"
#include "GameplayTagContainer.h"

struct FGameplayTag;
struct FGameplayTagContainer;

namespace OUUTags::Private
{
template <typename T>
void GetUnderlyingTagTypeHelper();
template <typename T>
requires(UE::CConvertibleTo<const T&, const FGameplayTag&>) FGameplayTag GetUnderlyingTagTypeHelper();
template <typename T>
requires(UE::CConvertibleTo<const T&, const FGameplayTagContainer&>) FGameplayTagContainer GetUnderlyingTagTypeHelper();

template <typename T>
using TUnderlyingTagType = decltype(GetUnderlyingTagTypeHelper<T>());
} // namespace OUUTags::Private

//----------------------------------------------------------------------------------------------------------------------

// Abstract base interface for TOUUGameplayTagCollection.
// Use this if you need to pass an arbitrary collection to another function.
struct FOUUGameplayTagCollection
{
public:
	virtual bool IsEmpty() const = 0;
	virtual bool HasTag(const FGameplayTag& TagToCheck) const = 0;
	virtual bool HasTagExact(const FGameplayTag& TagToCheck) const = 0;
	virtual bool HasAny(const FGameplayTagContainer& ContainerToCheck) const = 0;
	virtual bool HasAnyExact(const FGameplayTagContainer& ContainerToCheck) const = 0;
	virtual bool HasAll(const FGameplayTagContainer& ContainerToCheck) const = 0;
	virtual bool HasAllExact(const FGameplayTagContainer& ContainerToCheck) const = 0;
	// Equivalent To ContainerToCheck.HasAny(Collection).
	virtual bool ContainerHasAnyTagInCollection(const FGameplayTagContainer& ContainerToCheck) const = 0;
	// Equivalent To ContainerToCheck.HasAnyExact(Collection).
	virtual bool ContainerHasAnyTagInCollectionExact(const FGameplayTagContainer& ContainerToCheck) const = 0;
	// Equivalent To ContainerToCheck.HasAll(Collection).
	virtual bool ContainerHasAllTagsInCollection(const FGameplayTagContainer& ContainerToCheck) const = 0;
	// Equivalent To ContainerToCheck.HasAllExact(Collection).
	virtual bool ContainerHasAllTagsInCollectionExact(const FGameplayTagContainer& ContainerToCheck) const = 0;
	// Create a gameplay tag container containing all tags in the collection.
	// Note: This allocates a new container. Prefer using AppendToContainer instead.
	virtual FGameplayTagContainer ToContainer() const = 0;
	// Append all tags in the collection to the given container.
	virtual void AppendToContainer(FGameplayTagContainer& Container) const = 0;
};

//----------------------------------------------------------------------------------------------------------------------
template <typename... TagTs>
struct TOUUGameplayTagCollectionBase : public FOUUGameplayTagCollection
{
	static_assert(sizeof...(TagTs) == 0,
				  TEXT("If this triggers, you tried to pass something to this collection which is neither a gameplay "
					   "tag nor a gameplay tag container."));

public:
	TOUUGameplayTagCollectionBase() = default;

public:
	bool CollectionHasExactTag(const FGameplayTag& Tag) const;
	bool CollectionHasTag(const FGameplayTag& Tag) const;
	bool TagMatchesCollectionExact(const FGameplayTag& Tag) const;
	bool TagMatchesCollection(const FGameplayTag& Tag) const;
	template <typename FuncT>
	bool ForEachTag(const FuncT& Func) const;

	// -- FOUUGameplayTagCollection interface
public:
	bool IsEmpty() const override;
	void AppendToContainer(FGameplayTagContainer& Container) const override;
};

template <typename... TagTs>
FORCEINLINE bool TOUUGameplayTagCollectionBase<TagTs...>::CollectionHasExactTag(const FGameplayTag& Tag) const
{
	return false;
}
template <typename... TagTs>
FORCEINLINE bool TOUUGameplayTagCollectionBase<TagTs...>::CollectionHasTag(const FGameplayTag& Tag) const
{
	return false;
}
template <typename... TagTs>
FORCEINLINE bool TOUUGameplayTagCollectionBase<TagTs...>::TagMatchesCollectionExact(const FGameplayTag& Tag) const
{
	return false;
}
template <typename... TagTs>
FORCEINLINE bool TOUUGameplayTagCollectionBase<TagTs...>::TagMatchesCollection(const FGameplayTag& Tag) const
{
	return false;
}
template <typename... TagTs>
template <typename FuncT>
FORCEINLINE bool TOUUGameplayTagCollectionBase<TagTs...>::ForEachTag(const FuncT& Func) const
{
	return false;
}
template <typename... TagTs>
FORCEINLINE bool TOUUGameplayTagCollectionBase<TagTs...>::IsEmpty() const
{
	return true;
}
template <typename... TagTs>
FORCEINLINE void TOUUGameplayTagCollectionBase<TagTs...>::AppendToContainer(FGameplayTagContainer& Container) const
{
}

//----------------------------------------------------------------------------------------------------------------------

template <typename... TagTs>
struct TOUUGameplayTagCollectionBase<FGameplayTag, TagTs...> : public TOUUGameplayTagCollectionBase<TagTs...>
{
public:
	using Super = TOUUGameplayTagCollectionBase<TagTs...>;

public:
	TOUUGameplayTagCollectionBase(const FGameplayTag& InTag, const TagTs&... Tags);

public:
	bool CollectionHasExactTag(const FGameplayTag& InTag) const;
	bool CollectionHasTag(const FGameplayTag& InTag) const;
	bool TagMatchesCollectionExact(const FGameplayTag& InTag) const;
	bool TagMatchesCollection(const FGameplayTag& InTag) const;
	template <typename FuncT>
	bool ForEachTag(const FuncT& Func) const;

	// -- FOUUGameplayTagCollection interface
public:
	bool IsEmpty() const override;
	void AppendToContainer(FGameplayTagContainer& Container) const override;

private:
	const FGameplayTag& Tag;
};

template <typename... TagTs>
FORCEINLINE TOUUGameplayTagCollectionBase<FGameplayTag, TagTs...>::TOUUGameplayTagCollectionBase(
	const FGameplayTag& InTag,
	const TagTs&... Tags)
	: Super(Tags...)
	, Tag(InTag)
{
}
template <typename... TagTs>
FORCEINLINE bool TOUUGameplayTagCollectionBase<FGameplayTag, TagTs...>::CollectionHasExactTag(
	const FGameplayTag& InTag) const
{
	return Tag.MatchesTagExact(InTag) || Super::CollectionHasExactTag(InTag);
}
template <typename... TagTs>
FORCEINLINE bool TOUUGameplayTagCollectionBase<FGameplayTag, TagTs...>::CollectionHasTag(const FGameplayTag& InTag) const
{
	return Tag.MatchesTag(InTag) || Super::CollectionHasTag(InTag);
}
template <typename... TagTs>
FORCEINLINE bool TOUUGameplayTagCollectionBase<FGameplayTag, TagTs...>::TagMatchesCollectionExact(
	const FGameplayTag& InTag) const
{
	return InTag.MatchesTagExact(Tag) || Super::TagMatchesCollectionExact(InTag);
}

template <typename... TagTs>
FORCEINLINE bool TOUUGameplayTagCollectionBase<FGameplayTag, TagTs...>::TagMatchesCollection(
	const FGameplayTag& InTag) const
{
	return InTag.MatchesTag(Tag) || Super::TagMatchesCollection(InTag);
}
template <typename... TagTs>
template <typename FuncT>
FORCEINLINE bool TOUUGameplayTagCollectionBase<FGameplayTag, TagTs...>::ForEachTag(const FuncT& Func) const
{
	return (Tag.IsValid() && Func(Tag)) || Super::ForEachTag(Func);
}
template <typename... TagTs>
FORCEINLINE bool TOUUGameplayTagCollectionBase<FGameplayTag, TagTs...>::IsEmpty() const
{
	return Tag.IsValid() == false && Super::IsEmpty();
}
template <typename... TagTs>
FORCEINLINE void TOUUGameplayTagCollectionBase<FGameplayTag, TagTs...>::AppendToContainer(
	FGameplayTagContainer& Container) const
{
	Container.AddTag(Tag);
	Super::AppendToContainer(Container);
}

//----------------------------------------------------------------------------------------------------------------------

template <typename... TagTs>
struct TOUUGameplayTagCollectionBase<FGameplayTagContainer, TagTs...> : public TOUUGameplayTagCollectionBase<TagTs...>
{
public:
	using Super = TOUUGameplayTagCollectionBase<TagTs...>;

public:
	TOUUGameplayTagCollectionBase(const FGameplayTagContainer& InContainer, const TagTs&... Tags);

public:
	bool CollectionHasExactTag(const FGameplayTag& Tag) const;
	bool CollectionHasTag(const FGameplayTag& Tag) const;
	bool TagMatchesCollectionExact(const FGameplayTag& Tag) const;
	bool TagMatchesCollection(const FGameplayTag& Tag) const;
	template <typename FuncT>
	bool ForEachTag(const FuncT& Func) const;

	// -- FOUUGameplayTagCollection interface
public:
	bool IsEmpty() const override;
	void AppendToContainer(FGameplayTagContainer& InContainer) const override;

private:
	const FGameplayTagContainer& Container;
};

template <typename... TagTs>
FORCEINLINE TOUUGameplayTagCollectionBase<FGameplayTagContainer, TagTs...>::TOUUGameplayTagCollectionBase(
	const FGameplayTagContainer& InContainer,
	const TagTs&... Tags)
	: Super(Tags...)
	, Container(InContainer)
{
}
template <typename... TagTs>
FORCEINLINE bool TOUUGameplayTagCollectionBase<FGameplayTagContainer, TagTs...>::CollectionHasExactTag(
	const FGameplayTag& Tag) const
{
	return Container.HasTagExact(Tag) || Super::CollectionHasExactTag(Tag);
}
template <typename... TagTs>
FORCEINLINE bool TOUUGameplayTagCollectionBase<FGameplayTagContainer, TagTs...>::CollectionHasTag(
	const FGameplayTag& Tag) const
{
	return Container.HasTag(Tag) || Super::CollectionHasTag(Tag);
}
template <typename... TagTs>
FORCEINLINE bool TOUUGameplayTagCollectionBase<FGameplayTagContainer, TagTs...>::TagMatchesCollectionExact(
	const FGameplayTag& Tag) const
{
	return Tag.MatchesAnyExact(Container) || Super::TagMatchesCollectionExact(Tag);
}
template <typename... TagTs>
FORCEINLINE bool TOUUGameplayTagCollectionBase<FGameplayTagContainer, TagTs...>::TagMatchesCollection(
	const FGameplayTag& Tag) const
{
	return Tag.MatchesAny(Container) || Super::TagMatchesCollection(Tag);
}
template <typename... TagTs>
template <typename FuncT>
FORCEINLINE bool TOUUGameplayTagCollectionBase<FGameplayTagContainer, TagTs...>::ForEachTag(const FuncT& Func) const
{
	for (const auto& Tag : Container)
	{
		if (Func(Tag))
		{
			return true;
		}
	}

	return Super::ForEachTag(Func);
}
template <typename... TagTs>
FORCEINLINE bool TOUUGameplayTagCollectionBase<FGameplayTagContainer, TagTs...>::IsEmpty() const
{
	return Container.IsEmpty() && Super::IsEmpty();
}
template <typename... TagTs>
FORCEINLINE void TOUUGameplayTagCollectionBase<FGameplayTagContainer, TagTs...>::AppendToContainer(
	FGameplayTagContainer& InContainer) const
{
	InContainer.AppendTags(Container);
	Super::AppendToContainer(InContainer);
}

//----------------------------------------------------------------------------------------------------------------------

// Allows you to perform operations on a collection of tags or containers without having to copy them into a single
// container.
// Note that this is mainly a performance optimization for cases where you have to do one-off queries on multiple
// containers without a significant overlap in their contents. If you need to do multiple queries and/or expect the
// input containers to contain largely the same tags, combining them into a single container may still be faster.
//
// Usage Example:
//	const auto Collection = MakeCollection(ContainerA, TagB, ContainerC);
//	if (Collection.HasAll(SomeOtherContainer))
//	{
//		// ...
//	}
template <typename... TagTs>
struct TOUUGameplayTagCollection
	: public TOUUGameplayTagCollectionBase<OUUTags::Private::TUnderlyingTagType<TagTs>...>
{
public:
	using Super = TOUUGameplayTagCollectionBase<OUUTags::Private::TUnderlyingTagType<TagTs>...>;

public:
	TOUUGameplayTagCollection(const TagTs&... Tags);

public:
	template <typename... OtherTagTs>
	bool HasAny(const TOUUGameplayTagCollection<OtherTagTs...>& CollectionToCheck) const;
	template <typename... OtherTagTs>
	bool HasAnyExact(const TOUUGameplayTagCollection<OtherTagTs...>& CollectionToCheck) const;
	template <typename... OtherTagTs>
	bool HasAll(const TOUUGameplayTagCollection<OtherTagTs...>& CollectionToCheck) const;
	template <typename... OtherTagTs>
	bool HasAllExact(const TOUUGameplayTagCollection<OtherTagTs...>& CollectionToCheck) const;

	// -- FOUUGameplayTagCollection interface
public:
	bool IsEmpty() const final;
	bool HasTag(const FGameplayTag& TagToCheck) const final;
	bool HasTagExact(const FGameplayTag& TagToCheck) const final;
	bool HasAny(const FGameplayTagContainer& ContainerToCheck) const final;
	bool HasAnyExact(const FGameplayTagContainer& ContainerToCheck) const final;
	bool HasAll(const FGameplayTagContainer& ContainerToCheck) const final;
	bool HasAllExact(const FGameplayTagContainer& ContainerToCheck) const final;
	bool ContainerHasAnyTagInCollection(const FGameplayTagContainer& ContainerToCheck) const final;
	bool ContainerHasAnyTagInCollectionExact(const FGameplayTagContainer& ContainerToCheck) const final;
	bool ContainerHasAllTagsInCollection(const FGameplayTagContainer& ContainerToCheck) const final;
	bool ContainerHasAllTagsInCollectionExact(const FGameplayTagContainer& ContainerToCheck) const final;
	FGameplayTagContainer ToContainer() const final;
	void AppendToContainer(FGameplayTagContainer& Container) const final;
};

template <typename... TagTs>
FORCEINLINE TOUUGameplayTagCollection<TagTs...>::TOUUGameplayTagCollection(const TagTs&... Tags)
	: Super(Tags...)
{
}
template <typename... TagTs>
template <typename... OtherTagTs>
FORCEINLINE bool TOUUGameplayTagCollection<TagTs...>::HasAny(
	const TOUUGameplayTagCollection<OtherTagTs...>& CollectionToCheck) const
{
	return CollectionToCheck.ForEachTag([&](const FGameplayTag& Tag) { return Super::CollectionHasTag(Tag); });
}
template <typename... TagTs>
template <typename... OtherTagTs>
FORCEINLINE bool TOUUGameplayTagCollection<TagTs...>::HasAnyExact(
	const TOUUGameplayTagCollection<OtherTagTs...>& CollectionToCheck) const
{
	return CollectionToCheck.ForEachTag([&](const FGameplayTag& Tag) { return Super::CollectionHasEactTag(Tag); });
}
template <typename... TagTs>
template <typename... OtherTagTs>
FORCEINLINE bool TOUUGameplayTagCollection<TagTs...>::HasAll(
	const TOUUGameplayTagCollection<OtherTagTs...>& CollectionToCheck) const
{
	return CollectionToCheck.ForEachTag([&](const FGameplayTag& Tag) {
		return Super::CollectionHasTag(Tag) == false;
	}) == false;
}
template <typename... TagTs>
template <typename... OtherTagTs>
FORCEINLINE bool TOUUGameplayTagCollection<TagTs...>::HasAllExact(
	const TOUUGameplayTagCollection<OtherTagTs...>& CollectionToCheck) const
{
	return CollectionToCheck.ForEachTag([&](const FGameplayTag& Tag) {
		return Super::CollectionHasExactTag(Tag) == false;
	}) == false;
}
template <typename... TagTs>
FORCEINLINE bool TOUUGameplayTagCollection<TagTs...>::IsEmpty() const
{
	return Super::IsEmpty();
}
template <typename... TagTs>
FORCEINLINE bool TOUUGameplayTagCollection<TagTs...>::HasTag(const FGameplayTag& TagToCheck) const
{
	return Super::CollectionHasTag(TagToCheck);
}
template <typename... TagTs>
FORCEINLINE bool TOUUGameplayTagCollection<TagTs...>::HasTagExact(const FGameplayTag& TagToCheck) const
{
	return Super::CollectionHasExactTag(TagToCheck);
}

template <typename... TagTs>
bool TOUUGameplayTagCollection<TagTs...>::HasAny(const FGameplayTagContainer& ContainerToCheck) const
{
	for (const auto& Tag : ContainerToCheck)
	{
		if (Super::CollectionHasTag(Tag))
		{
			return true;
		}
	}

	return false;
}

template <typename... TagTs>
bool TOUUGameplayTagCollection<TagTs...>::HasAnyExact(const FGameplayTagContainer& ContainerToCheck) const
{
	for (const auto& Tag : ContainerToCheck)
	{
		if (Super::CollectionHasExactTag(Tag))
		{
			return true;
		}
	}

	return false;
}

template <typename... TagTs>
bool TOUUGameplayTagCollection<TagTs...>::HasAll(const FGameplayTagContainer& ContainerToCheck) const
{
	for (const auto& Tag : ContainerToCheck)
	{
		if (Super::CollectionHasTag(Tag) == false)
		{
			return false;
		}
	}

	return true;
}

template <typename... TagTs>
bool TOUUGameplayTagCollection<TagTs...>::HasAllExact(const FGameplayTagContainer& ContainerToCheck) const
{
	for (const auto& Tag : ContainerToCheck)
	{
		if (Super::CollectionHasExactTag(Tag) == false)
		{
			return false;
		}
	}

	return true;
}

template <typename... TagTs>
FORCEINLINE bool TOUUGameplayTagCollection<TagTs...>::ContainerHasAnyTagInCollection(
	const FGameplayTagContainer& ContainerToCheck) const
{
	return Super::ForEachTag([&](const FGameplayTag& Tag) { return ContainerToCheck.HasTag(Tag); });
}
template <typename... TagTs>
FORCEINLINE bool TOUUGameplayTagCollection<TagTs...>::ContainerHasAnyTagInCollectionExact(
	const FGameplayTagContainer& ContainerToCheck) const
{
	return Super::ForEachTag([&](const FGameplayTag& Tag) { return ContainerToCheck.HasTagExact(Tag); });
}
template <typename... TagTs>
FORCEINLINE bool TOUUGameplayTagCollection<TagTs...>::ContainerHasAllTagsInCollection(
	const FGameplayTagContainer& ContainerToCheck) const
{
	return Super::ForEachTag([&](const FGameplayTag& Tag) { return ContainerToCheck.HasTag(Tag) == false; })
		   == false;
}
template <typename... TagTs>
FORCEINLINE bool TOUUGameplayTagCollection<TagTs...>::ContainerHasAllTagsInCollectionExact(
	const FGameplayTagContainer& ContainerToCheck) const
{
	return Super::ForEachTag([&](const FGameplayTag& Tag) { return ContainerToCheck.HasTagExact(Tag) == false; })
		   == false;
}
template <typename... TagTs>
FORCEINLINE FGameplayTagContainer TOUUGameplayTagCollection<TagTs...>::ToContainer() const
{
	FGameplayTagContainer Result;
	AppendToContainer(Result);
	return Result;
}
template <typename... TagTs>
FORCEINLINE void TOUUGameplayTagCollection<TagTs...>::AppendToContainer(FGameplayTagContainer& Container) const
{
	Super::AppendToContainer(Container);
}

//----------------------------------------------------------------------------------------------------------------------

// Utility function for creating a gameplay tag collection from multiple separate tags and/or containers.
template <typename... TagTs>
FORCEINLINE TOUUGameplayTagCollection<TagTs...> MakeCollection(const TagTs&... Tags)
{
	return TOUUGameplayTagCollection<TagTs...>(Tags...);
}
