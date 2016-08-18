
#ifdef PATHREFINE_EXPORTS
#define PATHREFINE_API __declspec(dllexport)
#else
#define PATHREFINE_API __declspec(dllimport)
#endif

struct __Action
{
	CWComString name;
	CWComString paramList;

};

struct __RefinerInfo
{
	CWComString sPathFileForActions;
	CWComString sPathFull;
	__Action *actions[5];
};

PATHREFINE_API __RefinerInfo *RefineResourcePath(LPWSTR sPath);
PATHREFINE_API LPWSTR GetRefineResourcePath(LPWSTR aPath);
PATHREFINE_API void DeleteRefineResourcePathInfo(__RefinerInfo *pData);