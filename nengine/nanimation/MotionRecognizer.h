
#pragma once

typedef struct _SAvatarID
{
	MP_STRING pathPart;
	MP_STRING motionFrom;
	int id;

	_SAvatarID();
	_SAvatarID(const _SAvatarID& other);
} SAvatarID;

class CMotionRecognizer
{
public:
	CMotionRecognizer();
	~CMotionRecognizer();

	static CMotionRecognizer* GetInstance();

	int GetIDByPath(const std::string& path)const;

	void CorrectMotionPathIfNeeded(std::string& path);

private:
	void Init();

	MP_VECTOR<SAvatarID *> m_avatarsList;
};