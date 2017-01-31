
#include "StdAfx.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>

#include "DragonCrashCollectiblesSystemComponent.h"

#define UI_D  AZ::Edit::UIHandlers::Default
#define UI_CB AZ::Edit::UIHandlers::ComboBox
#define UI_SL AZ::Edit::UIHandlers::Slider
#define UI_SP AZ::Edit::UIHandlers::SpinBox
#define GRP   AZ::Edit::ClassElements::Group
#define CN    AZ::Edit::Attributes::ChangeNotify
#define VS	  AZ::Edit::Attributes::Visibility

namespace DragonCrashCollectibles
{
    void Crystal::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
			serialize->Class<Crystal, AZ::Component>()
				->Version(0)
				->Field("Crystal-Hidden", &Crystal::hidden)
				->Field("Crystal-ForceLocalModel", &Crystal::forceLocalSlice)
				->Field("Crystal-Slices", &Crystal::crystalModel)
				;
            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<Crystal>("Crystal", "A collectible crystal")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::Category, "DragonCrash-Collectibles")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("System"))
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)

					->ClassElement(GRP, "Crystal")
					->DataElement(UI_D, &Crystal::hidden, "Hidden in Mesh", "If true, the crystal will be hidden inside a destructible mesh. Else it will appear above the entity's position.")
					->DataElement(UI_D, &Crystal::forceLocalSlice, "Force Local Slice", "If true, then this gem will use the specified slice as specified below.")
					->DataElement(UI_D, &Crystal::crystalModel, "Crystal Slice","Slice to assign to this gem (Can be overwritten from the Environment Tile)")
                    ;
            }
        }
    }

    void Crystal::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("CrystalService"));
    }

    void Crystal::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC("CrystalService"));
    }

    void Crystal::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        (void)required;
    }

    void Crystal::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        (void)dependent;
    }

    void Crystal::Init()
    {
    }

    void Crystal::Activate()
    {
       CrystalRequestBus::Handler::BusConnect();
    }

    void Crystal::Deactivate()
    {
        CrystalRequestBus::Handler::BusDisconnect();
    }

#pragma region DragonCrashCollectibleRequestBus
	bool Crystal::isHidden() {
		return hidden;
	}

	void Crystal::setCrystalModel(AZ::Data::Asset<AZ::DynamicPrefabAsset> slice) {
		crystalModel = slice;
	}
#pragma endregion DragonCrashCollectibleRequestBus
}
