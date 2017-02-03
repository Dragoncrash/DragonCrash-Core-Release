
#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Prefab/PrefabAsset.h>

#include <DragonCrashCollectibles/DragonCrashCollectiblesBus.h>

namespace DragonCrashCollectibles
{
    class Crystal
        : public AZ::Component
        , protected CrystalRequestBus::Handler
    {
    public:
        AZ_COMPONENT(Crystal, "{8FD63DCF-BF41-4492-8CB9-08A900FBA063}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

    protected:
        ////////////////////////////////////////////////////////////////////////
        // DragonCrashCollectiblesRequestBus interface implementation
		bool getHidden() override;
		bool getEnabled() override;

		void setHidden(bool) override;
		void setEnabled(bool) override;
		void setCrystalModel(AZ::Data::Asset<AZ::DynamicPrefabAsset>) override;
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////
		bool enabled = false;
		bool hidden = true;
		bool forceLocalSlice = false;
		AZ::Data::Asset<AZ::DynamicPrefabAsset> crystalModel;//Replace in favor of regular spawner
		AZ::Data::Asset<AZ::DynamicPrefabAsset> staticModel;
		//Use Lua for more functionality

		//Keep track of spawned Entity IDs

    };

	
}
