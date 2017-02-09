#pragma once

#include <AzCore/EBus/EBus.h>

namespace DragonCrashController 
{
	class DragonCrashControllerRequests
		:public AZ::EBusTraits
	{
	public:
		static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
		static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::ById;
		using BusIdType = AZ::EntityId;

		//Primary Bus function for controller is to pass state
		enum States { dead, flight, hover, hover_zoom };//Expose the state enum for use with manager

		virtual int getState() = 0;
	};
	using DragonCrashControllerRequestBus = AZ::EBus<DragonCrashControllerRequests>;
}