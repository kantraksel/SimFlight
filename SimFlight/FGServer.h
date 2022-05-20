#pragma once
#include "AirplaneData.h"
#include "DynamicBody.h"

class FGServer
{
	private:
		//MaxPacketSize = MTU(1500) - IP frame(24) - UDP header(8)
		static constexpr unsigned int MaxPacketSize = 1468;

		void* pSocket;
		unsigned char packetBuff[MaxPacketSize];
		DynamicBody body;

	public:
		FGServer();
		~FGServer();

		void SelectServer(const char* host, unsigned short port);
		void SetPeer(const char* callsign, const char* aircraft, const char* livery, long fallbackId);

		void ProcessTick(AirplaneData geoData);
};
