
#pragma once

#include <AzCore/EBus/EBus.h>
#include <AzCore/Component/ComponentBus.h>
#include <AzFramework/Entity/EntityContextBus.h>
#include "WeatherStructs.h"

namespace EnvTile
{
   /* class Environment_Tile_System_v2Requests
        : public AZ::EBusTraits
    {

    public:
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        // Public functions
    };
    using Environment_Tile_System_v2RequestBus = AZ::EBus<Environment_Tile_System_v2Requests>;
	*/

	class Env_GeneratorRequests : public AZ::ComponentBus
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		// EBusTraits overrides
		static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
		static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::ById;
		//////////////////////////////////////////////////////////////////////////
		virtual ~Env_GeneratorRequests() {}

		//GENERATOR Spawn Events
		virtual AzFramework::SliceInstantiationTicket Gen_SpawnSlice(const AZ::Data::Asset<AZ::Data::AssetData>& slice) = 0;
		virtual AzFramework::SliceInstantiationTicket Gen_SpawnSliceRelative(const AZ::Data::Asset<AZ::Data::AssetData>& slice, const AZ::Transform& relative) = 0;

		//WEATHER Effects Control
		//Get Functions
		virtual int getSimple() = 0;
		virtual int getRainStrengths() = 0;
		virtual int getRainExtra() = 0;
		virtual int getRainTODStart() = 0;
		virtual int getRainTODStop() = 0;
		virtual int getSnowStrengths() = 0;
		virtual bool getSnowFreezeGround() = 0;
		virtual float getSnowFreezeAmount() = 0;
		virtual int getSnowTODStart() = 0;
		virtual int getSnowTODStop() = 0;

		//Set Functions
		virtual int setSimple(bool, bool) = 0;
		virtual int setRainStrengths(bool, bool, bool) = 0;
		virtual int setRainExtra(bool, bool) = 0;
		virtual int setRainTODStart(int) = 0;
		virtual int setRainTODStop(int) = 0;
		virtual int setSnowStrengths(bool, bool, bool) = 0;
		virtual bool setSnowFreezeGround(bool) = 0;
		virtual float setSnowFreezeAmount(float) = 0;
		virtual int setSnowTODStart(int) = 0;
		virtual int setSnowTODStop(int) = 0;

		//Flow Node Events
		virtual int getRowSize() = 0;
		virtual int getMaxTiles() = 0;
		virtual int getXOffset() = 0;
		virtual int getYOffset() = 0;
		virtual bool getRepetition() = 0;
		virtual int getSliceCount(int) = 0;

		virtual void preloadTriggersAtTime(int) = 0;
		virtual WeatherUnit processNextTrigger() = 0;
		virtual bool triggerListEmpty() = 0;

	};
	using Env_GeneratorRequestBus = AZ::EBus<Env_GeneratorRequests>;

	//Notification Bus
	class Env_TileNotifications : public AZ::ComponentBus
	{
	public:
		virtual ~Env_TileNotifications() {}

		virtual void OnSpawned(const AzFramework::SliceInstantiationTicket& ticket, const AZStd::vector<AZ::EntityId>& spawnedEntities) = 0;
	};
	using Env_TileNotificationBus = AZ::EBus<Env_TileNotifications>;

} // namespace Environment_Tile_System_v2
