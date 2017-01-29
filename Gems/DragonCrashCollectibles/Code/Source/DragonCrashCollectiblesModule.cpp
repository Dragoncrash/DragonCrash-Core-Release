
#include "StdAfx.h"
#include <platform_impl.h>

#include "DragonCrashCollectiblesSystemComponent.h"

#include <IGem.h>

namespace DragonCrashCollectibles
{
    class DragonCrashCollectiblesModule
        : public CryHooksModule
    {
    public:
        AZ_RTTI(DragonCrashCollectiblesModule, "{9B0B1405-6066-4703-82D0-301B9922B316}", CryHooksModule);

        DragonCrashCollectiblesModule()
            : CryHooksModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            m_descriptors.insert(m_descriptors.end(), {
                DragonCrashCollectiblesSystemComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<DragonCrashCollectiblesSystemComponent>(),
            };
        }
    };
}

// DO NOT MODIFY THIS LINE UNLESS YOU RENAME THE GEM
// The first parameter should be GemName_GemIdLower
// The second should be the fully qualified name of the class above
AZ_DECLARE_MODULE_CLASS(DragonCrashCollectibles_2c15f8d9046b4d6ea9724aba01fe892a, DragonCrashCollectibles::DragonCrashCollectiblesModule)
