
#include "StdAfx.h"
#include <platform_impl.h>

#include "DragonCrashGameManagerSystemComponent.h"

#include <IGem.h>

namespace DragonCrashGameManager
{
    class DragonCrashGameManagerModule
        : public CryHooksModule
    {
    public:
        AZ_RTTI(DragonCrashGameManagerModule, "{429E0BE3-0338-485A-A106-9E2572A4ADD0}", CryHooksModule);

        DragonCrashGameManagerModule()
            : CryHooksModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            m_descriptors.insert(m_descriptors.end(), {
                DragonCrashGameManagerSystemComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<DragonCrashGameManagerSystemComponent>(),
            };
        }
    };
}

// DO NOT MODIFY THIS LINE UNLESS YOU RENAME THE GEM
// The first parameter should be GemName_GemIdLower
// The second should be the fully qualified name of the class above
AZ_DECLARE_MODULE_CLASS(DragonCrashGameManager_4033e814e7914a12890b51838ead3cf9, DragonCrashGameManager::DragonCrashGameManagerModule)
