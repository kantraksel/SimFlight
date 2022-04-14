#pragma once
#include <functional>

class SCClient
{
	public:
		struct AirplaneData
		{
			double longitude;
			double latitude;

			double altitude;
			double heading;
			double pitch;
			double roll;
			//double speed;
		};

		typedef std::function<void(AirplaneData& data)> UpdateDataEvent;

	private:
		void* hSimConnect;
		void* hEvent;
		bool isRunning;
		UpdateDataEvent updateEvent;

		bool DefineAirplaneData();
		void SimConnectDispatch(void* pData, unsigned long cbData);

	public:
		SCClient();
		~SCClient();

		bool Connect();
		void Disconnnect();
		void RunLoop();

		void SetUpdateDataEvent(UpdateDataEvent event);
};
