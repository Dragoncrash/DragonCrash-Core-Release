
#include "StdAfx.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>

#include "DragonCrashGameManagerSystemComponent.h"

namespace DragonCrashGameManager
{
    void DragonCrashGameManagerSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<DragonCrashGameManagerSystemComponent, AZ::Component>()
                ->Version(0)
                ->SerializerForEmptyClass();

            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<DragonCrashGameManagerSystemComponent>("DragonCrashGameManager", "[Description of functionality provided by this System Component]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        // ->Attribute(AZ::Edit::Attributes::Category, "") Set a category
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("System"))
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ;
            }
        }
    }

    void DragonCrashGameManagerSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("DragonCrashGameManagerService"));
    }

    void DragonCrashGameManagerSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC("DragonCrashGameManagerService"));
    }

    void DragonCrashGameManagerSystemComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        (void)required;
    }

    void DragonCrashGameManagerSystemComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        (void)dependent;
    }

    void DragonCrashGameManagerSystemComponent::Init()
    {
    }

    void DragonCrashGameManagerSystemComponent::Activate()
    {
        DragonCrashGameManagerRequestBus::Handler::BusConnect(GetEntityId());
    }

    void DragonCrashGameManagerSystemComponent::Deactivate()
    {
        DragonCrashGameManagerRequestBus::Handler::BusDisconnect();
    }
}
