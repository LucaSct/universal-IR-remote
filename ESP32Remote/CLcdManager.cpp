#include "CLcdManager.h"



CLcdManager::CLcdManager(CBaseEventInterface * _basePointer) : CBaseEventInterface(_basePointer), LiquidCrystal(32, 33, 25, 26, 27, 14), lcdMainMenu("", this)
{
	bILcdnitialized = false;
}


CLcdManager::~CLcdManager()
{
}

void CLcdManager::init()
{
	CButtonManager::init(ADC1_CHANNEL_7);

	addButtonWatch(new SButtonEventObject(0, 2815, 130, 20, 400, 70, static_cast<buttonEvent> (&CLcdManager::upButtonEventHandler)));
	addButtonWatch(new SButtonEventObject(1, 1812, 130, 20, 400, 70, static_cast<buttonEvent> (&CLcdManager::downButtonEventHandler)));
	addButtonWatch(new SButtonEventObject(2, 0, 130, 20, 400, 70, static_cast<buttonEvent> (&CLcdManager::rightButtonEventHandler)));
	addButtonWatch(new SButtonEventObject(3, 2470, 130, 20, 400, 70, static_cast<buttonEvent> (&CLcdManager::okButtonEventHandler)));
	addButtonWatch(new SButtonEventObject(4, 3060, 130, 20, 400, 70, static_cast<buttonEvent> (&CLcdManager::leftButtonEventHandler)));

	/*addButtonWatch(new SButtonEventObject(0, 2930, 100, 60, 400, 70, static_cast<buttonEvent> (&CLcdManager::upButtonEventHandler)));
	addButtonWatch(new SButtonEventObject(1, 1890, 100, 60, 400, 70, static_cast<buttonEvent> (&CLcdManager::downButtonEventHandler)));
	addButtonWatch(new SButtonEventObject(2, 0000, 100, 60, 400, 70, static_cast<buttonEvent> (&CLcdManager::rightButtonEventHandler)));
	addButtonWatch(new SButtonEventObject(3, 2575, 100, 60, 400, 70, static_cast<buttonEvent> (&CLcdManager::okButtonEventHandler)));
	addButtonWatch(new SButtonEventObject(4, 3175, 100, 60, 400, 70, static_cast<buttonEvent> (&CLcdManager::leftButtonEventHandler)));*/

	loadIRSignalMenu = new CLcdMenu("loadIRSignal", this);

	CLcdMenuFunction * sendIRSignalFunc = new CLcdMenuFunction("sendIRSignal", static_cast<eventObject> (&CLcdManager::lcdFunctionEvent), this);
	CLcdMenuFunction * recvIRSignalFunc = new CLcdMenuFunction("recvIRSignal", static_cast<eventObject> (&CLcdManager::lcdFunctionEvent), this);
	CLcdMenuFunction * saveIRSignalFunc = new CLcdMenuFunction("saveIRSignal", static_cast<eventObject> (&CLcdManager::lcdFunctionEvent), this);
	CLcdMenuFunction * getBattChargeFunc = new CLcdMenuFunction("getBattCharge", static_cast<eventObject> (&CLcdManager::lcdFunctionEvent), this);
	CLcdMenuFunction * removeIRSignalFunc = new CLcdMenuFunction("removeIRSignal", static_cast<eventObject> (&CLcdManager::lcdFunctionEvent), this);
	CLcdMenuFunction * renameIRSignalFunc = new CLcdMenuFunction("renameIRSignal", static_cast<eventObject> (&CLcdManager::lcdFunctionEvent), this);
	CLcdMenuFunction * deepSleepFunc = new CLcdMenuFunction("deepSleep", static_cast<eventObject> (&CLcdManager::lcdFunctionEvent), this);

	lcdMainMenu.addElement(sendIRSignalFunc);
	lcdMainMenu.addElement(loadIRSignalMenu);
	lcdMainMenu.addElement(recvIRSignalFunc);
	lcdMainMenu.addElement(saveIRSignalFunc);
	lcdMainMenu.addElement(getBattChargeFunc);
	lcdMainMenu.addElement(removeIRSignalFunc);
	lcdMainMenu.addElement(renameIRSignalFunc);
	lcdMainMenu.addElement(deepSleepFunc);

	recvIRSignalFunc->addParameter(new CFunctionParameter("set timeout (ms)", CFunctionParameter::PARAMETER_GET_NUMBER));
	sendIRSignalFunc->addParameter(new CFunctionParameter("repeat", CFunctionParameter::PARAMETER_GET_NUMBER));
	sendIRSignalFunc->addParameter(new CFunctionParameter("delay (ms)", CFunctionParameter::PARAMETER_GET_NUMBER));
	deepSleepFunc->addParameter(new CFunctionParameter("sleep time (s)", CFunctionParameter::PARAMETER_GET_NUMBER));
	deepSleepFunc->addParameter("true");

	saveIRSignalFunc->addParameter(new CFunctionParameter("signal name", CFunctionParameter::PARAMETER_GET_STRING));
	removeIRSignalFunc->addParameter(new CFunctionParameter("signal name", CFunctionParameter::PARAMETER_GET_STRING));
	renameIRSignalFunc->addParameter(new CFunctionParameter("from name", CFunctionParameter::PARAMETER_GET_STRING));
	renameIRSignalFunc->addParameter(new CFunctionParameter("to name", CFunctionParameter::PARAMETER_GET_STRING));

	pinMode(LCD_INTERFACE_ACTIVATE_PIN, INPUT);
}

void CLcdManager::loop()
{
	if (gpio_get_level(LCD_INTERFACE_ACTIVATE_PIN))
	{
		if (!bILcdnitialized)
		{
			delay(300);

			Serial.println("lcd");

			begin(16, 2);

			noCursor();
			noBlink();

			print("Hallo");

			bILcdnitialized = true;
		}

		/*setCursor(0, 1);

		print(adc1_get_voltage(ADC1_CHANNEL_7));

		delay(100);*/

		CButtonManager::loop();

		lcdMainMenu.loop();
	}
	else
		bILcdnitialized = false;
}

void CLcdManager::updateLoadMenu(std::vector<String>* _signalNameList)
{
	loadIRSignalMenu->clear();

	for (String signalName : *_signalNameList)
	{
		CLcdMenuFunction * tempFunc = new CLcdMenuFunction(signalName, static_cast<eventObject> (&CLcdManager::lcdFunctionEvent), this, "loadIRSignal");

		tempFunc->addParameter(signalName);

		loadIRSignalMenu->addElement(tempFunc);
	}

	loadIRSignalMenu->setUpdateRequired();
}

String CLcdManager::lcdFunctionEvent(String _eventData)
{
	clear();

	CLcdMenuInterface::printCenteredString(raiseEvent(_eventData.substring(0, _eventData.indexOf(":")), _eventData.substring(_eventData.indexOf(":") + 1)), false, 0, 15, this);

	delay(500);

	return "";
}

void CLcdManager::upButtonEventHandler(ButtonActions _buttonAction)
{
	lcdMainMenu.processButtonEvent(CLcdMenuInterface::BUTTON_UP, _buttonAction);
}

void CLcdManager::downButtonEventHandler(ButtonActions _buttonAction)
{
	lcdMainMenu.processButtonEvent(CLcdMenuInterface::BUTTON_DOWN, _buttonAction);
}

void CLcdManager::rightButtonEventHandler(ButtonActions _buttonAction)
{
	lcdMainMenu.processButtonEvent(CLcdMenuInterface::BUTTON_RIGHT, _buttonAction);
}

void CLcdManager::okButtonEventHandler(ButtonActions _buttonAction)
{
	lcdMainMenu.processButtonEvent(CLcdMenuInterface::BUTTON_OK, _buttonAction);
}

void CLcdManager::leftButtonEventHandler(ButtonActions _buttonAction)
{
	
	lcdMainMenu.processButtonEvent(CLcdMenuInterface::BUTTON_LEFT, _buttonAction);
}


