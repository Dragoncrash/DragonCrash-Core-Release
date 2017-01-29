
#pragma once

#include <AzCore/Component/Component.h>

#include <DragonCrashCollectibles/DragonCrashCollectiblesBus.h>

namespace DragonCrashCollectibles
{
    class DragonCrashCollectiblesSystemComponent
        : public AZ::Component
        , protected DragonCrashCollectiblesRequestBus::Handler
    {
    public:
        AZ_COMPONENT(DragonCrashCollectiblesSystemComponent, "{8FD63DCF-BF41-4492-8CB9-08A900FBA063}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

    protected:
        ////////////////////////////////////////////////////////////////////////
        // DragonCrashCollectiblesRequestBus interface implementation

        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////
    };
}
