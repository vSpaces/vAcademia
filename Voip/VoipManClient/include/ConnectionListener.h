#pragma once

struct ivoiceconnectionlistender
{
	virtual void OnRecordConnectionLost(unsigned int auErrorCode) = 0;
	virtual void OnPlayConnectionLost() = 0;
	virtual void OnSelfRecordingConnectionLost() = 0;
	virtual void OnRecordConnectionRestored() = 0;
	virtual void OnPlayConnectionRestored() = 0;
	virtual void OnSelfRecordingConnectionRestored() = 0;
};