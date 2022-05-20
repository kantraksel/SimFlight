#pragma once
#include "AirplaneData.h"
#include "FlightSimulatorState.h"

class SCClient
{
	public:
		struct AirplaneEvents
		{
			virtual void OnFlightStart() = 0;
			virtual void OnFlightEnd() = 0;
			virtual void OnUpdate(const AirplaneData& data) = 0;
		};

	private:
		void* hSimConnect;
		void* hEvent;
		bool isRunning;
		AirplaneEvents* pEvents;
		FlightSimulatorState fsState;

		bool DefineAirplaneData();
		void SimConnectDispatch(void* pData, unsigned long cbData);
		void UpdateFlightState(bool isRunning);

	public:
		SCClient();
		~SCClient();

		bool Connect();
		void Disconnnect();
		void RunLoop();

		void SetEvent(AirplaneEvents* pEvents);
};
