
#include "StdAfx.h"
#include <platform_impl.h>
#include <IGem.h>

#include "DragonCrashControllerComponent.h"

namespace DragonCrashController
{
    class DragonCrashControllerModule
        : public CryHooksModule
    {
    public:
        AZ_RTTI(DragonCrashControllerModule, "{E79F6CA0-68F4-4768-8CF4-F71B986AF3F8}", CryHooksModule);

        DragonCrashControllerModule()
            : CryHooksModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            m_descriptors.insert(m_descriptors.end(), {
				DragonCrashControllerComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<DragonCrashControllerComponent>(),
            };
        }
    };
}

// DO NOT MODIFY THIS LINE UNLESS YOU RENAME THE GEM
// The first parameter should be GemName_GemIdLower
// The second should be the fully qualified name of the class above
AZ_DECLARE_MODULE_CLASS(DragonCrashController_7662a13aee7e4f43a6cdf658973809c6, DragonCrashController::DragonCrashControllerModule)
