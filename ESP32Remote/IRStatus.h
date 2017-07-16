#pragma once

enum IRStatus
{
	IR_ERROR,
	IR_OK,
	IR_ALREADY_EXISTS,
	IR_NOT_FOUND,
	IR_CMD_NOT_RECOGNIZED
};

inline String IRStatusToString(IRStatus _status)
{
	switch (_status)
	{
		case IR_ERROR:
			return "IR_ERROR";
		case IR_ALREADY_EXISTS:
			return "IR_ALREADY_EXIST";
		case IR_NOT_FOUND:
			return "IR_NOT_FOUND";
		case IR_CMD_NOT_RECOGNIZED:
			return "IR_CMD_NOT_RECOG";
		default:
			return "IR_OK";
	}
}

