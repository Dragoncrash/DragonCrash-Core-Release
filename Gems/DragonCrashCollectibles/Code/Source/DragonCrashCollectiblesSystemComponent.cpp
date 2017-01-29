
#include "StdAfx.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>

#include "DragonCrashCollectiblesSystemComponent.h"

namespace DragonCrashCollectibles
{
    void DragonCrashCollectiblesSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<DragonCrashCollectiblesSystemComponent, AZ::Component>()
                ->Version(0)
                ->SerializerForEmptyClass();

            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<DragonCrashCollectiblesSystemComponent>("DragonCrashCollectibles", "[Description of functionality provided by this System Component]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        // ->Attribute(AZ::Edit::Attributes::Category, "") Set a category
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("System"))
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ;
            }
        }
    }

    void DragonCrashCollectiblesSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("DragonCrashCollectiblesService"));
    }

    void DragonCrashCollectiblesSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC("DragonCrashCollectiblesService"));
    }

    void DragonCrashCollectiblesSystemComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        (void)required;
    }

    void DragonCrashCollectiblesSystemComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        (void)dependent;
    }

    void DragonCrashCollectiblesSystemComponent::Init()
    {
    }

    void DragonCrashCollectiblesSystemComponent::Activate()
    {
        DragonCrashCollectiblesRequestBus::Handler::BusConnect();
    }

    void DragonCrashCollectiblesSystemComponent::Deactivate()
    {
        DragonCrashCollectiblesRequestBus::Handler::BusDisconnect();
    }
}
