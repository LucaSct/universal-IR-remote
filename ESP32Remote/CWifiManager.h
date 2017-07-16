#pragma once

#include <vector>
#include <WiFi.h>

#include "CBaseEventInterface.h"

#include "IRStatus.h"

#define CLIENT_TEST_CONNECTION_INTERVAL 5000 //ms
#define CLIENT_TEST_CONNECTION_TIMEOUT 3000 //ms

using namespace std;


class CWifiManager : public CBaseEventInterface
{

	public:
		CWifiManager(uint16_t _port, CBaseEventInterface * _basePointer);
		virtual ~CWifiManager();

		void init();
		void loop() override;


	private:
		WiFiServer server;
		WiFiClient client;

		bool bClientConnected,
			 bClientConnectionNeedsUpdate;

		uint32_t iLastClientConnectionUpdateTime,
				 iClientConnectionUpdateTimoutStartTime;

		void eventParser();
};

