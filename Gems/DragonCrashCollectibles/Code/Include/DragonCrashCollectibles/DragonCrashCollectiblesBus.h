
#pragma once

#include <AzCore/EBus/EBus.h>
#include <AzCore/Prefab/PrefabAsset.h>

namespace DragonCrashCollectibles
{
    class CrystalRequests
        : public AZ::EBusTraits
    {

    public:
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        // Public functions

		virtual bool getEnabled() = 0;
		virtual bool getHidden() = 0;

		virtual void setCrystalModel(AZ::Data::Asset<AZ::DynamicPrefabAsset>) = 0;
		virtual void setEnabled(bool) = 0;
		virtual void setHidden(bool) = 0;
    };
    using CrystalRequestBus = AZ::EBus<CrystalRequests>;
} // namespace DragonCrashCollectibles
