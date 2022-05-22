#include "SCClient.h"
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

enum EventId
{
	EVENT_SIM,
	EVENT_PAUSE,
};

SCClient::SCClient()
{
	hSimConnect = NULL;
	hEvent = NULL;
	isRunning = false;
	pEvents = nullptr;
}

SCClient::~SCClient()
{
	if (hEvent) CloseHandle(hEvent);
	if (hSimConnect) SimConnect_Close(hSimConnect);
}

bool SCClient::Connect()
{
	if (!pEvents)
		return false;

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

	ADD_VAR("AILERON LEFT DEFLECTION PCT", "percent over 100");
	ADD_VAR("AILERON RIGHT DEFLECTION PCT", "percent over 100");
	ADD_VAR("ELEVATOR DEFLECTION PCT", "percent over 100");
	ADD_VAR("RUDDER DEFLECTION PCT", "percent over 100");
	ADD_VAR("TRAILING EDGE FLAPS LEFT PERCENT", "percent over 100");
	ADD_VAR("SPOILERS LEFT POSITION", "percent over 100");
	//ADD_VAR("SPOILERS RIGHT POSITION", "percent over 100");

	ADD_VAR("ENG N1 RPM:1", "rpm");
	ADD_VAR("ENG N2 RPM:1", "rpm");
	ADD_VAR("ENG N1 RPM:2", "rpm");
	ADD_VAR("ENG N2 RPM:2", "rpm");
	ADD_VAR("ENG N1 RPM:3", "rpm");
	ADD_VAR("ENG N2 RPM:3", "rpm");
	ADD_VAR("ENG N1 RPM:4", "rpm");
	ADD_VAR("ENG N2 RPM:4", "rpm");

	ADD_VAR("GEAR TOTAL PCT EXTENDED", "percent");

	ADD_VAR("TRANSPONDER CODE:1", "BCO16");

	if (FAILED(SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_AIRPLANE_DATA, DEFINITION_AIRPLANE_DATA, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SIM_FRAME)))
	{
		Logger::Log("SCClient::Connect() - SimConnect_RequestDataOnSimObject() FAILED");
		return false;
	}

	if (FAILED(SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_SIM, "Sim")))
	{
		Logger::Log("SCClient::Connect() - SimConnect_SubscribeToSystemEvent(Sim) FAILED");
		return false;
	}

	if (FAILED(SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_PAUSE, "Pause")))
	{
		Logger::Log("SCClient::Connect() - SimConnect_SubscribeToSystemEvent(Pause) FAILED");
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
	while (isRunning)
	{
		if (WaitForSingleObject(hEvent, 1000) == WAIT_OBJECT_0)
		{
			SimConnect_CallDispatch(hSimConnect,
				[](SIMCONNECT_RECV* pData, DWORD cbData, void* pContext)
				{
					((SCClient*)pContext)->SimConnectDispatch(pData, cbData);
				},
				this);
		}
	}
}

static int HexToDecimal(int n)
{
	int o = 0;
	int f = 1;

	while (n != 0)
	{
		o += (n % 16) * f;
		n /= 16;
		f *= 10;
	}

	return o;
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
			Logger::Log("Flight Simulator has terminated connection!");
			fsState.Reset();
			pEvents->OnFlightEnd();
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
					pAirplaneData->squawkCode = HexToDecimal(pAirplaneData->squawkCode);
					pEvents->OnUpdate(*pAirplaneData);
					break;
				}
			}
			break;
		}

		case SIMCONNECT_RECV_ID_EVENT:
		{
			auto* pEventData = (SIMCONNECT_RECV_EVENT*)pData;

			switch (pEventData->uEventID)
			{
				case EVENT_SIM:
				{
					bool isSimulating = pEventData->dwData == 1;
					fsState.UpdateSimulationState(isSimulating, [&](bool isRunning)
						{
							UpdateFlightState(isRunning);
						});
					break;
				}

				case EVENT_PAUSE:
				{
					bool isPaused = pEventData->dwData == 1;
					fsState.UpdatePauseState(isPaused, [&](bool isRunning)
						{
							UpdateFlightState(isRunning);
						});
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

void SCClient::SetEvent(AirplaneEvents* pEvents)
{
	this->pEvents = pEvents;
}

void SCClient::UpdateFlightState(bool isRunning)
{
	if (isRunning)
		pEvents->OnFlightStart();
	else
		pEvents->OnFlightEnd();
}
