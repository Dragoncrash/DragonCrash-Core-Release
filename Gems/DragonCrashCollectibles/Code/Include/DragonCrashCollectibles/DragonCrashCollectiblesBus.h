
#pragma once

#include <AzCore/EBus/EBus.h>
#include <AzCore/Prefab/PrefabAsset.h>
#include <AzFramework/Entity/EntityContextBus.h>

namespace DragonCrashCollectibles
{
    class CrystalRequests
        : public AZ::EBusTraits
    {

    public:
		virtual ~CrystalRequests() {}

        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::ById;
		using BusIdType = AZ::EntityId;

        // Public functions		
		virtual AzFramework::SliceInstantiationTicket Crystal_SpawnSlice(const AZ::Data::Asset<AZ::Data::AssetData>& slice) = 0;
		virtual AzFramework::SliceInstantiationTicket Crystal_SpawnSliceRelative(const AZ::Data::Asset<AZ::Data::AssetData>& slice, const AZ::Transform& relative) = 0;

		virtual void Spawn() = 0;
		virtual void Despawn() = 0;

		virtual bool isCrystal() = 0;

		virtual bool getEnabled() = 0;
		virtual bool getHidden() = 0;

		virtual void setCrystalModel(AZ::Data::Asset<AZ::DynamicPrefabAsset>) = 0;
		virtual void setEnabled(bool) = 0;
		virtual void setHidden(bool) = 0;
		virtual void setSpawnLink(AZ::EntityId) = 0;
		virtual void setSpawnLinkValid(bool) = 0;
    };
    using CrystalRequestBus = AZ::EBus<CrystalRequests>;

	class CrystalNotifications : public AZ::ComponentBus
	{
	public:
		virtual ~CrystalNotifications() {}

		virtual void OnSpawned(const AzFramework::SliceInstantiationTicket& ticket, const AZStd::vector<AZ::EntityId>& spawnedEntities) = 0;
	};
	using CrystalNotificationBus = AZ::EBus<CrystalNotifications>;
} // namespace DragonCrashCollectibles
