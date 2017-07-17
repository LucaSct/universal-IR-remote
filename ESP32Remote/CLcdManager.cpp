#include "CLcdManager.h"



CLcdManager::CLcdManager(CBaseEventInterface * _basePointer) : CBaseEventInterface(_basePointer), lcd(32, 33, 25, 26, 27, 14), lcdMainMenu("", &lcd)
{
	bILcdnitialized = false;
}


CLcdManager::~CLcdManager()
{
}

void CLcdManager::init(std::vector<String> * _signalNameList)
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

	CLcdMenu * loadIRSignalMenu = new CLcdMenu("loadIRSignal", &lcd);

	for (String signalName : *_signalNameList)
	{
		CLcdMenuFunction * tempFunc = new CLcdMenuFunction(signalName, static_cast<eventObject> (&CLcdManager::lcdFunctionEvent), this, "loadIRSignal");

		tempFunc->addParameter(signalName);

		loadIRSignalMenu->addElement(tempFunc);
	}

	CLcdMenuFunction * sendIRSignalFunc = new CLcdMenuFunction("sendIRSignal", static_cast<eventObject> (&CLcdManager::lcdFunctionEvent), this);
	CLcdMenuFunction * recvIRSignalFunc = new CLcdMenuFunction("recvIRSignal", static_cast<eventObject> (&CLcdManager::lcdFunctionEvent), this);
	CLcdMenuFunction * saveIRSignalFunc = new CLcdMenuFunction("saveIRSignal", static_cast<eventObject> (&CLcdManager::lcdFunctionEvent), this);
	CLcdMenuFunction * getBattChargeFunc = new CLcdMenuFunction("getBattCharge", static_cast<eventObject> (&CLcdManager::lcdFunctionEvent), this);
	CLcdMenuFunction * removeIRSignalFunc = new CLcdMenuFunction("removeIRSignal", static_cast<eventObject> (&CLcdManager::lcdFunctionEvent), this);

	lcdMainMenu.addElement(sendIRSignalFunc);
	lcdMainMenu.addElement(loadIRSignalMenu);
	lcdMainMenu.addElement(recvIRSignalFunc);
	lcdMainMenu.addElement(saveIRSignalFunc);
	lcdMainMenu.addElement(getBattChargeFunc);
	lcdMainMenu.addElement(removeIRSignalFunc);

	recvIRSignalFunc->addParameter(new CFunctionParameter("set timeout (ms)", CFunctionParameter::PARAMETER_GET_NUMBER));
	sendIRSignalFunc->addParameter(new CFunctionParameter("repeat", CFunctionParameter::PARAMETER_GET_NUMBER));
	sendIRSignalFunc->addParameter(new CFunctionParameter("delay (ms)", CFunctionParameter::PARAMETER_GET_NUMBER));

	saveIRSignalFunc->addParameter(new CFunctionParameter("signal name", CFunctionParameter::PARAMETER_GET_STRING));
	removeIRSignalFunc->addParameter(new CFunctionParameter("signal name", CFunctionParameter::PARAMETER_GET_STRING));

	

	/*CLcdMenuFunction test(this);

	test.registerEvent("blabla", static_cast<eventObject> (&CLcdManager::blabla));*/

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

			lcd.begin(16, 2);

			lcd.noCursor();
			lcd.noBlink();

			lcd.print("Hallo");

			bILcdnitialized = true;
		}

		/*lcd.setCursor(0, 1);

		lcd.print(adc1_get_voltage(ADC1_CHANNEL_7));

		delay(100);*/

		CButtonManager::loop();

		lcdMainMenu.loop();
	}
	else
		bILcdnitialized = false;
}

String CLcdManager::lcdFunctionEvent(String _eventData)
{
	lcd.clear();

	//Serial.println(_eventData);

	//lcd.print(_eventData);

	CLcdMenuInterface::printCenteredString(raiseEvent(_eventData.substring(0, _eventData.indexOf(":")), _eventData.substring(_eventData.indexOf(":") + 1)), false, 0, 15, &lcd);

	delay(500);

	return "";
}

void CLcdManager::upButtonEventHandler(ButtonActions _buttonAction)
{
	//Serial.println("up");

	lcdMainMenu.processButtonEvent(CLcdMenuInterface::BUTTON_UP, _buttonAction);


	/*lcd.clear();

	if (_buttonAction == BUTTON_CLICK_EVENT)
	{
		lcd.print(raiseEvent("sendIRSignal", ""));

		delay(500);

		lcd.clear();
	}*/

	/*switch (_buttonAction)
	{
		case BUTTON_DOWN_EVENT:
			lcd.print("up: down event");
			break;
		case BUTTON_UP_EVENT:
			lcd.print("up: up event");
			break;
		case BUTTON_CLICK_EVENT:
			lcd.print("up: click event");
	}*/
}

void CLcdManager::downButtonEventHandler(ButtonActions _buttonAction)
{
	//Serial.println("down");

	lcdMainMenu.processButtonEvent(CLcdMenuInterface::BUTTON_DOWN, _buttonAction);

	/*lcd.clear();

	switch (_buttonAction)
	{
		case BUTTON_DOWN_EVENT:
			lcd.print("down: down event");
			break;
		case BUTTON_UP_EVENT:
			lcd.print("down: up event");
			break;
		default:
			lcd.print("down: click event");
	}*/
}

void CLcdManager::rightButtonEventHandler(ButtonActions _buttonAction)
{
	lcdMainMenu.processButtonEvent(CLcdMenuInterface::BUTTON_RIGHT, _buttonAction);

	/*lcd.clear();

	switch (_buttonAction)
	{
		case BUTTON_DOWN_EVENT:
			lcd.print("right: down event");
			break;
		case BUTTON_UP_EVENT:
			lcd.print("right: up event");
			break;
		default:
			lcd.print("right: click event");
	}*/
}

void CLcdManager::okButtonEventHandler(ButtonActions _buttonAction)
{
	lcdMainMenu.processButtonEvent(CLcdMenuInterface::BUTTON_OK, _buttonAction);

	/*lcd.clear();

	switch (_buttonAction)
	{
		case BUTTON_DOWN_EVENT:
			lcd.print("ok: down event");
			break;
		case BUTTON_UP_EVENT:
			lcd.print("ok up event");
			break;
		default:
			lcd.print("ok: click event");
	}*/
}

void CLcdManager::leftButtonEventHandler(ButtonActions _buttonAction)
{
	
	lcdMainMenu.processButtonEvent(CLcdMenuInterface::BUTTON_LEFT, _buttonAction);

	/*lcd.clear();

	switch (_buttonAction)
	{
		case BUTTON_DOWN_EVENT:
			lcd.print("left: down event");
			break;
		case BUTTON_UP_EVENT:
			lcd.print("left: up event");
			break;
		default:
			lcd.print("left: click event");
	}*/
}

//String CLcdManager::blabla(String _eventData)
//{
//}