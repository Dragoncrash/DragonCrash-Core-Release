
#pragma once

#include <AzCore/Component/Component.h>
#include <EnvTile.h>
#include <DragonCrashControllerComponent.h>

#include <DragonCrashGameManager/DragonCrashGameManagerBus.h>
#include <DragonCrashCollectiblesBus.h>
#include <DragonCrashCollectiblesSystemComponent.h>
#include <EnvTileBus.h>

namespace DragonCrashGameManager
{
    class DragonCrashGameManagerSystemComponent
        : public AZ::Component
        , protected DragonCrashGameManagerRequestBus::Handler
    {
    public:
        AZ_COMPONENT(DragonCrashGameManagerSystemComponent, "{13C835EE-DC82-4661-86E0-08A9037895FC}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

    protected:
        ////////////////////////////////////////////////////////////////////////
        // DragonCrashGameManagerRequestBus interface implementation

        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////

		//Players
		static const int maxNumberOfPlayers = 4;

		AZStd::fixed_vector<AZ::EntityId,maxNumberOfPlayers> players;

		//Landing Zone
		AZ::EntityId LandingZone;

		


    };
}
