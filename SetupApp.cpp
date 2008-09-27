/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		SETUPAPP.CPP
** COMPONENT:	The Application.
** DESCRIPTION:	The CSetupApp class definition.
**
*******************************************************************************
*/

#include "Common.hpp"
#include "SetupApp.hpp"

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

const tchar* FILE_VER_10  = TXT("1.0");
const tchar* FILE_VER_11  = TXT("1.1");
const tchar* FILE_VER_12  = TXT("1.2");
const tchar* CUR_FILE_VER = FILE_VER_12;

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
	m_strTitle = TXT("Setup");

	OSVERSIONINFO oInfo = { 0 };

	// Get Windows version info.
	oInfo.dwOSVersionInfoSize = sizeof(oInfo);

	::GetVersionEx(&oInfo);

	// Is Windows NT line?
	m_bWinNT = (oInfo.dwPlatformId == VER_PLATFORM_WIN32_NT);

	// Read the .ini file version.
	CString strVer = m_oIniFile.ReadString(TXT("Version"), TXT("Version"), TXT(""));

	// .ini file missing?
	if (strVer == TXT(""))
	{
		const tchar* pszMsg = TXT("The Setup script file is missing or corrupt:-\n\n")
							  TXT("Script: %s");

		FatalMsg(pszMsg, m_oIniFile.m_strPath);
		return false;
	}

	// Different .ini file version?
	if (strVer != CUR_FILE_VER)
	{
		const tchar* pszMsg = TXT("The Setup script is for a different version of this installer:-\n\n")
							  TXT("Found:      '%s'\n")
							  TXT("Expected: '%s'\n\n")
							  TXT("Script: %s");

		FatalMsg(pszMsg, strVer, CUR_FILE_VER, m_oIniFile.m_strPath);
		return false;
	}

	// Get title, product and author names.
	m_strTitle   = m_oIniFile.ReadString(TXT("Main"), TXT("Title"),   TXT(""));
	m_strProduct = m_oIniFile.ReadString(TXT("Main"), TXT("Product"), TXT(""));
	m_strAuthor  = m_oIniFile.ReadString(TXT("Main"), TXT("Author"),  TXT(""));

	if (m_strTitle == TXT(""))
	{
		const tchar* pszMsg = TXT("The Setup script file is missing the window title:-\n\n")
							  TXT("Entry: [Main] Title=\n")
							  TXT("Script: %s\n");

		FatalMsg(pszMsg, m_oIniFile.m_strPath);
		return false;
	}

	if (m_strProduct == TXT(""))
	{
		const tchar* pszMsg = TXT("The Setup script file is missing the product name:-\n\n")
							  TXT("Entry: [Main] Title=\n")
							  TXT("Script: %s\n");

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
