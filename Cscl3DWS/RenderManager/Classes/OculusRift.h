#pragma once

class COculusRift
{
public:
	COculusRift();
	~COculusRift();

	IOculusRiftSupport* GetOculusObject(IOculusRiftSupport::InitError& initError);

	void Update();

	void OnStop();
	void OnStart();

private:
	IOculusRiftSupport* m_oculus;
	bool m_isCtrlFound;
};