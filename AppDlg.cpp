/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		APPDLG.CPP
** COMPONENT:	The Application.
** DESCRIPTION:	CAppDlg class definition.
**
*******************************************************************************
*/

#include "AppHeaders.hpp"
#include "AboutDlg.hpp"
#include <shlobj.h>

#ifndef CSIDL_PROGRAM_FILES
// NB: Only available with IE 5.x and later.
#define	CSIDL_PROGRAM_FILES		0x0026
#endif

/******************************************************************************
** Method:		Default constructor.
**
** Description:	.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

CAppDlg::CAppDlg()
	: CMainDlg(IDD_MAIN)
{
	DEFINE_CTRL_TABLE
		CTRL(IDC_FOLDER,	&m_ebAppFolder)
		CTRL(IDC_PROG_ICON,	&m_ckProgIcon)
		CTRL(IDC_CREATE,	&m_rbNewGroup)
		CTRL(IDC_NEW_GROUP,	&m_ebNewGroup)
		CTRL(IDC_EXISTING,	&m_rbOldGroup)
		CTRL(IDC_OLD_GROUP,	&m_cbOldGroup)
		CTRL(IDC_DESK_ICON,	&m_ckDeskIcon)
	END_CTRL_TABLE

	DEFINE_CTRLMSG_TABLE
		CMD_CTRLMSG(IDC_BROWSE,		BN_CLICKED,	OnBrowse  )
		CMD_CTRLMSG(IDC_PROG_ICON,	BN_CLICKED,	OnProgIcon)
		CMD_CTRLMSG(IDC_CREATE,		BN_CLICKED,	OnNewGroup)
		CMD_CTRLMSG(IDC_EXISTING,	BN_CLICKED,	OnOldGroup)
		CMD_CTRLMSG(IDC_INSTALL,	BN_CLICKED,	OnInstall )
		CMD_CTRLMSG(IDC_CANCEL,		BN_CLICKED,	OnClose   )
		CMD_CTRLMSG(IDC_ABOUT,		BN_CLICKED, OnAbout   )
	END_CTRLMSG_TABLE
}

/******************************************************************************
** Method:		OnInitDialog()
**
** Description:	Initialise the dialog.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppDlg::OnInitDialog()
{
	// Get the default installation settings.
	CPath   strAppDir  = App.m_oIniFile.ReadString("Main", "DefFolder", ""   );
	bool    bProgIcon  = App.m_oIniFile.ReadBool  ("Main", "ProgIcon",  false);
	CString strProgGrp = App.m_oIniFile.ReadString("Main", "ProgGroup", ""   );
	bool    bNewGroup  = App.m_oIniFile.ReadBool  ("Main", "NewGroup",  true );
	bool    bDeskIcon  = App.m_oIniFile.ReadBool  ("Main", "DeskIcon",  false);

	// Try and get the "Program Files" folder.
	// NB: Only available with IE 5.x and later.
	CPath strProgsDir = CPath::SpecialDir(CSIDL_PROGRAM_FILES);

	if (strProgsDir == "")
		strProgsDir = CPath::WindowsDir().Drive() + "Program Files";

	// Get the start menu folder.
	CPath strStartDir = CPath::SpecialDir(CSIDL_PROGRAMS);

	CFileFinder oFinder;
	CFileTree   oFileTree;

	// Find all Start menu folders...
	oFinder.Find(strStartDir, "*.*", false, oFileTree);

	// Initialise the controls.
	m_ebAppFolder.Text(strProgsDir + strAppDir);
	m_ckProgIcon.Check(bProgIcon);
	m_rbNewGroup.Check(bNewGroup);
	m_ebNewGroup.Text(strProgGrp);
	m_rbOldGroup.Check(!bNewGroup);
	m_ckDeskIcon.Check(bDeskIcon);

	// Initialise Start Menu folders combo.
	for (int i = 0; i < oFileTree.Root()->m_oData.m_astrDirs.Size(); ++i)
	{
		CPath strDir = oFileTree.Root()->m_oData.m_astrDirs[i];

		// Ignore "this" and "parent" folders.
		if ( (strDir == ".") || (strDir == "..") )
			continue;

		m_cbOldGroup.Add(strDir);
	}

	// Select 1st item by default.
	if (m_cbOldGroup.Count() > 0)
		m_cbOldGroup.CurSel(0);

	// Initialise dependent controls.
	OnProgIcon();
}

/******************************************************************************
** Method:		OnBrowse()
**
** Description:	Browse for the installation folder.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppDlg::OnBrowse()
{
	CPath strPath = m_ebAppFolder.Text();

	if (strPath.SelectDir(App.m_AppWnd, "Select Installation Folder", strPath))
		m_ebAppFolder.Text(strPath);
}

/******************************************************************************
** Methods:		OnProgIcon()
**
** Description:	Add Program Icon clicked - enable/disable relevant controls.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppDlg::OnProgIcon()
{
	if (m_ckProgIcon.IsChecked())
	{
		m_rbNewGroup.Enable(true);
		m_rbOldGroup.Enable(true);

		if (m_rbNewGroup.IsChecked())
			OnNewGroup();
		else
			OnOldGroup();
	}
	else
	{
		m_rbNewGroup.Enable(false);
		m_ebNewGroup.Enable(false);
		m_rbOldGroup.Enable(false);
		m_cbOldGroup.Enable(false);
	}
}

/******************************************************************************
** Methods:		OnNewGroup()
**				OnOldGroup()
**
** Description:	Program group radio button clicked - enable/disable relevant
**				controls.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppDlg::OnNewGroup()
{
	m_ebNewGroup.Enable(true);
	m_cbOldGroup.Enable(false);
}

void CAppDlg::OnOldGroup()
{
	m_ebNewGroup.Enable(false);
	m_cbOldGroup.Enable(true);
}

/******************************************************************************
** Method:		OnInstall()
**
** Description:	Do the installation.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppDlg::OnInstall()
{
}

/******************************************************************************
** Method:		OnClose()
**
** Description:	Close the application.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppDlg::OnClose()
{
	App.m_AppWnd.Close();
}

/******************************************************************************
** Method:		OnAbout()
**
** Description:	Show the about dialog.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppDlg::OnAbout()
{
	CAboutDlg Dlg;

	Dlg.RunModal(App.m_AppWnd);
}
