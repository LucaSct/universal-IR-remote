#pragma once
#include <vector>

#include <driver/adc.h>

#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include "CEventManager.h"
#include "CWifiManager.h"
#include "CLcdManager.h"
#include "CIRControl.h"

#include "IRStatus.h"

#define BATTERY_ADC_CHANNEL ADC1_CHANNEL_3

#define PL(msg) Serial.println(msg);

using namespace std;

class CMainApplication : CEventManager
{
	public:
		CMainApplication();
		~CMainApplication();

		void init();
		void loop() override;

	private:

		CIRControl irControl;

		CLcdManager lcdManager;
		CWifiManager wifiManager;

		IRStatus getIRSingnalNameList(vector<String> * _signalNameList);

		IRStatus saveIRSignal(String _name, CIRControl * _irControl, bool _bOverwrite = false);
		IRStatus loadIRSignal(String _name, CIRControl * _irControl);

		int16_t getBatteryCharge();

		String saveIRSignalEvent(String _eventData);
		String loadIRSignalEvent(String _eventData);

		String sendIRSignalEvent(String _eventData);
		String receiveIRSignalEvent(String _eventData);

		String getBatteryChargeEvent(String _eventData);
		String getIRSignalNameListEvent(String _eventData);

		String removeIRSignalEvent(String _eventData);
};

