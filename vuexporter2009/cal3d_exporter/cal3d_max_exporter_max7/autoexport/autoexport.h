#pragma once

class TownAutoExportUtility : public UtilityObj
{
	// member variables
protected:
	HINSTANCE m_hInstance;
	
public:
	IUtil		*iu;
	Interface	*ip;
	HWND		hPanel;
    
	TownAutoExportUtility();
	~TownAutoExportUtility();

	void	BeginEditParams(Interface *ip,IUtil *iu);
	void	EndEditParams(Interface *ip,IUtil *iu);
	void	DeleteThis() {}
	void	SetInstance(HINSTANCE hInstance) { m_hInstance = hInstance;}

	bool	ExportDirectoriesExists();

	void	ExportToLib();

	void	ApplySelected();
	void	DoSettings();
	void	WeldSelected( float dist);
};