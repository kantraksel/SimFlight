#include "SCClient.h"
#include <string>
#include <format>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define SIMCONNECT_H_NOMANIFEST
#include "SimConnect/SimConnect.h"
#include "Logger.h"

enum DataDefinitionId
{
	DEFINITION_AIRPLANE_DATA,
};

enum DataRequestId
{
	REQUEST_AIRPLANE_DATA,
};

SCClient::SCClient()
{
	hSimConnect = NULL;
	hEvent = NULL;
	isRunning = false;
}

SCClient::~SCClient()
{
	if (hEvent) CloseHandle(hEvent);
	if (hSimConnect) SimConnect_Close(hSimConnect);
}

bool SCClient::Connect()
{
	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!hEvent)
		return false;

	HRESULT hResult = SimConnect_Open(&hSimConnect, "SimFlight", NULL, 0, hEvent, 0);
	if (SUCCEEDED(hResult))
	{
		if (!DefineAirplaneData()) return false;

		isRunning = true;
		return true;
	}

	return false;
}

bool SCClient::DefineAirplaneData()
{
#define LOGGER_ERROR1 "SCClient::Connect() - SimConnect_AddToDataDefinition("
#define LOGGER_ERROR2 ") FAILED"
#define LOG_ERROR(error) Logger::Log(LOGGER_ERROR1 error LOGGER_ERROR2)
#define DEFINE_VAR(name, unit) SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRPLANE_DATA, name, unit)
#define ADD_VAR(name, unit) if (FAILED(DEFINE_VAR(name, unit))) \
							{ \
								LOG_ERROR(name); \
								return false; \
							}

	ADD_VAR("PLANE LONGITUDE", "degrees");
	ADD_VAR("PLANE LATITUDE", "degrees");
	ADD_VAR("PLANE ALTITUDE", "feet");
	ADD_VAR("PLANE HEADING DEGREES MAGNETIC", "degrees");
	ADD_VAR("PLANE PITCH DEGREES", "degrees");
	ADD_VAR("PLANE BANK DEGREES", "degrees");
	//ADD_VAR("AIRSPEED TRUE", "knots");

	if (FAILED(SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_AIRPLANE_DATA, DEFINITION_AIRPLANE_DATA, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SIM_FRAME)))
	{
		Logger::Log("SCClient::Connect() - SimConnect_RequestDataOnSimObject() FAILED");
		return false;
	}
	return true;

#undef ADD_VAR
#undef DEFINE_VAR
#undef LOG_ERROR
#undef LOGGER_ERROR2
#undef LOGGER_ERROR1
}

void SCClient::Disconnnect()
{
	isRunning = false;
}

void SCClient::RunLoop()
{
	while (isRunning && WaitForSingleObject(hEvent, 1000) == WAIT_OBJECT_0)
	{
		SimConnect_CallDispatch(hSimConnect,
			[](SIMCONNECT_RECV* pData, DWORD cbData, void* pContext)
			{
				((SCClient*)pContext)->SimConnectDispatch(pData, cbData);
			},
			this);
	}
}

void SCClient::SimConnectDispatch(void* pData, unsigned long cbData)
{
	auto* pSCData = (SIMCONNECT_RECV*)pData;
	switch (pSCData->dwID)
	{
		case SIMCONNECT_RECV_ID_OPEN:
			break;

		case SIMCONNECT_RECV_ID_QUIT:
		{
			isRunning = false;
			Logger::Log("SCClient - Disconnected by Flight Simulator");
			break;
		}

		case SIMCONNECT_RECV_ID_SIMOBJECT_DATA:
		{
			auto* pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA*)pData;
			switch (pObjData->dwRequestID)
			{
				case REQUEST_AIRPLANE_DATA:
				{
					auto* pAirplaneData = (AirplaneData*)&pObjData->dwData;
					if (updateEvent) updateEvent(*pAirplaneData);
					break;
				}
			}
			break;
		}

		default:
		{
			Logger::Log(std::format("SCClient - Unknown data: dwID={} cbData={}", pSCData->dwID, cbData));
			break;
		}
	}
}

void SCClient::SetUpdateDataEvent(UpdateDataEvent updateEvent)
{
	this->updateEvent = updateEvent;
}
