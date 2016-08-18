
#pragma once

class CEngineWarnings
{
public:
	CEngineWarnings(CUserSettings* settings);
	~CEngineWarnings();

	std::vector<unsigned int>& GetIDs();

	void AddWarning(unsigned int id);

private:
	void AnalyzeIfNeeded();

	bool m_isAnalyzed;
	std::vector<unsigned int> m_ids;
	CUserSettings* m_settings;
};