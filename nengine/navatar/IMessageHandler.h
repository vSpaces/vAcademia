
#pragma once

class IMessageObject;

class IMessageHandler
{
	virtual void ProcessMessage(std::string& command, IMessageObject* message) = 0;
};