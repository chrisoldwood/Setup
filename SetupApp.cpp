/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		SETUPAPP.CPP
** COMPONENT:	The Application.
** DESCRIPTION:	The CSetupApp class definition.
**
*******************************************************************************
*/

#include "AppHeaders.hpp"

/******************************************************************************
**
** Global variables.
**
*******************************************************************************
*/

// "The" application object.
CSetupApp App;

/******************************************************************************
**
** Constants.
**
*******************************************************************************
*/

const char* FILE_VER_10  = "1.0";
const char* FILE_VER_11  = "1.1";
const char* FILE_VER_12  = "1.2";
const char* CUR_FILE_VER = FILE_VER_12;

/******************************************************************************
**
** Class constants.
**
*******************************************************************************
*/

#ifdef _DEBUG
const char* CSetupApp::VERSION      = "v1.2 [Debug]";
#else
const char* CSetupApp::VERSION      = "v1.2";
#endif

/******************************************************************************
** Method:		Constructor
**
** Description:	Default constructor.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

CSetupApp::CSetupApp()
	: CApp(m_AppWnd, m_AppCmds)
	, m_bWinNT(false)
{

}

/******************************************************************************
** Method:		Destructor
**
** Description:	Cleanup.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

CSetupApp::~CSetupApp()
{

}

/******************************************************************************
** Method:		OnOpen()
**
** Description:	Initialises the application.
**
** Parameters:	None.
**
** Returns:		true or false.
**
*******************************************************************************
*/

bool CSetupApp::OnOpen()
{
	// Set the initial app title.
	m_strTitle = "Setup";

	OSVERSIONINFO oInfo = { 0 };

	// Get Windows version info.
	oInfo.dwOSVersionInfoSize = sizeof(oInfo);

	::GetVersionEx(&oInfo);

	// Is Windows NT line?
	m_bWinNT = (oInfo.dwPlatformId == VER_PLATFORM_WIN32_NT);

	// Read the .ini file version.
	CString strVer = m_oIniFile.ReadString("Version", "Version", "");

	// .ini file missing?
	if (strVer == "")
	{
		const char* pszMsg = "The Setup script file is missing or corrupt:-\n\n"
							 "Script: %s";

		FatalMsg(pszMsg, m_oIniFile.m_strPath);
		return false;
	}

	// Different .ini file version?
	if (strVer != CUR_FILE_VER)
	{
		const char* pszMsg = "The Setup script is for a different version of this installer:-\n\n"
							 "Found:      '%s'\n"
							 "Expected: '%s'\n\n"
							 "Script: %s";

		FatalMsg(pszMsg, strVer, CUR_FILE_VER, m_oIniFile.m_strPath);
		return false;
	}

	// Get title, product and author names.
	m_strTitle   = m_oIniFile.ReadString("Main", "Title",   "");
	m_strProduct = m_oIniFile.ReadString("Main", "Product", "");
	m_strAuthor  = m_oIniFile.ReadString("Main", "Author",  "");

	if (m_strTitle == "")
	{
		const char* pszMsg = "The Setup script file is missing the window title:-\n\n"
							 "Entry: [Main] Title=\n"
							 "Script: %s\n";

		FatalMsg(pszMsg, m_oIniFile.m_strPath);
		return false;
	}

	if (m_strProduct == "")
	{
		const char* pszMsg = "The Setup script file is missing the product name:-\n\n"
							 "Entry: [Main] Title=\n"
							 "Script: %s\n";

		FatalMsg(pszMsg, m_oIniFile.m_strPath);
		return false;
	}

	// Create the main window.
	if (!m_AppWnd.Create())
		return false;

	// Show it.
	m_AppWnd.Centre();
	m_AppWnd.Show(m_iCmdShow);

	// Update UI.
	m_AppCmds.UpdateUI();

	return true;
}

/******************************************************************************
** Method:		OnClose()
**
** Description:	Cleans up the application resources.
**
** Parameters:	None.
**
** Returns:		true or false.
**
*******************************************************************************
*/

bool CSetupApp::OnClose()
{
	return true;
}
