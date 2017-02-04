
#include "StdAfx.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Component/Entity.h>
#include <ISystem.h>

//Buses
#include <AzCore/Component/ComponentApplicationBus.h>
#include <AzFramework/Entity/EntityContextBus.h>
#include <AzFramework/Entity/GameEntityContextBus.h>
#include <AzCore/Component/TransformBus.h>

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

#pragma region Crystal
    void Crystal::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
			serialize->Class<Crystal, AZ::Component>()
				->Version(0)
				->Field("Crystal-Enabled", &Crystal::enabled)
				->Field("Crystal-Hidden", &Crystal::hidden)
				->Field("Crystal-ForceLocalModel", &Crystal::forceLocalSlice)
				->Field("Crystal-Slice", &Crystal::crystalModel)
				->Field("Crystal-Static Slice", &Crystal::staticModel)
				;
            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<Crystal>("Crystal", "A collectible crystal")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::Category, "DragonCrash-Collectibles")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game"))
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)

					->ClassElement(GRP, "Crystal")
					->DataElement(UI_D, &Crystal::enabled, "Enabled", "If enabled, this asset can function as the gem.")
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
	bool Crystal::getHidden() {
		return hidden;
	}
	bool Crystal::getEnabled(){
		return enabled;
	}

	void Crystal::setHidden(bool b){
		hidden = b;
	}

	void Crystal::setEnabled(bool b){
		enabled = b;
	}

	void Crystal::setCrystalModel(AZ::Data::Asset<AZ::DynamicPrefabAsset> slice) {
		crystalModel = slice;
	}

#pragma region Spawner
	void Crystal::Spawn() {
		if (enabled) {
			if (hidden)Crystal_SpawnSlice(staticModel);
			else Crystal_SpawnSlice(crystalModel);
		}
		else Crystal_SpawnSlice(staticModel);
	}

	AzFramework::SliceInstantiationTicket Crystal::Crystal_SpawnSlice(const AZ::Data::Asset<AZ::Data::AssetData>& slice) {
		return SpawnSliceInternal(slice, AZ::Transform::Identity());
	}

	AzFramework::SliceInstantiationTicket Crystal::Crystal_SpawnSliceRelative(const AZ::Data::Asset<AZ::Data::AssetData>& slice, const AZ::Transform& relative) {
		return SpawnSliceInternal(slice, relative);
	}

	void Crystal::OnSliceInstantiated(const AZ::Data::AssetId& sliceAssetId, const AZ::PrefabComponent::PrefabInstanceAddress& sliceAddress) {
		//Taken from SpawnerComponent: spawns all entities packaged in the slice.
		const AzFramework::SliceInstantiationTicket& ticket = (*AzFramework::SliceInstantiationResultBus::GetCurrentBusId());

		// Stop listening for this ticket (since it's done). We can have have multiple tickets in flight.
		AzFramework::SliceInstantiationResultBus::MultiHandler::BusDisconnect(ticket);

		const AZ::PrefabComponent::EntityList& entities = sliceAddress.second->GetInstantiated()->m_entities;
		for (AZ::Entity * currEntity : entities)
		{
			CryLog("Slice Instantiated");
		}

		//EBUS_EVENT_ID(GetEntityId(), Env_TileNotificationBus, OnSpawned, ticket, entityIds);
	}

	void Crystal::OnSliceInstantiationFailed(const AZ::Data::AssetId& sliceAssetId) {
		AzFramework::SliceInstantiationResultBus::MultiHandler::BusDisconnect(*AzFramework::SliceInstantiationResultBus::GetCurrentBusId());
		AZ_Error("Env_TileGenerator", false, "Slice '%s' failed to instantiate", sliceAssetId.ToString<AZStd::string>().c_str());
	}

	AzFramework::SliceInstantiationTicket Crystal::SpawnSliceInternal(const AZ::Data::Asset<AZ::Data::AssetData>& slice, const AZ::Transform& relative) {
		//Taken from SpawnerComponent

		AZ::Transform t = AZ::Transform::Identity();

		EBUS_EVENT_ID_RESULT(t, GetEntityId(), AZ::TransformBus, GetWorldTM);

		t *= relative;

		AzFramework::SliceInstantiationTicket ticket;

		EBUS_EVENT_RESULT(ticket, AzFramework::GameEntityContextRequestBus, InstantiateDynamicSlice, slice, t, nullptr);
		AzFramework::SliceInstantiationResultBus::MultiHandler::BusConnect(ticket);

		return ticket;
	}
#pragma endregion Spawner
#pragma endregion DragonCrashCollectibleRequestBus

#pragma endregion Crystal


}
