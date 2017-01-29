
#include "StdAfx.h"
#include <platform_impl.h>

#include "EnvTile.h"

#include <IGem.h>

namespace EnvTile
{
    class Environment_Tile_System_v2Module
        : public CryHooksModule
    {
    public:
        AZ_RTTI(Environment_Tile_System_v2Module, "{5BB0007E-F494-4A53-AA2B-D127F7B26448}", CryHooksModule);

        Environment_Tile_System_v2Module()
            : CryHooksModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            m_descriptors.insert(m_descriptors.end(), {
                Env_TileGenerator::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<Env_TileGenerator>(),
            };
        }
    };
}

// DO NOT MODIFY THIS LINE UNLESS YOU RENAME THE GEM
// The first parameter should be GemName_GemIdLower
// The second should be the fully qualified name of the class above
AZ_DECLARE_MODULE_CLASS(Environment_Tile_System_v2_af4435841bcf4e6f8c84b7188a870b6e, EnvTile::Environment_Tile_System_v2Module)
