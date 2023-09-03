// Copyright (c) 2023 Jonas Reich & Contributors

#include "GameplayTags/TypedGameplayTagContainerCustomization.h"

#include "DetailWidgetRow.h"
#include "GameplayTagsEditorModule.h"
#include "IDetailChildrenBuilder.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"

FTypedGameplayTagContainer* GetContainerFromPropertyHandle(TSharedPtr<IPropertyHandle> Handle)
{
	if (Handle.IsValid() == false || Handle->IsValidHandle() == false)
		return nullptr;

	void* ValuePtr = nullptr;
	if (Handle->GetValueData(OUT ValuePtr) == FPropertyAccess::Result::Success)
	{
		return static_cast<FTypedGameplayTagContainer*>(ValuePtr);
	}
	return nullptr;
}

void FTypedGameplayTagContainer_PropertyTypeCustomiation::CustomizeHeader(
	TSharedRef<IPropertyHandle> PropertyHandle,
	FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	if (PropertyHandle->IsValidHandle() == false)
		return;

	HandleSP = PropertyHandle;

	HeaderRow.NameContent()[PropertyHandle->CreatePropertyNameWidget()];

	TArray<void*> RawDataPtrs;
	PropertyHandle->AccessRawData(OUT RawDataPtrs);
	if (RawDataPtrs.Num() != 1)
	{
		HeaderRow
			.ValueContent()[SNew(STextBlock).Text(INVTEXT("Multi-editing not supported for typed tag containers"))];
		return;
	}

	// The class we're editing
	auto OuterBaseClass = PropertyHandle->GetOuterBaseClass();
	// The class where the property was first created
	auto PropertyOwnerClass = PropertyHandle->GetProperty()->GetOwnerClass();

	const bool bIsTypedTagNameEditable = OuterBaseClass == PropertyOwnerClass;

	UStruct* ParentStruct = FTypedGameplayTag_Base::StaticStruct();
	for (auto* Struct : TObjectRange<UScriptStruct>())
	{
		// Exclude the parent struct itself from the results.
		if (Struct == ParentStruct)
			continue;

		if (Struct->IsChildOf(ParentStruct))
		{
			TypedTagOptions.Add(*Struct->GetName());
		}
	}

	if (auto* Container = GetContainerFromPropertyHandle(PropertyHandle))
	{
		WorkingContainer = MakeShared<FGameplayTagContainer>(Container->Tags);

		TSharedPtr<SBox> ComboBoxSlot;

		// clang-format off
		HeaderRow.ValueContent()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight().Padding(2.f)[SAssignNew(ComboBoxSlot, SBox)]
			+ SVerticalBox::Slot().AutoHeight().Padding(2.f)[SAssignNew(ErrorBox, SBox)]
			+ SVerticalBox::Slot().AutoHeight().Padding(2.f)[SAssignNew(GamplayTagContainerBox, SBox)]
		];
		// clang-format on

		if (auto TypedTagName =
				PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FTypedGameplayTagContainer, TypedTagName)))
		{
			auto SelectionChangedLambda = [&, this](FName NewName, ESelectInfo::Type) {
				if (auto* Container = GetContainerFromPropertyHandle(HandleSP))
				{
					// Change tag name
					auto TypedTagName =
						HandleSP->GetChildHandle(GET_MEMBER_NAME_CHECKED(FTypedGameplayTagContainer, TypedTagName))
							.ToSharedRef();

					TypedTagName->SetValueFromFormattedString(NewName.ToString());

					// We need a new widget with updated filter string
					RefreshContainerWidget();
				}
			};

			// clang-format off
			ComboBoxSlot->SetContent(
				SNew(SComboBox<FName>)
					.OptionsSource(&TypedTagOptions)
					.OnSelectionChanged_Lambda(SelectionChangedLambda)
					.InitiallySelectedItem(Container->TypedTagName)
					.IsEnabled(bIsTypedTagNameEditable)
					.OnGenerateWidget_Lambda([](FName Name) { return SNew(STextBlock).Text(FText::FromName(Name)); })
				[
					// Needs some random content. Ask Epic
					SNew(STextBlock)
					.Text_Lambda([this]() -> FText
					{
						if (auto* Container = GetContainerFromPropertyHandle(HandleSP))
						{
							return FText::FromName(Container->TypedTagName);
						}
						return FText::GetEmpty();
					})
				]
			);
			// clang-format on
		}

		RefreshContainerWidget();
	}
}

void FTypedGameplayTagContainer_PropertyTypeCustomiation::CustomizeChildren(
	TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder,
	IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	// No children.
}

void FTypedGameplayTagContainer_PropertyTypeCustomiation::RefreshContainerWidget()
{
	if (auto* Container = GetContainerFromPropertyHandle(HandleSP))
	{
		// Update filter tags
		Container->PopulateFilterTags();

		{
			// Filter tags in the copy to check if there are tags that do not match the filter tags.
			auto OnlyRightTags = CopyTemp(*Container);
			OnlyRightTags.SetTags(OnlyRightTags.Tags);

			auto OnlyWrongTags = CopyTemp(*Container);
			OnlyWrongTags.Tags.RemoveTags(OnlyRightTags.Tags);

			if (OnlyWrongTags.Tags.Num() > 0)
			{
				// clang-format off
				TSharedRef<SWidget> ErrorWidget =
					SNew(SBorder)
					.ColorAndOpacity(FColor::Yellow)
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot().AutoHeight().Padding(2.f)
						[
							SNew(STextBlock)
							.ColorAndOpacity(FColor::Yellow)
							.Text(FText::Format(INVTEXT("{0} tags that do not match the filter: {1}"),
								  FText::AsNumber(OnlyWrongTags.Tags.Num()),
								  FText::FromString(OnlyWrongTags.Tags.ToStringSimple())))
							.Clipping(EWidgetClipping::ClipToBounds)
							.OverflowPolicy(ETextOverflowPolicy::Ellipsis)
						]
						+ SVerticalBox::Slot().AutoHeight().Padding(2.f)
						[
							SNew(SButton)
							.Text(INVTEXT("Delete Mismatch Tags"))
							.OnClicked_Lambda([this, RightTags = OnlyRightTags.Tags]() {
								SetNewTags(RightTags);
								RefreshContainerWidget();
								return FReply::Handled();
							})
						]
					];
				// clang-format on

				ErrorBox->SetContent(ErrorWidget);
			}
			else
			{
				ErrorBox->SetContent(SNullWidget::NullWidget);
			}
		}

		const FString FilterString = OUU::Runtime::Private::MakeFilterString(Container->CachedFilterTags);
		auto GameplayTagContainerWidget = IGameplayTagsEditorModule::Get().MakeGameplayTagContainerWidget(
			FOnSetGameplayTagContainer::CreateLambda(
				[this](const FGameplayTagContainer& NewValue) { SetNewTags(NewValue); }),
			WorkingContainer,
			FilterString);
		GamplayTagContainerBox->SetContent(GameplayTagContainerWidget);
	}
}

void FTypedGameplayTagContainer_PropertyTypeCustomiation::SetNewTags(const FGameplayTagContainer& NewValue)
{
	if (auto* Container = GetContainerFromPropertyHandle(HandleSP))
	{
		auto Tags = HandleSP->GetChildHandle(GET_MEMBER_NAME_CHECKED(FTypedGameplayTagContainer, Tags)).ToSharedRef();

		Tags->SetValueFromFormattedString(NewValue.ToString());
	}
}
