
#pragma once

#include <AzCore/EBus/EBus.h>

namespace DragonCrashCollectibles
{
    class DragonCrashCollectiblesRequests
        : public AZ::EBusTraits
    {

    public:
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        // Public functions
    };
    using DragonCrashCollectiblesRequestBus = AZ::EBus<DragonCrashCollectiblesRequests>;
} // namespace DragonCrashCollectibles
