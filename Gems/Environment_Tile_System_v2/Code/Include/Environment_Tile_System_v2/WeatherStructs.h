#pragma once
#include <AzCore/Component/Component.h> //Not sure where to import just AZStd so importing component

//Contains Structs used by the Weather System

//Struct for weather trigger pairings [TODO] [REDESIGN]
namespace EnvTile{
	struct WeatherUnit{
		//Accumulate weather flags and associate to tile indices
		unsigned int t_wind : 1, t_cloud : 1, t_rain : 3,
		t_snow : 3, t_rain_extra_thunder : 1, t_rain_extra_lightning : 1,
			 t_snow_extra_freeze : 1;

		float t_snow_extra_f_amount;
		int tileNumber;
		bool start; //Start or stop effect
	};

	struct WeatherTrigger{
		int triggerTime;//In hours [0-24]
		//24 is never actually reached since TOD ends at 23.99. Use 24 for full day effects.
		AZStd::vector<WeatherUnit> triggerList;
	};
}