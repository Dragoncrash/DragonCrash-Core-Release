
#pragma once

#include <AzCore/EBus/EBus.h>

namespace DragonCrashCollectibles
{
    class CrystalRequests
        : public AZ::EBusTraits
    {

    public:
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        // Public functions
    };
    using CrystalRequestBus = AZ::EBus<CrystalRequests>;
} // namespace DragonCrashCollectibles
