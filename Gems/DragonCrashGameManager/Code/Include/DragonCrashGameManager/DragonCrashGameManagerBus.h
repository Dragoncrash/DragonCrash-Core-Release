
#pragma once

#include <AzCore/EBus/EBus.h>

namespace DragonCrashGameManager
{
    class DragonCrashGameManagerRequests
        : public AZ::EBusTraits
    {

    public:
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        // Public functions
    };
    using DragonCrashGameManagerRequestBus = AZ::EBus<DragonCrashGameManagerRequests>;
} // namespace DragonCrashGameManager
