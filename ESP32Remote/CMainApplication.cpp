#include "CMainApplication.h"



CMainApplication::CMainApplication() : wifiManager(222, this), lcdManager(this)
{
}


CMainApplication::~CMainApplication()
{
}

void CMainApplication::init()
{
	Serial.begin(56000);;

	if (!SD.begin(22))
	{
		Serial.println("Card Mount Failed");
		return;
	}

	Serial.println("IR_OK");

	if (!SD.exists("/root"))
		SD.mkdir("/root");


	//CWifiManager::init();

	lcdManager.init();
	wifiManager.init();

	addInstance(&lcdManager);
	addInstance(&wifiManager);

	registerEvent("saveIRSignal", static_cast<eventObject> (&CMainApplication::saveIRSignalEvent));
	registerEvent("loadIRSignal", static_cast<eventObject> (&CMainApplication::loadIRSignalEvent));
	registerEvent("sendIRSignal", static_cast<eventObject> (&CMainApplication::sendIRSignalEvent));
	registerEvent("recvIRSignal", static_cast<eventObject> (&CMainApplication::receiveIRSignalEvent));
	registerEvent("getBattCharge", static_cast<eventObject> (&CMainApplication::getBatteryChargeEvent));
	registerEvent("getIRSignalNameList", static_cast<eventObject> (&CMainApplication::getIRSignalNameListEvent)); //not on lcd diplay (command lenght > 14)
	registerEvent("removeIRSignal", static_cast<eventObject> (&CMainApplication::removeIRSignalEvent));

	//printDirectory();

	/*Serial.println(a.readStringUntil('\n'));

	Serial.println(a.readStringUntil('\n'));*/

	adc1_config_width(ADC_WIDTH_12Bit);
	adc1_config_channel_atten(BATTERY_ADC_CHANNEL, ADC_ATTEN_11db);

	if (!irControl.init(GPIO_NUM_21, GPIO_NUM_12, LEDC_CHANNEL_0, LEDC_TIMER_0))
	{
		PL("ir init error");

		return;
	}

	/*Serial.printf("this: %p\n", (void *) this);
	Serial.printf("irControl 1: %p\n", (void *)&irControl);
	Serial.printf("irControl 2: %p\n", (void *) &this->CMainApplication::irControl);*/
}

void CMainApplication::loop()
{
	CEventManager::loop();

	//irControl.getSignal(0);

	//saveIRSignal("third", &irControl);
	/*loadIRSignal("third", &irControl);

	Serial.println("sending");

	for (int i = 0; i < 5; i++)
	{
	irControl.sendSignal();

	delay(2000);
	}

	delay(100);*/
}

IRStatus CMainApplication::saveIRSignal(String _name, CIRControl * _irControl, bool _bOverwrite)
{
	File file;

	uint16_t lowValue,
		highValue;

	uint iCounter = 0;

	if (SD.exists("/root/" + _name) && !_bOverwrite)
		return IR_ALREADY_EXISTS;

	file = SD.open("/root/" + _name, FILE_WRITE);

	if (!file)
		return IR_ERROR;


	while (true)
	{
		if (!_irControl->getIRSignalValue(&highValue, &lowValue, iCounter))
			return IR_ERROR;

		file.print(String(lowValue) + "," + String(highValue) + ";");

		if (highValue == 0 || lowValue == 0)
			break;

		iCounter++;
	}

	file.close();

	return IR_OK;
}

IRStatus CMainApplication::loadIRSignal(String _name, CIRControl * _irControl)
{
	String readBuffer,
		tempString;

	File file;

	uint currentIndex = 0,
		tempIndex;

	uint16_t lowValue,
		highValue;

	uint8_t iCounter = 0;

	if (!SD.exists(("/root/" + _name).c_str()))
		return IR_NOT_FOUND;

	file = SD.open("/root/" + _name, FILE_READ);

	if (!file)
		return IR_ERROR;

	readBuffer = file.readString();

	file.close();

	while (currentIndex != readBuffer.length())
	{
		tempIndex = readBuffer.indexOf(';', currentIndex);

		tempString = readBuffer.substring(currentIndex, tempIndex);

		lowValue = (uint16_t)tempString.substring(0, tempString.indexOf(',')).toInt();
		highValue = (uint16_t)tempString.substring(tempString.indexOf(',') + 1).toInt();

		_irControl->setIRSignalValue(highValue, lowValue, iCounter);

		/*Serial.print(lowValue);
		Serial.print(", ");
		Serial.println(highValue);*/

		currentIndex = tempIndex + 1;

		iCounter++;
	}

	return IR_OK;
}

IRStatus CMainApplication::getIRSingnalNameList(vector<String> * _signalNameList)
{
	File rootDir = SD.open("/root");

	if (!rootDir)
		return IR_ERROR;

	String fileName;

	_signalNameList->clear();

	while (true)
	{

		File entry = rootDir.openNextFile();

		if (!entry)
			break;

		if (!entry.isDirectory())
		{
			fileName = entry.name();

			_signalNameList->push_back(fileName.substring(fileName.lastIndexOf('/') + 1));
		}

		entry.close();
	}

	rootDir.close();

	return IR_OK;
}

//0%:	1975
//100%: 2260

int16_t CMainApplication::getBatteryCharge()
{
	int16_t value = 0;

	for (int i = 0; i < 10; i++)
	{
		value += (float)20 / 57 * (float)adc1_get_voltage(BATTERY_ADC_CHANNEL) - (float)39500 / 57;

		delay(5);
	}

	return value / 10;
}

String CMainApplication::saveIRSignalEvent(String _eventData)
{
	bool bOverwite = false;

	if (_eventData.substring(_eventData.indexOf(':') + 1) == "true")
		bOverwite = true;

	PL("saveIR");

	//return IRStatusToString(static_cast<CMainApplication *>(_basePointer)->saveIRSignal(_eventData.substring(0, _eventData.indexOf(':')), &irControl, bOverwite));
	return IRStatusToString(saveIRSignal(_eventData.substring(0, _eventData.indexOf(':')), &irControl, bOverwite));
}

String CMainApplication::loadIRSignalEvent(String _eventData)
{
	PL("loadIR");

	return IRStatusToString(loadIRSignal(_eventData.substring(0, _eventData.indexOf(':')), &irControl));
}

String CMainApplication::sendIRSignalEvent(String _eventData)
{
	irControl.sendSignal();

	PL("sendIR");

	return IRStatusToString(IR_OK);
}

String CMainApplication::receiveIRSignalEvent(String _eventData)
{
	PL("receiveIR");

	return IRStatusToString(irControl.getSignal(_eventData.toInt()));
}

String CMainApplication::getBatteryChargeEvent(String _eventData)
{
	PL("getBatteryCharge");

	if (getBatteryCharge() < 0)
		return IRStatusToString(IR_ERROR);

	return ("IR_BATT_CHAR:" + String(getBatteryCharge()));
}

String CMainApplication::getIRSignalNameListEvent(String _eventData)
{
	vector<String> signalList;

	String signalListString = "";

	getIRSingnalNameList(&signalList);

	for (int i = 0; i < signalList.size(); i++)
		signalListString += String(signalList[i]) + ";";

	return "IR_SIGNAL_NAME_LIST: " + signalListString;
}

String CMainApplication::removeIRSignalEvent(String _eventData)
{
	if (!SD.exists("/root/" + _eventData))
		return IRStatusToString(IR_NOT_FOUND);

	if (SD.remove("/root/" + _eventData))
		return IRStatusToString(IR_OK);
	else
		return IRStatusToString(IR_ERROR);
}
