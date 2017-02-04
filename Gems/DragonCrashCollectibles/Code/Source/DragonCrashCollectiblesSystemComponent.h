
#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Prefab/PrefabAsset.h>
#include <AzCore/Prefab/PrefabAsset.h>
#include <AzCore/Math/Transform.h>

#include <DragonCrashCollectibles/DragonCrashCollectiblesBus.h>
#include <AzFramework/Entity/EntityContextBus.h>
#include <AzCore/Component/ComponentBus.h>

namespace DragonCrashCollectibles
{
    class Crystal
        : public AZ::Component
        , protected CrystalRequestBus::Handler
		, private AzFramework::SliceInstantiationResultBus::MultiHandler
    {
    public:
        AZ_COMPONENT(Crystal, "{8FD63DCF-BF41-4492-8CB9-08A900FBA063}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

		AzFramework::SliceInstantiationTicket Crystal::Crystal_SpawnSlice(const AZ::Data::Asset<AZ::Data::AssetData>& slice) override;
		AzFramework::SliceInstantiationTicket Crystal::Crystal_SpawnSliceRelative(const AZ::Data::Asset<AZ::Data::AssetData>& slice, const AZ::Transform& relative) override;
		void OnSliceInstantiated(const AZ::Data::AssetId& sliceAssetId, const AZ::PrefabComponent::PrefabInstanceAddress& sliceAddress) override;
		void OnSliceInstantiationFailed(const AZ::Data::AssetId& sliceAssetId) override;
		void Spawn() override;
    protected:
        ////////////////////////////////////////////////////////////////////////
        // DragonCrashCollectiblesRequestBus interface implementation
		bool getHidden() override;
		bool getEnabled() override;

		void setHidden(bool) override;
		void setEnabled(bool) override;
		void setCrystalModel(AZ::Data::Asset<AZ::DynamicPrefabAsset>) override;

		bool isCrystal() override { return true; }
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////
		bool enabled = false;
		bool hidden = true;
		AZ::Data::Asset<AZ::DynamicPrefabAsset> crystalModel;
		AZ::Data::Asset<AZ::DynamicPrefabAsset> staticModel;
		
	private:
		AzFramework::SliceInstantiationTicket SpawnSliceInternal(const AZ::Data::Asset<AZ::Data::AssetData>& slice, const AZ::Transform& relative);

    };

	
}
