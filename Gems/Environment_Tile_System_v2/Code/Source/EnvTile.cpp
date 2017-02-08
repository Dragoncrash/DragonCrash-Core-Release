
#include "StdAfx.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Component/Entity.h>
#include <ISystem.h>
#include <AzCore/Math/Quaternion.h>

//Buses
#include <AzCore/Component/ComponentApplicationBus.h>
#include <AzFramework/Entity/EntityContextBus.h>
#include <AzFramework/Entity/GameEntityContextBus.h>
#include <AzCore/Component/TransformBus.h>
#include <DragonCrashCollectibles/DragonCrashCollectiblesBus.h>

#include "EnvTile.h"

#define UI_D  AZ::Edit::UIHandlers::Default
#define UI_CB AZ::Edit::UIHandlers::ComboBox
#define UI_SL AZ::Edit::UIHandlers::Slider
#define UI_SP AZ::Edit::UIHandlers::SpinBox
#define GRP   AZ::Edit::ClassElements::Group
#define CN    AZ::Edit::Attributes::ChangeNotify
#define VS	  AZ::Edit::Attributes::Visibility

namespace EnvTile
{
   
#pragma region Component
	Env_TileGenerator::Env_TileGenerator() {
		//Taken from SpawnerComponent. Not sure what this does yet
		for (auto iter = sliceList.begin(); iter != sliceList.end(); iter++) {
			iter->SetFlags(static_cast<AZ::u8>(AZ::Data::AssetFlags::OBJECTSTREAM_NO_LOAD));
		}
		
	}

	void Env_TileGenerator::Reflect(AZ::ReflectContext* context)
	{
		if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
		{
			serialize->Class<Env_TileGenerator, AZ::Component>()
				->Version(0)

				//Generator Params
				->Field("GridRowSize", &Env_TileGenerator::maxRowSize)
				->Field("MaxTiles", &Env_TileGenerator::maxTiles)

				//Editor UI List Access
				->Field("ListIndex", &Env_TileGenerator::listIndex)
				->Field("ListSize", &Env_TileGenerator::listSize)
				->Field("tmpLoc", &Env_TileGenerator::offloc)
				->Field("tmpRot", &Env_TileGenerator::offrot)
				->Field("tmpScale", &Env_TileGenerator::offscale)
				->Field("xOffset", &Env_TileGenerator::xOffset)
				->Field("yOffset", &Env_TileGenerator::yOffset)

				//Editor UI Exclusive Fields
				->Field("SpawnMethod", &Env_TileGenerator::sp_Method)
				->Field("SpawnType", &Env_TileGenerator::sp_Type)

				//Object and Transform Lists
				->Field("sliceList", &Env_TileGenerator::sliceList)
				->Field("sliceLocalTransformList", &Env_TileGenerator::sliceLocalTransforms)

				//Simple Weather
				->Field("Cloudy", &Env_TileGenerator::Cloudy)
				->Field("Windy", &Env_TileGenerator::Windy)

				//Complex Weather: Rain
				->Field("Rain Enabled", &Env_TileGenerator::RAIN_ENABLED)
				->Field("Light Rain", &Env_TileGenerator::rain_strength_light)
				->Field("Medium Rain", &Env_TileGenerator::rain_strength_medium)
				->Field("Heavy Rain", &Env_TileGenerator::rain_strength_heavy)
				->Field("Thunder Enabled", &Env_TileGenerator::rain_extra_Thunder)
				->Field("Lightning Enabled", &Env_TileGenerator::rain_extra_Lightning)
				->Field("Rain TOD Low Limit", &Env_TileGenerator::rain_TOD_Range_Start)
				->Field("Rain TOD High Limit", &Env_TileGenerator::rain_TOD_Range_Stop)

				//Complex Weather: Snow
				->Field("Snow Enabled", &Env_TileGenerator::SNOW_ENABLED)
				->Field("Light Snow", &Env_TileGenerator::snow_strength_light)
				->Field("Medium Snow", &Env_TileGenerator::snow_strength_medium)
				->Field("Heavy Snow", &Env_TileGenerator::snow_strength_heavy)
				->Field("Freeze Ground", &Env_TileGenerator::snow_extra_FreezeGround)
				->Field("Freeze Amount", &Env_TileGenerator::snow_extra_FreezeAmount)
				->Field("Snow TOD Low Limit", &Env_TileGenerator::snow_TOD_Range_Start)
				->Field("Snow TOD High Limit", &Env_TileGenerator::snow_TOD_Range_Stop)

				//Decorative Layer
				->Field("Floating Islands Slice List", &Env_TileGenerator::decoLayer)

				//Crystals
				->Field("Spawn Crystals", &Env_TileGenerator::spawnCrystals)
				->Field("Tile Indices", &Env_TileGenerator::tileIndices)
				->Field("Multi-Crystal Placement", &Env_TileGenerator::multipleCrystalsPerTile)
				->Field("Advanced Mode", &Env_TileGenerator::advancedMode)
				->Field("Free Floating Gems", &Env_TileGenerator::unhiddenGems)
				->Field("Advanced Mode Crystal Slices", &Env_TileGenerator::AdvancedCrystalModels)
				->Field("Default Crystal Slice", &Env_TileGenerator::DefaultCrystalModel)
				/*->Field("Hidden Crystal Slices", &Env_TileGenerator::hiddenAdvancedCrystalModels)
				->Field("Maximum Crystals", &Env_TileGenerator::maxCrystals)				
				*/

				//Landing Zone
				->Field("Landing Zone-Enable", &Env_TileGenerator::enableLZ)
				->Field("Landing Zone-Base Tile", &Env_TileGenerator::LandingZoneBaseSlice)
				->Field("Landing Zone-Deco Layer", &Env_TileGenerator::LandingZoneDecoSlices)
				->Field("Landing Zone-Override Index", &Env_TileGenerator::lz_tile_index_override)
				;

			if (AZ::EditContext* ec = serialize->GetEditContext())
			{
				ec->Class<Env_TileGenerator>("Env_Generator v2", "Generates environment tiles as dynamic slices")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
						->Attribute(AZ::Edit::Attributes::Category, "Environment Tile System")
						->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game"))
						->Attribute(AZ::Edit::Attributes::AutoExpand, true)
						->Attribute(AZ::Edit::Attributes::Icon, "Editor/Icons/Components/StaticMesh")
						->Attribute(AZ::Edit::Attributes::ViewportIcon, "Editor/Icons/Components/Viewport/StaticMesh.png")

					->ClassElement(GRP, "Generator PCG")
						->Attribute(AZ::Edit::Attributes::AutoExpand, true)

						->DataElement(UI_SP, &Env_TileGenerator::maxRowSize, "Max Row Length", "Maximum row length before starting a new row. Ignored for Manual mode.")
							->Attribute("Min", 1)
							->Attribute("Max", 20)//Subject to change. Dimension of 20 seems small...
							->Attribute("Step", 1)
						->DataElement(UI_SP, &Env_TileGenerator::maxTiles, "Max Tiles", "Maximum number of Tiles to generate. Ignored if not using Spawn Repetition.")
							->Attribute("Min", 1)

						//Spawn Method and Repetition Selection
						->DataElement(UI_CB, &Env_TileGenerator::sp_Method, "Spawning Method", "Choose how to spawn slices.")
							->EnumAttribute(Env_TileGenerator::spawnMethod::Randomized, "Randomized")
							->EnumAttribute(Env_TileGenerator::spawnMethod::Ordered, "Ordered")
							->EnumAttribute(Env_TileGenerator::spawnMethod::Manual, "Manual")

						->DataElement(UI_CB, &Env_TileGenerator::sp_Type, "Spawn Repetition", "Choose whether or not to spawn slices repeatedly.")
							->EnumAttribute(Env_TileGenerator::spawnType::Once, "Once")
							->EnumAttribute(Env_TileGenerator::spawnType::Repeat, "Repeat")

					//Constant Transform Offset
					->ClassElement(GRP, "Constant Offset")
						->DataElement(UI_SL, &Env_TileGenerator::xOffset, "Grid X Offset", "Offset amount to accumulate on the global X axis for each consecutive slice.")
						->DataElement(UI_SL, &Env_TileGenerator::yOffset, "Grid Y Offset", "Offset amount to accumulate on the global Y axis for each consecutive slice.")

					//Per Instance Local Transform Offset
					->ClassElement(GRP, "Local Transform Offset")
						->DataElement(UI_SP, &Env_TileGenerator::listIndex, "Tile Index", "Tile which is to have its properties modified. [Manual Mode only]")
							->Attribute("Min", 0)
							->Attribute("Max", &Env_TileGenerator::listSize)
							->Attribute(CN, &Env_TileGenerator::onListIndexChanged)

						->DataElement(UI_D, &Env_TileGenerator::offloc, "Location Offset", "Amount to adjust tile location from grid spawn location.")
							->Attribute(CN, &Env_TileGenerator::onLocalTransformChanged)
						->DataElement(UI_D, &Env_TileGenerator::offrot, "Rotation Offset", "Amount to adjust tile rotation.")
							->Attribute(CN, &Env_TileGenerator::onLocalTransformChanged)
						->DataElement(UI_D, &Env_TileGenerator::offscale, "Additional Scaling", "Amount to adjust tile scale.")
							->Attribute(CN, &Env_TileGenerator::onLocalTransformChanged)

					//Need to have 2D list of compatible slices [TODO]


					//Simple Weather Section
					->ClassElement(GRP, "Simple Weather")
						->Attribute(AZ::Edit::Attributes::AutoExpand, false)
						//Cloudy
						->DataElement(UI_D, &Env_TileGenerator::Cloudy, "Cloudy", "Allows simple, cloudy weather if checked.")
						//Windy
						->DataElement(UI_D, &Env_TileGenerator::Windy, "Windy", "Allows simple, windy weather if checked.")

					//Rain Section
					->ClassElement(GRP, "Complex: Rain")
							->Attribute(AZ::Edit::Attributes::AutoExpand, false)
						//Intensity
						->DataElement(UI_D, &Env_TileGenerator::rain_strength_light, "Light Showers", "Allows tile to generate light rain showers.")
							->Attribute(CN, &Env_TileGenerator::checkRainEnabled)

						->DataElement(UI_D, &Env_TileGenerator::rain_strength_medium, "Regular Showers", "Allows tile to generate regular rain showers.")
							->Attribute(CN, &Env_TileGenerator::checkRainEnabled)

						->DataElement(UI_D, &Env_TileGenerator::rain_strength_heavy, "Heavy Storm Showers", "Allows tile to generate heavy storm rain.")
							->Attribute(CN, &Env_TileGenerator::checkRainEnabled)

						//Extras
						->DataElement(UI_D, &Env_TileGenerator::rain_extra_Thunder, "Thunder", "Allows thunder when rain is enabled.")
						->DataElement(UI_D, &Env_TileGenerator::rain_extra_Lightning, "Lightning", "Allows lightning when rain is enabled.")
						//TODO: FIGURE OUT HOW TO ADD A SOUND INPUT HERE FOR THUNDER SOUND

						//RAIN_TOD
						->DataElement(UI_SL, &Env_TileGenerator::rain_TOD_Range_Start, "TOD_Start", "The Time of Day value at which this weather effect will begin to be applicable.")
							->Attribute(AZ::Edit::Attributes::Min, 0)
							->Attribute(AZ::Edit::Attributes::Max, 24)
							->Attribute(AZ::Edit::Attributes::Step, 1)
							->Attribute(CN, &Env_TileGenerator::OnRainStartTODChanged)

						->DataElement(UI_SL, &Env_TileGenerator::rain_TOD_Range_Stop, "TOD_Stop", "The Time of Day value at which this weather effect will no longer be applicable.")
							->Attribute(AZ::Edit::Attributes::Min, 0)
							->Attribute(AZ::Edit::Attributes::Max, 24)
							->Attribute(AZ::Edit::Attributes::Step, 1)
							->Attribute(CN, &Env_TileGenerator::OnRainStopTODChanged)

						//Snowfall Section
						->ClassElement(GRP, "Complex: Snowfall")
							->Attribute(AZ::Edit::Attributes::AutoExpand, false)
						//Intensity
						->DataElement(UI_D, &Env_TileGenerator::snow_strength_light, "Light Snowfall", "Allows tile to generate light snowfall.")
						->DataElement(UI_D, &Env_TileGenerator::snow_strength_medium, "Regular Snowfall", "Allows tile to generate regular snowfall.")
						->DataElement(UI_D, &Env_TileGenerator::snow_strength_heavy, "Heavy Snowfall", "Allows tile to generate heavy snowfall.")
						//Extras
						->DataElement(UI_D, &Env_TileGenerator::snow_extra_FreezeGround, "Freeze Ground", "Will render snow overlays on terrain mesh. NOTE: This will not affect static meshes.")

						->DataElement(UI_SL, &Env_TileGenerator::snow_extra_FreezeAmount, "Freeze Amount", "Influence of the freezing on the terrain when Freeze Ground is enabled.")
							->Attribute(AZ::Edit::Attributes::Min, 0.0f)
							->Attribute(AZ::Edit::Attributes::Max, 1.0f)
							->Attribute(AZ::Edit::Attributes::Step, 0.01f)

						//SNOW_TOD
						->DataElement(UI_SL, &Env_TileGenerator::snow_TOD_Range_Start, "TOD_Start", "The Time of Day value at which this weather effect will begin to be applicable.")
							->Attribute(AZ::Edit::Attributes::Min, 0)
							->Attribute(AZ::Edit::Attributes::Max, 24)
							->Attribute(AZ::Edit::Attributes::Step, 1)
							->Attribute(CN, &Env_TileGenerator::OnSnowStartTODChanged)

						->DataElement(UI_SL, &Env_TileGenerator::snow_TOD_Range_Stop, "TOD_Stop", "The Time of Day value at which this weather effect will no longer be applicable.")
							->Attribute(AZ::Edit::Attributes::Min, 0)
							->Attribute(AZ::Edit::Attributes::Max, 24)
							->Attribute(AZ::Edit::Attributes::Step, 1)
							->Attribute(CN, &Env_TileGenerator::OnSnowStopTODChanged)
					//Seperate Slice List for UI Visibiltiy Toggle Behavior for above sections
					->ClassElement(GRP, "Slice Lists")
						->DataElement(UI_D, &Env_TileGenerator::sliceList, "Base Slice List", "List of generated base slices")
						->Attribute(CN, &Env_TileGenerator::onListLengthChanged)
						//Decorative Layer
						->DataElement(UI_D, &Env_TileGenerator::decoLayer, "Decorative Layer Slice List", "Slices to spawn on top of base tile.")
						
					//Crystals
					->ClassElement(GRP, "Crystals")
						->DataElement(UI_D, &Env_TileGenerator::spawnCrystals, "Spawn Crystals", "Enables or disables the spawning of Collectible Crystals")
						->DataElement(UI_D, &Env_TileGenerator::multipleCrystalsPerTile, "Multi-Crystal Placement", "If true, then multiple gems can be spawned on the same tile.")
						->DataElement(UI_D, &Env_TileGenerator::advancedMode, "Advanced Mode", "If advanced mode, each player gets maxCrystals amount of Crystals in their color. Else maxCrystals is the total number of crystals for all players.")
						->DataElement(UI_D, &Env_TileGenerator::unhiddenGems, "Free Floating Crystals", "Number of Crystals to spawn out in the open dynamically.")
						->DataElement(UI_D, &Env_TileGenerator::DefaultCrystalModel, "Casual Crystal", "Slice used for the Casual Mode Crystal")
						->DataElement(UI_D, &Env_TileGenerator::AdvancedCrystalModels, "Advanced Crystals", "Slices used for Crystals (Max:4)")
					
					//Landing Zone
					->ClassElement(GRP, "Landing Zone")
						->DataElement(UI_D, &Env_TileGenerator::enableLZ, "Enable LZ", "If true then the Landing Zone Settings are applied and a base tile is overriden with the Landing Zone.")
						->DataElement(UI_D, &Env_TileGenerator::LandingZoneBaseSlice, "Base Slice", "The slice to use as the base tile slice for the landing zone tile.")
						->DataElement(UI_D, &Env_TileGenerator::LandingZoneDecoSlices, "Deco Slices", "The slices to spawn as a decorative layer for the landing zone tile.")
						->DataElement(UI_D, &Env_TileGenerator::lz_tile_index_override, "Tile Index Override", "The index of the tile which will be overridden by the landing zone slices.")
						/*	
						->DataElement(UI_D, &Env_TileGenerator::maxCrystals,"Max Crystals","Maximum number of crystals to spawn per player.")
						->Attribute(AZ::Edit::Attributes::Min, 0)
						
						->Attribute(AZ::Edit::Attributes::Min, 0)
						->DataElement(UI_D, &Env_TileGenerator::AdvancedCrystalModels, "Crystal Slices", "Slices used for Crystals (Max:4)")
						*/
					;

			}
		}
	}

	void Env_TileGenerator::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
	{
		provided.push_back(AZ_CRC("EnvTileService"));
	}

	void Env_TileGenerator::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
	{
		incompatible.push_back(AZ_CRC("EnvTileService"));
	}

	void Env_TileGenerator::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
	{
		(void)required;
	}

	void Env_TileGenerator::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
	{
		dependent.push_back(AZ_CRC("TransformService"));
	}
#pragma endregion Component

#pragma region UI and Node
	int Env_TileGenerator::getSimple() {
		//Output will be 0,1,2 or 3
		return (int)Cloudy + (2 * (int)Windy);
	}

	int Env_TileGenerator::getRainStrengths() {
		int result = 0;
		//Treat as binary bitmask (8bit)
		if (rain_strength_light) result += 1;
		if (rain_strength_medium)result += 2;
		if (rain_strength_heavy) result += 4;

		return result;
	}

	int Env_TileGenerator::getRainExtra() {
		return (int)rain_extra_Thunder + (2 * (int)rain_extra_Lightning);
	}

	int Env_TileGenerator::getRainTODStart() {
		return rain_TOD_Range_Start;
	}

	int Env_TileGenerator::getRainTODStop() {
		return rain_TOD_Range_Stop;
	}

	int Env_TileGenerator::getSnowStrengths() {
		int result = 0;
		//Same as rain: 8bit bitmask
		if (snow_strength_light) result += 1;
		if (snow_strength_medium)result += 2;
		if (snow_strength_heavy) result += 4;

		return result;
	}

	bool Env_TileGenerator::getSnowFreezeGround() {
		return snow_extra_FreezeGround;
	}

	float Env_TileGenerator::getSnowFreezeAmount() {
		return snow_extra_FreezeAmount;
	}

	int Env_TileGenerator::getSnowTODStart() {
		return snow_TOD_Range_Start;
	}

	int Env_TileGenerator::getSnowTODStop() {
		return snow_TOD_Range_Stop;
	}

	int Env_TileGenerator::setSimple(bool cloudy, bool windy) {
		Cloudy = cloudy;
		Windy = windy;
		return getSimple();
	}

	int Env_TileGenerator::setRainStrengths(bool low, bool mid, bool hi) {
		rain_strength_light = low;
		rain_strength_medium = mid;
		rain_strength_heavy = hi;
		return getRainStrengths();
	}

	int Env_TileGenerator::setRainExtra(bool thunder, bool lightning) {
		rain_extra_Thunder = thunder;
		rain_extra_Lightning = lightning;
		return getRainExtra();
	}

	int Env_TileGenerator::setRainTODStart(int val) {
		rain_TOD_Range_Start = val;
		return rain_TOD_Range_Start;
	}

	int Env_TileGenerator::setRainTODStop(int val) {
		rain_TOD_Range_Stop = val;
		return rain_TOD_Range_Stop;
	}

	int Env_TileGenerator::setSnowStrengths(bool low, bool mid, bool hi) {
		snow_strength_light = low;
		snow_strength_medium = mid;
		snow_strength_heavy = hi;
		return getSnowStrengths();
	}

	bool Env_TileGenerator::setSnowFreezeGround(bool freeze) {
		snow_extra_FreezeGround = freeze;
		return snow_extra_FreezeGround;
	}

	float Env_TileGenerator::setSnowFreezeAmount(float amt) {
		snow_extra_FreezeAmount = amt;
		return snow_extra_FreezeAmount;
	}

	int Env_TileGenerator::setSnowTODStart(int val) {
		snow_TOD_Range_Start = val;
		return snow_TOD_Range_Start;
	}

	int Env_TileGenerator::setSnowTODStop(int val) {
		snow_TOD_Range_Stop = val;
		return snow_TOD_Range_Stop;
	}

	///////////////////////////////////////////////////////////
	//UI NotifyFunctions
	///////////////////////////////////////////////////////////
	void Env_TileGenerator::checkRainEnabled() {
		if (rain_strength_light || rain_strength_medium || rain_strength_heavy)
			RAIN_ENABLED = true;
		else
			RAIN_ENABLED = false;
	}

	AZ::u32 Env_TileGenerator::OnRainStartTODChanged() {
		//Ensure bound integrity is maintained
		if (rain_TOD_Range_Start > rain_TOD_Range_Stop)
			rain_TOD_Range_Stop = rain_TOD_Range_Start;
		return AZ_CRC("RefreshValues");
	}

	AZ::u32 Env_TileGenerator::OnRainStopTODChanged() {
		//Ensure bound integrity is maintained
		if (rain_TOD_Range_Start > rain_TOD_Range_Stop)
			rain_TOD_Range_Start = rain_TOD_Range_Stop;
		return AZ_CRC("RefreshValues");
	}

	AZ::u32 Env_TileGenerator::OnSnowStartTODChanged() {
		//Ensure bound integrity is maintained
		if (snow_TOD_Range_Start > snow_TOD_Range_Stop)
			snow_TOD_Range_Stop = snow_TOD_Range_Start;
		return AZ_CRC("RefreshValues");
	}

	AZ::u32 Env_TileGenerator::OnSnowStopTODChanged() {
		//Ensure bound integrity is maintained
		if (snow_TOD_Range_Start > snow_TOD_Range_Stop)
			snow_TOD_Range_Start = snow_TOD_Range_Stop;
		return AZ_CRC("RefreshValues");
	}

	AZ::u32 Env_TileGenerator::onListIndexChanged() {
		//Show the selected slice's transform on editor panel
		CryLog("List Index Changed!");
		AZ::Transform t = sliceLocalTransforms[listIndex];
		offloc = t.GetPosition();
		//TODO: Extract rotation from transform.
		offscale = t.RetrieveScale();

		return AZ_CRC("RefreshAttributesAndValues");
	}

	AZ::u32 Env_TileGenerator::onListLengthChanged() {
		//Transform list must have same number of entries as the Slice List
		if (sliceList.size() > sliceLocalTransforms.size()) {
			//Added new slice entry -> add new transform
			AZ::Transform t = AZ::Transform::Identity();

			sliceLocalTransforms.push_back(t);
		}
		else if (sliceList.size() < sliceLocalTransforms.size()) {
			//Otherwise shrink the array down to the proper size and drop all entries with out of bounds indices
			sliceLocalTransforms.resize(sliceList.size());
			//BUG:(Potential): Deleting an entry from the middle may skew the transform list
		}
		return AZ_CRC("RefreshAttributesAndValues");
	}

	AZ::u32 Env_TileGenerator::onLocalTransformChanged() {
		//Update the local transform entry in the list when the panel entries for local transforms change.
		CryLog("Local Transform Changed!");
		AZ::Transform t = AZ::Transform::Identity();
		//Check order of transformation (necessary?)
		//t.SetRotationPartFromQuaternion(AZ::Quaternion::CreateFromVector3(offrot));//Need to think more on rotations...
		t.MultiplyByScale(offscale);
		t.SetPosition(offloc);
		sliceLocalTransforms[listIndex] = t;

		return AZ_CRC("RefreshValues");
	}
#pragma endregion UI and Node

#pragma region EBus
	///////////////////////////////////////////////////////////
	//EBUS Generator Functions
	///////////////////////////////////////////////////////////
	AzFramework::SliceInstantiationTicket Env_TileGenerator::Gen_SpawnSlice(const AZ::Data::Asset<AZ::Data::AssetData>& slice) {
		return SpawnSliceInternal(slice, AZ::Transform::Identity());
	}

	AzFramework::SliceInstantiationTicket Env_TileGenerator::Gen_SpawnSliceRelative(const AZ::Data::Asset<AZ::Data::AssetData>& slice, const AZ::Transform& relative) {
		return SpawnSliceInternal(slice, relative);
	}

	///////////////////////////////////////////////////////////
	//SliceInstantiationResultBus Functions
	///////////////////////////////////////////////////////////
	void Env_TileGenerator::OnSliceInstantiated(const AZ::Data::AssetId& sliceAssetId, const AZ::PrefabComponent::PrefabInstanceAddress& sliceAddress) {
		//Taken from SpawnerComponent: spawns all entities packaged in the slice.
		const AzFramework::SliceInstantiationTicket& ticket = (*AzFramework::SliceInstantiationResultBus::GetCurrentBusId());

		// Stop listening for this ticket (since it's done). We can have have multiple tickets in flight.
		AzFramework::SliceInstantiationResultBus::MultiHandler::BusDisconnect(ticket);

		const AZ::PrefabComponent::EntityList& entities = sliceAddress.second->GetInstantiated()->m_entities;

		entityIds.reserve(entities.size());
		tmp_spawned++;//Increment tile spawn counter
		CryLog("Objects Spawned: %i", tmp_spawned);
		for (AZ::Entity * currEntity : entities)
		{
			entityIds.push_back(currEntity->GetId());
			CryLog("Slice Instantiated");
		}
		int loopLength = sp_Type == spawnType::Once ? sliceList.size() : maxTiles;
		//int crystalCount = advancedMode ? maxCrystals*numPlayers : maxCrystals;
		if (tmp_spawned == loopLength+decoLayerObjectCount+1+LandingZoneDecoSlices.size())PostActivate();//Objects have spawned and it is safe to iterate.

		EBUS_EVENT_ID(GetEntityId(), Env_TileNotificationBus, OnSpawned, ticket, entityIds);
	}

	void Env_TileGenerator::OnSliceInstantiationFailed(const AZ::Data::AssetId& sliceAssetId) {
		AzFramework::SliceInstantiationResultBus::MultiHandler::BusDisconnect(*AzFramework::SliceInstantiationResultBus::GetCurrentBusId());
		AZ_Error("Env_TileGenerator", false, "Slice '%s' failed to instantiate", sliceAssetId.ToString<AZStd::string>().c_str());
	}
#pragma endregion EBus

#pragma region Component Overrides
	void Env_TileGenerator::Init() {}

	void Env_TileGenerator::Activate()
	{
		//Link the component to its bus
		const AZ::EntityId eid = GetEntityId();
		Env_GeneratorRequestBus::Handler::BusConnect(eid);

		//Get Sub-Object counts
		decoLayerObjectCount = 0;
		if (decoLayer.size() > 0) {
			for (auto i : decoLayer) {
				if (i.size() > 0) {
					for (auto j : i) {
						decoLayerObjectCount++;
					}
				}
			}
		}
		//Get number of loop iterations based on spawnType setting (Also the number of Base Tiles)
		int loopLength = sp_Type == spawnType::Once ? sliceList.size() : maxTiles;

		//Adjust LZ tile index to be within bounds
		if (lz_tile_index_override > loopLength)lz_tile_index_override = loopLength;
		else if (lz_tile_index_override < 0)lz_tile_index_override = 0;

		//Spawn logic triggered on activation
#pragma region Base
		AZ::Vector3 accumulatedOffset = AZ::Vector3::CreateZero();//Accumulates the offset used to determine grid position.

		//Loop through the list and spawn after constructing proper transforms.
		for (int i = 0; i < loopLength; i++) {

			bool isLZ = (i == lz_tile_index_override) && enableLZ ? true : false;

			AZ::Data::Asset<AZ::DynamicPrefabAsset> sliceToSpawn;
			AZ::Transform sliceTransform = AZ::Transform::Identity();

			//Select Slice
			if (isLZ) sliceToSpawn = LandingZoneBaseSlice;
			else if (sp_Method == spawnMethod::Randomized) {
				CryLog("DEBUG: Generating Random Int");
				srand((int)time(NULL));
				int rand_index = rand() % sliceList.size();
				CryLog("DEBUG: Generated random value is: %i", rand_index);
			}
			else sliceToSpawn = sliceList[i%sliceList.size()];

			//Construct Transform
			if (sp_Method == spawnMethod::Manual) {
				CryLog("Manual Mode: Will use Local Transform only.");
				sliceTransform = sliceLocalTransforms[i%sliceList.size()];
			}
			else {
				CryLog("Ordered Mode or Randomized Mode: Use Accumulating Constant Offset.");
				//Handle Row Switch
				if (i) {//Ignore initial iteration at grid space (0,0)
					if (i%maxRowSize == 0) {
						CryLog("Starting new row...");
						accumulatedOffset.SetX(0.0);
						accumulatedOffset.SetY(accumulatedOffset.GetY() + (float)yOffset);
						CryLog("Current X: %f", accumulatedOffset.GetX());
						CryLog("Current Y: %f", accumulatedOffset.GetY());
					}
					else {
						CryLog("Accumulating offset for current row");
						accumulatedOffset.SetX(accumulatedOffset.GetX() + (float)xOffset);
						CryLog("Current X: %f", accumulatedOffset.GetX());
						CryLog("Current Y: %f", accumulatedOffset.GetY());
					}
				}
				//Set the grid space location.
				sliceTransform.SetPosition(accumulatedOffset);

			}
			//Spawn Slice
			Gen_SpawnSliceRelative(sliceToSpawn, sliceTransform);
			tilePositions.push_back(sliceTransform.GetPosition());
			
			if(isLZ) CryLog("Spawned LZ Slice: %i", i);
			else CryLog("Spawned Base Slice: %i", i);
#pragma endregion Base

#pragma region Deco
			//Handle Decorative Slice Layer
			//NOTE: ALL SUB-SLICES ARE SPAWNED. TO BYPASS THIS FOR VARIATIONS OF A SINGLE MODEL,
			//	REPEAT THE SLICES IN THE BASE SLICE LIST
			int tmp_sub_obj_count = 0;

			if (isLZ) {
				for (auto item : LandingZoneDecoSlices) {
					Gen_SpawnSliceRelative(item, sliceTransform);
					CryLog("LZ Tile %i: Spawned Sub Slice %i", i, tmp_sub_obj_count);
					tmp_sub_obj_count++;
				}
			}
			else {
				if (decoLayer.size() > 0 && decoLayer[i%sliceList.size()].size() > 0) {
					for (auto item : decoLayer[i%sliceList.size()]) {
						//Spawn at Tile Location (NOTE: BUILD SLICES ACCORDINGLY)
						Gen_SpawnSliceRelative(item, sliceTransform);
						CryLog("Base Tile %i: Spawned Sub Slice %i", i, tmp_sub_obj_count);
						tmp_sub_obj_count++;
					}
				}
			}
			
#pragma endregion Deco
		}



		//PostActivate();
	}

	void Env_TileGenerator::Deactivate()
	{
		Env_GeneratorRequestBus::Handler::BusDisconnect();
	}

#pragma endregion Component Overrides

#pragma region Helper Functions
	void Env_TileGenerator::PostActivate() {
		//Crystals
		int maxCrystals = advancedMode ? MAX_ADVANCED_CRYSTALS : MAX_CASUAL_CRYSTALS;
		CryLog("Max Crystals: %i", maxCrystals);
		int p1 = 0, p2 = 0, p3 = 0, p4 = 0;//Advanced mode only
		
		AZStd::vector<int> indices;
		for (int i = 0; i < entityIds.size(); i++)indices.push_back(i);
		std::random_shuffle(indices.begin(), indices.end());
		
		if (entityIds.size() < maxCrystals){
			CryLog("WARNING: Number of entities spawned is less than the number of Crystals requested! Capping Crystal Count to %i.", entityIds.size());
			maxCrystals = entityIds.size();
		}

		for (int i = 0; i < min(maxCrystals, (int)entityIds.size()); i++) {
			bool objIsCrystal = false;
			EBUS_EVENT_ID_RESULT(objIsCrystal, entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, isCrystal);
			if (objIsCrystal) {
				CryLog("Object is a Crystal!");
				if (advancedMode) {
					srand((int)time(NULL));
					int r = rand() % 4;
					bool validPlayerChosen = true;

					//Randomly pick which player's gem will spawn
					switch (r) {
					case 0:
						p1++;
						if (p1 > 4) {
							validPlayerChosen = false;
						}
						else {
							if (AdvancedCrystalModels.size() >= 1) {
								EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setCrystalModel, AdvancedCrystalModels[0]);
								EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setEnabled, true);
								if (unhiddenGems > 0) {
									EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setHidden, false);
									unhiddenGems--;
								}
								else EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setHidden, true);
							}
							else CryLog("Warning: Not enough Crystal Slice Models Provided!");
						}
						break;
					case 1:
						p2++;
						if (p2 > 4) {
							validPlayerChosen = false;
						}
						else {
							if (AdvancedCrystalModels.size() >= 2) {
								EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setCrystalModel, AdvancedCrystalModels[1]);
								EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setEnabled, true);
								if (unhiddenGems > 0) {
									EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setHidden, false);
									unhiddenGems--;
								}
								else EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setHidden, true);
							}
							else CryLog("Warning: Not enough Crystal Slice Models Provided!");
						}
						break;
					case 2:
						p3++;
						if (p3 > 4) {
							validPlayerChosen = false;
						}
						else {
							if (AdvancedCrystalModels.size() >= 3) {
								EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setCrystalModel, AdvancedCrystalModels[2]);
								EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setEnabled, true);
								if (unhiddenGems > 0) {
									EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setHidden, false);
									unhiddenGems--;
								}
								else EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setHidden, true);
							}
							else CryLog("Warning: Not enough Crystal Slice Models Provided!");
						}
						break;
					case 3:
						p4++;
						if (p4 > 4) {
							validPlayerChosen = false;
						}
						else {
							if (AdvancedCrystalModels.size() >= 4) {
								EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setCrystalModel, AdvancedCrystalModels[3]);
								EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setEnabled, true);
								if (unhiddenGems > 0) {
									EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setHidden, false);
									unhiddenGems--;
								}
								else EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setHidden, true);
							}
							else CryLog("Warning: Not enough Crystal Slice Models Provided!");
						}
						break;
					}

					//If the random method didn't work, give up and go in order
					if (!validPlayerChosen) {
						if (p1 < 4) {
							if (AdvancedCrystalModels.size() >= 1) {
								EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setCrystalModel, AdvancedCrystalModels[0]);
								EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setEnabled, true);
								if (unhiddenGems > 0) {
									EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setHidden, false);
									unhiddenGems--;
								}
								else EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setHidden, true);
							}
							else CryLog("Warning: Not enough Crystal Slice Models Provided!");
						}
						else if (p2 < 4) {
							if (AdvancedCrystalModels.size() >= 2) {
								EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setCrystalModel, AdvancedCrystalModels[1]);
								EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setEnabled, true);
								if (unhiddenGems > 0) {
									EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setHidden, false);
									unhiddenGems--;
								}
								else EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setHidden, true);
							}
							else CryLog("Warning: Not enough Crystal Slice Models Provided!");
						}
						else if (p3 < 4) {
							if (AdvancedCrystalModels.size() >= 3) {
								EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setCrystalModel, AdvancedCrystalModels[2]);
								EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setEnabled, true);
								if (unhiddenGems > 0) {
									EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setHidden, false);
									unhiddenGems--;
								}
								else EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setHidden, true);
							}
							else CryLog("Warning: Not enough Crystal Slice Models Provided!");
						}
						else if (p4 < 4) {
							if (AdvancedCrystalModels.size() >= 4) {
								EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setCrystalModel, AdvancedCrystalModels[3]);
								EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setEnabled, true);
								if (unhiddenGems > 0) {
									EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setHidden, false);
									unhiddenGems--;
								}
								else EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setHidden, true);
							}
							else CryLog("Warning: Not enough Crystal Slice Models Provided!");
						}
						else CryLog("If this message is printed, there is a problem in Env_TileGenerator::PostActivate.");
					}

					//Spawning should be fine I think. Not sure what happens if no slice is specified
					EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, Spawn);
				}
				else {
					//Basic Mode: Just spawn gems anywhere. Any player can get any gem.
					for (int i = 0; i < maxCrystals; i++){
						EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setCrystalModel, DefaultCrystalModel);
						EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setEnabled, true);
						if (unhiddenGems > 0){
							EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setHidden, false);
						}
						else EBUS_EVENT_ID(entityIds[indices[i]], DragonCrashCollectibles::CrystalRequestBus, setHidden, true);
					}

				}
				crystalsSpawned++;
			}
			else {
				CryLog("Object is not a Crystal!");
			}
			
		}

		/*while (crystalsSpawned < maxCrystals) {
			reserved = false;
			srand((int)time(NULL));
			r = rand() % entityIds.size();
			for (auto num : reservedNumbers) {
				if (num == r) { reserved = true; break; }
			}
			if (reserved) {
				CryLog("Reserved Number Picked");
				crystalsSpawned++;
				continue;
			}
			else {
				reservedNumbers.push_back(r);
				bool objIsCrystal = false;
				EBUS_EVENT_ID_RESULT(objIsCrystal, entityIds[r], DragonCrashCollectibles::CrystalRequestBus, isCrystal);
				//EBUS_EVENT_ID(entityIds[r], DragonCrashCollectibles::CrystalRequestBus, setEnabled, true);
				if (objIsCrystal) {
					CryLog("Object is a Crystal!! Index %i", r);
					crystalsSpawned++;
				}
				else {
					CryLog("Object is not a Crystal!! Index %i", r);
					crystalsSpawned++;
				}
				//reservedNumbers.push_back(r);

			}
		}*/
	}

	void Env_TileGenerator::preloadTriggersAtTime(int tod) {
		/*CryLog("Loading Triggers for Time: %i", tod);
		for (WeatherUnit w : env_weather_trigger[tod].triggerList){
		queueTriggers.push_back(w);//Manually push back instead of using op= to copy
		}
		CryLog("Loaded %i Triggers", env_weather_trigger[tod].triggerList.size());*/
	}

	WeatherUnit Env_TileGenerator::processNextTrigger() {

		//[REPLACE LIST LOADING WITH ITERATOR IMPLEMENTATION]

		//NOTE: Must check that the container is not empty BEFORE using this function
		WeatherUnit w = queueTriggers.front();
		queueTriggers.pop_front();
		return w;
	}

	AzFramework::SliceInstantiationTicket Env_TileGenerator::SpawnSliceInternal(const AZ::Data::Asset<AZ::Data::AssetData>& slice, const AZ::Transform& relative) {
		//Taken from SpawnerComponent

		AZ::Transform t = AZ::Transform::Identity();

		EBUS_EVENT_ID_RESULT(t, GetEntityId(), AZ::TransformBus, GetWorldTM);

		t *= relative;

		AzFramework::SliceInstantiationTicket ticket;

		EBUS_EVENT_RESULT(ticket, AzFramework::GameEntityContextRequestBus, InstantiateDynamicSlice, slice, t, nullptr);
		AzFramework::SliceInstantiationResultBus::MultiHandler::BusConnect(ticket);

		return ticket;
	}
#pragma endregion Helper Functions
}
/*for (int i = 0; i < 25; i++) { //[0-24]
WeatherTrigger tmp;
tmp.triggerTime = i;
env_weather_trigger.push_back(tmp);
}
CryLog("Initialize Weather Trigger List [Success]");
*/
//Construct List: one entry per hour (consider refining time gap to smaller increments)



//Initial pass to set up weatherlink vectors
//for (int i = 0; i < entityIds.size(); i++){
//CryLog("Entity Number %i", i);//[TODO] Add Tile number property

//Use EBUS to get Tile Parameters (Not sure how this will work for unsupported entites that cannot return a value)
/*	int simple = getSimple(),
r_strength = getRainStrengths(),
s_strength = getSnowStrengths(),
r_extra = getRainExtra();
bool s_extra_freeze = getSnowFreezeGround();
float s_extra_freeze_amount = getSnowFreezeAmount();
int r_todstart = getRainTODStart(),
r_todstop = getRainTODStop(),
s_todstart = getSnowTODStart(),
s_todstop = getSnowTODStop();*/
/*EBUS_EVENT_ID_RESULT(simple, entityIds[i], Env_Tile::Env_TileRequestBus, getSimple);
EBUS_EVENT_ID_RESULT(r_strength, entityIds[i], Env_Tile::Env_TileRequestBus, getRainStrengths);
EBUS_EVENT_ID_RESULT(s_strength, entityIds[i], Env_Tile::Env_TileRequestBus, getSnowStrengths);
EBUS_EVENT_ID_RESULT(r_extra, entityIds[i], Env_Tile::Env_TileRequestBus, getRainExtra);
EBUS_EVENT_ID_RESULT(s_extra_freeze, entityIds[i], Env_Tile::Env_TileRequestBus, getSnowFreezeGround);
EBUS_EVENT_ID_RESULT(s_extra_freeze_amount, entityIds[i], Env_Tile::Env_TileRequestBus, getSnowFreezeAmount);
EBUS_EVENT_ID_RESULT(r_todstart, entityIds[i], Env_Tile::Env_TileRequestBus, getRainTODStart);
EBUS_EVENT_ID_RESULT(r_todstop, entityIds[i], Env_Tile::Env_TileRequestBus, getRainTODStop);
EBUS_EVENT_ID_RESULT(s_todstart, entityIds[i], Env_Tile::Env_TileRequestBus, getSnowTODStart);
EBUS_EVENT_ID_RESULT(s_todstop, entityIds[i], Env_Tile::Env_TileRequestBus, getSnowTODStop);
*/
//Set trigger properties and attach indices after extracting flags
//RAIN
/*if (r_strength > 0){//Rain is enabled
WeatherUnit tmp_start, tmp_end;

tmp_start.start = true;
tmp_end.start = false;

//Simple Weather Addition
if (simple & 1){
tmp_start.t_cloud = 1;
tmp_end.t_cloud = 1;
}
if (simple & 2){
tmp_start.t_wind = 1;
tmp_end.t_wind = 1;
}

//Rain flags
tmp_start.t_rain = r_strength;
tmp_end.t_rain = r_strength;

if (r_extra & 1){
tmp_start.t_rain_extra_thunder = 1;
tmp_end.t_rain_extra_thunder = 1;
}
if (r_extra & 2){
tmp_start.t_rain_extra_lightning = 1;
tmp_end.t_rain_extra_lightning = 1;
}


//Tile
tmp_start.tileNumber = i;
tmp_end.tileNumber = i;

//Insert at TOD value
env_weather_trigger[r_todstart].triggerList.push_back(tmp_start);
env_weather_trigger[r_todstop].triggerList.push_back(tmp_end);

}

//SNOW
if (s_strength > 0){//Snow is enabled
WeatherUnit tmp_start, tmp_end;

tmp_start.start = true;
tmp_end.start = false;

//Simple Weather Addition
if (simple & 1){
tmp_start.t_cloud = 1;
tmp_end.t_cloud = 1;
}
if (simple & 2){
tmp_start.t_wind = 1;
tmp_end.t_wind = 1;
}

//Snow flags
tmp_start.t_snow = s_strength;
tmp_end.t_snow = s_strength;

tmp_start.t_snow_extra_freeze = s_extra_freeze;
tmp_start.t_snow_extra_f_amount = s_extra_freeze_amount;

tmp_end.t_snow_extra_freeze = s_extra_freeze;
tmp_end.t_snow_extra_f_amount = s_extra_freeze_amount;

//Tile
tmp_start.tileNumber = i;
tmp_end.tileNumber = i;

//Insert at TOD value
env_weather_trigger[s_todstart].triggerList.push_back(tmp_start);
env_weather_trigger[s_todstop].triggerList.push_back(tmp_end);
}
*/
//SIMPLE ONLY [TODO]: Add in Simple TOD option
/*if (r_strength==0 && s_strength==0 && simple > 0){
WeatherUnit tmp_start, tmp_end;

if (simple & 1){
tmp_start.t_cloud = 1;
tmp_end.t_cloud = 1;
}
if (simple & 2){
tmp_start.t_wind = 1;
tmp_end.t_wind = 1;
}


}*/
//}