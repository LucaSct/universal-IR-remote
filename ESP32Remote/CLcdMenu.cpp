#include "CLcdMenu.h"


CLcdMenuInterface::CLcdMenuInterface(String _menuName)
{
	bUpdateRequired = true;

	menuName = _menuName;

	activeMenu = this;
	parentMenu = nullptr;
	lcd = nullptr;

}

CLcdMenuInterface::~CLcdMenuInterface()
{
}

bool CLcdMenuInterface::printCenteredString(String _string, bool _bSelected, uint8_t _row, uint8_t _endPosition, uint8_t _startPosition)
{
	return printCenteredString(_string, _bSelected, _row, _endPosition, lcd, _startPosition);
}

bool CLcdMenuInterface::printCenteredString(String _string, bool _bSelected, uint8_t _row, uint8_t _endPosition, LiquidCrystal* _lcd, uint8_t _startPosition)
{
	if (_lcd == nullptr)
		return false;

	if (_bSelected)
	{
		if (_endPosition + 1 - _startPosition < _string.length() + 2)
			return false;

		_lcd->setCursor(_startPosition, _row);
		_lcd->print('<');

		_lcd->setCursor(_startPosition + (_endPosition + 1 - _startPosition - _string.length()) / 2, _row);
		_lcd->print(_string);

		_lcd->setCursor(_endPosition, _row);
		_lcd->print('>');
	}
	else
	{
		if (_endPosition + 1 - _startPosition < _string.length())
			return false;

		_lcd->setCursor(_startPosition + (_endPosition + 1 - _startPosition - _string.length()) / 2, _row);
		_lcd->print(_string);
	}

	return true;
}

void CLcdMenuInterface::setActive()
{
	bUpdateRequired = true;

	activeMenu = this;
}

void CLcdMenuInterface::activateMenu(CLcdMenuInterface* _menu)
{
	activeMenu = _menu;

	activeMenu->setUpdateRequired();

	delay(50);
}

void CLcdMenuInterface::setLcdPointer(LiquidCrystal * _lcd)
{
	lcd = _lcd;
}

void CLcdMenuInterface::setParentMenuPointer(CLcdMenuInterface* _parentMenu)
{
	parentMenu = _parentMenu;
}



CLcdMenu::CLcdMenu(String _menuName, LiquidCrystal * _lcd) : CLcdMenuInterface(_menuName)
{
	iIndex = 0;

	btopSelected = true;

	lcd = _lcd;
}

CLcdMenu::~CLcdMenu()
{
}

void CLcdMenu::loop()
{
	if (activeMenu != this)
		activeMenu->loop();
	else if (bUpdateRequired)
	{
		if (menuList.size() == 0)
			return;

		lcd->clear();

		printCenteredString(menuList[iIndex]->menuName, btopSelected, 0, 15);

		if (iIndex + 1 < menuList.size() )
			printCenteredString(menuList[iIndex + 1]->menuName, !btopSelected, 1, 15);

		bUpdateRequired = false;
	}
}

void CLcdMenu::processButtonEvent(Buttons _button, CButtonManager::ButtonActions _buttonAction)
{
	if (activeMenu != this)
	{
		activeMenu->processButtonEvent(_button, _buttonAction);

		return;
	}

	if (_buttonAction != CButtonManager::BUTTON_CLICK_EVENT)
		return;

	switch (_button)
	{
		case BUTTON_UP:
			if (btopSelected)
			{
				if (decrementIndex())
					bUpdateRequired = true;
			}
			else
			{
				btopSelected = true;

				bUpdateRequired = true;
			}

			break;

		case BUTTON_DOWN:
			if (!btopSelected)
			{
				if (incrementIndex())
					bUpdateRequired = true;
			}
			else if (iIndex + 1 < menuList.size())
			{
				btopSelected = false;

				bUpdateRequired = true;
			}

			break;

		case BUTTON_OK:
			if (btopSelected && menuList[iIndex]->hasContent())
				activateMenu(menuList[iIndex]);
			else if (menuList[iIndex + 1]->hasContent())
				activateMenu(menuList[iIndex + 1]);

			bUpdateRequired = true;

			break;

		case BUTTON_LEFT:
			if (parentMenu != nullptr)
				parentMenu->setActive();

			break;
	}
}

void CLcdMenu::addElement(CLcdMenuInterface * _menu)
{
	_menu->setParentMenuPointer(this);
	_menu->setLcdPointer(lcd);

	menuList.push_back(_menu);
}

void CLcdMenu::removeElement(CLcdMenuInterface* _menu)
{
	for (int i = 0; i < menuList.size(); i++)
	{
		if (menuList[i] == _menu)
		{
			menuList.erase(menuList.begin() + i);

			break;
		}
	}
}

bool CLcdMenu::hasContent()
{
	if (menuList.size() == 0)
		return false;

	return true;
}

bool CLcdMenu::incrementIndex()
{
	if (iIndex + 2 >= menuList.size())
		return false;

	iIndex++;

	return true;
}

bool CLcdMenu::decrementIndex()
{
	if (iIndex <= 0)
		return false;

	iIndex--;

	return true;
}



CFunctionParameter::CFunctionParameter(String _menuName, Parameters _parameter) : CLcdMenuInterface(_menuName)
{
	parameter = _parameter;

	reset();
}

CFunctionParameter::~CFunctionParameter()
{
}

void CFunctionParameter::loop()
{
	if (bUpdateRequired)
	{
		lcd->clear();

		printCenteredString(menuName, false, 0, 15);

		if (parameter == PARAMETER_GET_NUMBER)
		{
			lcd->noCursor();

			printCenteredString(result, false, 1, 15);
		}
		else
		{
			lcd->setCursor(0, 1);
			lcd->print(result);
			lcd->setCursor(result.length() - 1, 1);
			lcd->cursor();
		}
			

		bUpdateRequired = false;
	}
}

void CFunctionParameter::reset()
{
	bDone = false;

	iCurrentNumber = 0;
	cCurrentChar = 0x61;

	bDone = false;

	if (parameter == PARAMETER_GET_NUMBER)
		result = "0";
	else
		result = "a";
}

void CFunctionParameter::processButtonEvent(Buttons _button, CButtonManager::ButtonActions _buttonAction)
{
	if (_buttonAction == CButtonManager::ButtonActions::BUTTON_HOLD_EVENT && (_button == BUTTON_UP || _button == BUTTON_DOWN))
	{
		processButtonEvent(_button, CButtonManager::ButtonActions::BUTTON_CLICK_EVENT);

		return;
	}
	else if (_buttonAction != CButtonManager::ButtonActions::BUTTON_CLICK_EVENT)
		return;

	if (_button == BUTTON_OK)
	{
		bDone = true;

		lcd->noCursor();

		parentMenu->setActive();

		bUpdateRequired = true;

		return;
	}

	if (parameter == PARAMETER_GET_STRING)
	{
		switch (_button)
		{
			case BUTTON_UP:
				if (cCurrentChar == 0x7A)
					cCurrentChar = 0x2F;
				else if (cCurrentChar == 0x39)
					cCurrentChar = 0x40;
				else if (cCurrentChar == 0x5A)
					cCurrentChar = 0x5E;

				cCurrentChar++;

				result[result.length() - 1] = cCurrentChar;

				bUpdateRequired = true;
				break;

			case BUTTON_DOWN:
				if (cCurrentChar == 0x30)
					cCurrentChar = 0x7B;
				else if (cCurrentChar == 0x41)
					cCurrentChar = 0x3A;
				else if (cCurrentChar == 0x5F)
					cCurrentChar = 0x5B;

				cCurrentChar--;

				result[result.length() - 1] = cCurrentChar;

				bUpdateRequired = true;
				break;

			case BUTTON_LEFT:
				if (result.length() == 1)
					return;

				result.remove(result.length() - 1, 1);

				cCurrentChar = result[result.length() - 1];

				bUpdateRequired = true;
				break;

			case BUTTON_RIGHT:
				if (result.length() == 16)
					return;

				result += (char) 0x61;

				cCurrentChar = 0x61;

				bUpdateRequired = true;
				break;
		}
	}
	else
	{
		switch (_button)
		{
			case BUTTON_UP:
				iCurrentNumber++;

				bUpdateRequired = true;
				break;

			case BUTTON_DOWN:
				if (iCurrentNumber <= 0)
					return;

				iCurrentNumber--;

				bUpdateRequired = true;
				break;
		}

		result = String(iCurrentNumber);
	}
}

bool CFunctionParameter::hasContent()
{
	return true;
}

bool CFunctionParameter::isDone()
{
	return bDone;
}

String CFunctionParameter::getResult()
{
	return result;
}



CLcdMenuFunction::CLcdMenuFunction(String _menuName, eventObject _event, CBaseEventInterface * _basePointer) : CLcdMenuInterface(_menuName), CBaseEventInterface(_basePointer)
{
	iIndex = 0;

	parameterResult = "";

	event = _event;
}

CLcdMenuFunction::~CLcdMenuFunction()
{
}

void CLcdMenuFunction::loop()
{
	if (activeMenu != this)
	{
		activeMenu->loop();
		
		return;
	}

	if (parameterList.size() == 0)
	{
		(basePointer->*event)(menuName);

		parentMenu->setActive();

		return;
	}

	if (parameterList[iIndex]->isDone())
	{
		parameterResult += ":" + parameterList[iIndex]->getResult();

		parameterList[iIndex]->reset();

		if (iIndex + 1 >= parameterList.size())
		{
			(basePointer->*event)(menuName + parameterResult);

			parameterResult = "";

			iIndex = 0;

			parentMenu->setActive();

			return;
		}

		iIndex++;
	}
	else
		activateMenu(parameterList[iIndex]);
}

void CLcdMenuFunction::processButtonEvent(Buttons _button, CButtonManager::ButtonActions _buttonAction)
{
	if (activeMenu != this)
		activeMenu->processButtonEvent(_button, _buttonAction);
}

void CLcdMenuFunction::addParameter(CFunctionParameter * _parameter)
{
	_parameter->setParentMenuPointer(this);
	_parameter->setLcdPointer(lcd);

	parameterList.push_back(_parameter);
}

void CLcdMenuFunction::removeParameter(CFunctionParameter * _parameter)
{
	for (int i = 0; i < parameterList.size(); i++)
	{
		if (parameterList[i] == _parameter)
		{
			parameterList.erase(parameterList.begin() + i);

			break;
		}
	}
}

bool CLcdMenuFunction::hasContent()
{
	return true;
}