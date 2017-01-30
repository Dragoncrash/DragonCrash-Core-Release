
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
		bool isHidden() override;
		void setCrystalModel(AZ::Data::Asset<AZ::DynamicPrefabAsset>) override;
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////

		bool hidden = false;
		AZ::Data::Asset<AZ::DynamicPrefabAsset> crystalModel;
    };
}
