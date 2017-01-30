
#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Prefab/PrefabAsset.h>
#include <AzCore/Math/Transform.h>
#include <algorithm>
#include <Environment_Tile_System_v2/WeatherStructs.h>

#include <Environment_Tile_System_v2/EnvTileBus.h>
#include <AzFramework/Entity/EntityContextBus.h>
#include <AzCore/Component/ComponentBus.h>
#include <LmbrCentral/Physics/PhysicsSystemComponentBus.h>

namespace EnvTile
{
	
	class Env_TileGenerator
		: public AZ::Component
		, protected Env_GeneratorRequestBus::Handler
		, private AzFramework::SliceInstantiationResultBus::MultiHandler
	{
	public:
		AZ_COMPONENT(Env_TileGenerator, "{A03A8932-D4E4-4766-A536-8CA8C280C7BD}")

			Env_TileGenerator();
		~Env_TileGenerator() override = default;

		static void Reflect(AZ::ReflectContext* context);

		static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
		static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
		static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
		static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

		////////////////////////////////////////////////////////////////////////
		// Env_GeneratorRequestBus interface implementation
		////////////////////////////////////////////////////////////////////////
		AzFramework::SliceInstantiationTicket Gen_SpawnSlice(const AZ::Data::Asset<AZ::Data::AssetData>& slice) override;
		AzFramework::SliceInstantiationTicket Gen_SpawnSliceRelative(const AZ::Data::Asset<AZ::Data::AssetData>& slice, const AZ::Transform& relative) override;
		////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// SliceInstantiationResultBus::MultiHandler
		void OnSliceInstantiated(const AZ::Data::AssetId& sliceAssetId, const AZ::PrefabComponent::PrefabInstanceAddress& sliceAddress) override;
		void OnSliceInstantiationFailed(const AZ::Data::AssetId& sliceAssetId) override;
		//////////////////////////////////////////////////////////////////////////

	protected:
		////////////////////////////////////////////////////////////////////////
		// Node Functions
		int getRowSize() override { return maxRowSize; }
		int getMaxTiles() override { return maxTiles; }
		int getXOffset() override { return xOffset; }
		int getYOffset() override { return yOffset; }
		bool getRepetition() override { return sp_Type == spawnType::Repeat; }
		int getSliceCount(int n) override { return sliceList.size(); }
		////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////
		// Editor UI Notify Functions
		AZ::u32 onListIndexChanged();
		AZ::u32 onListLengthChanged();
		AZ::u32 onLocalTransformChanged();
		//AZ::u32 onConstTransformChanged();
		////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////
		// AZ::Component interface implementation
		void Init() override;
		void Activate() override;
		void Deactivate() override;
		////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////
		// Env_GeneratorRequestBus interface implementation

		//Get Functions
		int getSimple() override;
		int getRainStrengths() override;
		int getRainExtra() override;
		int getRainTODStart() override;
		int getRainTODStop() override;
		int getSnowStrengths() override;
		bool getSnowFreezeGround() override;
		float getSnowFreezeAmount() override;
		int getSnowTODStart() override;
		int getSnowTODStop() override;

		//Set Functions
		int setSimple(bool, bool) override;
		int setRainStrengths(bool, bool, bool) override;
		int setRainExtra(bool, bool) override;
		int setRainTODStart(int) override;
		int setRainTODStop(int) override;
		int setSnowStrengths(bool, bool, bool) override;
		bool setSnowFreezeGround(bool) override;
		float setSnowFreezeAmount(float) override;
		int setSnowTODStart(int) override;
		int setSnowTODStop(int) override;
		////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////
		//Editor UI Notify Functions
		void checkRainEnabled();
		AZ::u32 OnRainStartTODChanged();
		AZ::u32 OnRainStopTODChanged();
		AZ::u32 OnSnowStartTODChanged();
		AZ::u32 OnSnowStopTODChanged();
		////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////
		// UI Enum Fields
		enum class spawnMethod {
			Randomized, //Select Slice to Spawn from list randomly with constant transform offset.
			Ordered,	//Select Slices in order from list with constant transform offset.
			Manual		//Select Slices in order from list with manual transforms
		};
		enum class spawnType {
			Once,		//Goes through the list of Slices once and spawns each entry.
			Repeat		//Loops through the list of Slices until the maximum specified number of tiles are spawned.
		};

		////////////////////////////////////////////////////////////////////////

		//Ids of spawned entities
		AZStd::vector<AZ::EntityId> entityIds;
		//int entity_layerID = 0;

		//WEATHER
		//First builds trigger list and order [PHASEOUT]
		AZStd::vector<WeatherTrigger> env_weather_trigger;
		AZStd::deque<WeatherUnit>queueTriggers;


		int initialTime = 0;//Time at game start.
		int tmp_spawned_tiles = 0;//DEBUG: LOGGED TO CONSOLE

								  //Helper Functions
		void PostActivate();
		//From Env_TileBus
		void preloadTriggersAtTime(int tod) override;//[TODO]
		WeatherUnit processNextTrigger() override;//[TODO]
		bool triggerListEmpty() override {
			return queueTriggers.empty();
		}

	private:
#pragma region
		//Grid Parameters
		int maxRowSize = 1;
		int maxTiles = 1;

		//Const Offset for auto grid spawning
		int xOffset = 0, yOffset = 0;

		//Tile list
		//AZStd::vector<AZ::Data::Asset<AZ::DynamicPrefabAsset>> sliceList;
		//List of Local Transforms per slice
		AZStd::vector<AZ::Transform> sliceLocalTransforms;

		//LAYER->Slice List
		AZStd::vector<AZ::Data::Asset<AZ::DynamicPrefabAsset>> sliceList;
		int slice_layerID = 0, max_slice_layerID = 0;//For UI

		//Editor UI access to Lists
		int listIndex = 0;
		int listSize = 0;

		//Weather
		//Enables
		bool RAIN_ENABLED = false;
		bool SNOW_ENABLED = false;

		//Simple
		//TODO: For actual use expand these two options to COMPLEX FORM
		bool Cloudy = false;
		bool Windy = false;

		//Complex-RAIN
		bool rain_strength_light = false;
		bool rain_strength_medium = false;
		bool rain_strength_heavy = false;
		bool rain_extra_Thunder = false;
		bool rain_extra_Lightning = false;
		//Need to add thunder sound sample...
		int rain_TOD_Range_Start = 0;
		int rain_TOD_Range_Stop = 0;

		//Complex-Snowfall
		bool snow_strength_light = false;
		bool snow_strength_medium = false;
		bool snow_strength_heavy = false;
		bool snow_extra_FreezeGround = false;
		float snow_extra_FreezeAmount = 0.0f;
		int snow_TOD_Range_Start = 0;
		int snow_TOD_Range_Stop = 0;
#pragma endregion Base Tiles


		//Test 2D Vectors
		AZStd::vector<AZStd::vector<AZ::Data::Asset<AZ::DynamicPrefabAsset>>> decoLayer;

		//[UI_ONLY] -> Local Transform Components
		AZ::Vector3 offloc = AZ::Vector3::CreateZero();
		AZ::Vector3 offrot = AZ::Vector3::CreateZero();//Unimplemented. [TODO]
		AZ::Vector3 offscale = AZ::Vector3::CreateOne();

		//[UI_ONLY] -> Spawn Params(BASE)
		spawnMethod sp_Method = spawnMethod::Randomized;
		spawnType   sp_Type = spawnType::Once;

		//Spawning Helper (Taken from SpawnerComponent)
		AzFramework::SliceInstantiationTicket SpawnSliceInternal(const AZ::Data::Asset<AZ::Data::AssetData>& slice, const AZ::Transform& relative);
	};
}
