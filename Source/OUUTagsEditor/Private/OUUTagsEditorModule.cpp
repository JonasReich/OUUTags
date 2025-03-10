// Copyright (c) 2025 Jonas Reich & Contributors

#include "GameplayTags/TypedGameplayTagContainer.h"
#include "GameplayTags/TypedGameplayTagContainerCustomization.h"
#include "MessageLogModule.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"

struct FOUUTagsEditorModule : public FDefaultModuleImpl
{
public:
	void StartupModule() override
	{
		FCoreDelegates::OnAllModuleLoadingPhasesComplete.AddLambda(
			[]() { FTypedGameplayTag_Base::RegisterAllDerivedPropertyTypeLayouts(); });

		auto& PropertyEditor = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyEditor.RegisterCustomPropertyTypeLayout(
			FTypedGameplayTagContainer::StaticStruct()->GetFName(),
			FOnGetPropertyTypeCustomizationInstance::CreateLambda([]() -> TSharedRef<IPropertyTypeCustomization> {
				return MakeShared<FTypedGameplayTagContainer_PropertyTypeCustomization>();
			}));

		auto& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
		MessageLogModule.RegisterLogListing(TEXT("GameplayTagValidation"), INVTEXT("Gameplay Tag Validation"));
	}

	void ShutdownModule() override
	{
		FTypedGameplayTag_Base::UnregisterAllDerivedPropertyTypeLayouts();

		if (auto* PropertyEditor = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor"))
		{
			PropertyEditor->UnregisterCustomPropertyTypeLayout(FTypedGameplayTagContainer::StaticStruct()->GetFName());
		}
	}
};

IMPLEMENT_MODULE(FOUUTagsEditorModule, OUUTagsEditor)
