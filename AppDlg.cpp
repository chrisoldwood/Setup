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
#include "ProgressDlg.hpp"
#include "ConflictDlg.hpp"

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
		CTRL(IDC_ALL_USERS, &m_ckAllUsers)
		CTRL(IDC_CREATE,	&m_rbNewGroup)
		CTRL(IDC_NEW_GROUP,	&m_ebNewGroup)
		CTRL(IDC_EXISTING,	&m_rbOldGroup)
		CTRL(IDC_OLD_GROUP,	&m_cbOldGroup)
		CTRL(IDC_DESK_ICON,	&m_ckDeskIcon)
	END_CTRL_TABLE

	DEFINE_CTRLMSG_TABLE
		CMD_CTRLMSG(IDC_BROWSE,		BN_CLICKED,	OnBrowse  )
		CMD_CTRLMSG(IDC_PROG_ICON,	BN_CLICKED,	OnProgIcon)
		CMD_CTRLMSG(IDC_ALL_USERS,	BN_CLICKED,	OnAllUsers)
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
	bool    bAllUsers  = App.m_oIniFile.ReadBool  ("Main", "AllUsers",  false);
	CString strProgGrp = App.m_oIniFile.ReadString("Main", "ProgGroup", ""   );
	bool    bNewGroup  = App.m_oIniFile.ReadBool  ("Main", "NewGroup",  true );
	bool    bDeskIcon  = App.m_oIniFile.ReadBool  ("Main", "DeskIcon",  false);

	// Is always "All Users" on 9x.
	if (!App.m_bWinNT)
		bAllUsers = true;

	// Try and get the "Program Files" folder.
	// NB: Only available with IE 5.x and later.
	CPath strProgsDir = CPath::SpecialDir(CSIDL_PROGRAM_FILES);

	if (strProgsDir == "")
		strProgsDir = CPath::WindowsDir().Drive() + "Program Files";

	// Initialise the controls.
	m_ebAppFolder.Text(strProgsDir + strAppDir);
	m_ckProgIcon.Check(bProgIcon);
	m_ckAllUsers.Check(bAllUsers);
	m_rbNewGroup.Check(bNewGroup);
	m_ebNewGroup.Text(strProgGrp);
	m_rbOldGroup.Check(!bNewGroup);
	m_ckDeskIcon.Check(bDeskIcon);

	// Initialise dependent controls.
	OnProgIcon();
	OnAllUsers();
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
** Method:		OnProgIcon()
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
		m_ckAllUsers.Enable(App.m_bWinNT);
		m_rbNewGroup.Enable(true);
		m_rbOldGroup.Enable(true);

		if (m_rbNewGroup.IsChecked())
			OnNewGroup();
		else
			OnOldGroup();
	}
	else
	{
		m_ckAllUsers.Enable(false);
		m_rbNewGroup.Enable(false);
		m_ebNewGroup.Enable(false);
		m_rbOldGroup.Enable(false);
		m_cbOldGroup.Enable(false);
	}
}

/******************************************************************************
** Method:		OnAllUsers()
**
** Description:	All Users clicked - populate existing folders list.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppDlg::OnAllUsers()
{
	m_cbOldGroup.Reset();

	// Get the start menu folder.
	bool  bAllUsers   = m_ckAllUsers.IsChecked();
	uint  nFolderID   = (bAllUsers && App.m_bWinNT) ? CSIDL_COMMON_PROGRAMS : CSIDL_PROGRAMS;
	CPath strStartDir = CPath::SpecialDir(nFolderID);

	CFileFinder oFinder;
	CFileTree   oFileTree;

	// Find all Start menu folders...
	oFinder.Find(strStartDir, "*.*", false, oFileTree);

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

	// Warn user about required privilages.
	if (bAllUsers && App.m_bWinNT)
		NotifyMsg("Please ensure you have Administrator rights before installing for all users.");
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
	// Show the progress dialog.
	CProgressDlg Dlg;

	Dlg.RunModeless(App.m_AppWnd);
	Dlg.Title("Installing");
	Dlg.UpdateLabel("Copying files...");
	App.m_AppWnd.Enable(false);

	try
	{
		// Template shorthands.
		typedef TMap<CString, CString> CStrStrMap;

		CString strErr;

		// Get the source and destination folders.
		CPath strSetupDir   = CPath::ApplicationDir();
		CPath strInstallDir = m_ebAppFolder.Text();
		
		CStrArray  astrFileList;
		CStrArray  astrFiles;
		CStrStrMap oNameMap;
		CStrStrMap oDescMap;

		// Load the file list.
		if (LoadList("File", astrFileList) > 0)
		{
			// Parse file list entries.
			for (int i = 0; i < astrFileList.Size(); ++i)
			{
				CStrArray astrFields;
				CString   strFile, strName, strDesc;

				// Split into File + Name + Description.
				int nFields = CStrTok::Split(astrFileList[i], ',', astrFields);

				if (nFields >= 1)	strFile = astrFields[0];
				if (nFields >= 2)	strName = astrFields[1];
				if (nFields >= 3)	strDesc = astrFields[2];

				// Add to collections.
				astrFiles.Add(strFile);
				oNameMap.Add(strFile, strName);
				oDescMap.Add(strFile, strDesc);
			}

			// Verify the file list.
			for (int i = 0; i < astrFiles.Size(); ++i)
			{
				CString strFileName = astrFiles[i];
				CPath   strFilePath = CPath(strSetupDir, strFileName);

				if ( (strFileName == "") || (!strFilePath.Exists()) )
				{
					strErr = "Failed to verify the File List.\n\nThe File List contains invalid entries.";

					throw strErr;
				}
			}
		}

		// Create the installation folder, if required.
		if (!strInstallDir.Exists())
		{
			if (!CFile::CreateFolder(strInstallDir, true))
			{
				strErr.Format("Failed to create folder: %s.\n\n%s", strInstallDir, App.FormatError());

				throw strErr;
			}
		}

		// Initialise progress bar.
		Dlg.UpdateLabel("Copying files...");
		Dlg.InitMeter(astrFiles.Size());

		// Copy the files to the installation folder...
		for (int i = 0; i < astrFiles.Size(); ++i)
		{
			CString strFile    = astrFiles[i];
			CPath   strSrcFile = CPath(strSetupDir,   strFile);
			CPath   strDstFile = CPath(strInstallDir, strFile);

			Dlg.UpdateLabelAndMeter("Copying file: " + (CString)strFile, i);

			CopyFile(strSrcFile, strDstFile);
		}

		// Create a Start menu shortcut?
		if (m_ckProgIcon.IsChecked())
		{
			uint nFolderID = (m_ckAllUsers.IsChecked() && App.m_bWinNT) ? CSIDL_COMMON_PROGRAMS : CSIDL_PROGRAMS;

			// Get the Start menu base and target folders.
			CPath   strStartDir = CPath::SpecialDir(nFolderID);
			CString strFolder   = m_ebNewGroup.Text();

			// Using existing folder?
			if (m_rbOldGroup.IsChecked())
				strFolder = m_cbOldGroup.Text();

			// Create full path to Start menu folder.
			CPath strIconsDir = CPath(strStartDir, strFolder);

			if (!strIconsDir.Exists())
			{
				if (!CFile::CreateFolder(strIconsDir, true))
				{
					strErr.Format("Failed to create folder: %s.\n\n%s", strIconsDir, App.FormatError());

					throw strErr;
				}
			}

			CStrArray astrShortcuts;

			// Load the Shortcut list.
			if (LoadList("Shortcut", astrShortcuts) > 0)
			{
				// Create all Shortcuts.
				for (int i = 0; i < astrShortcuts.Size(); ++i)
				{
					CPath   strFile = astrShortcuts[i];
					CString strName = strFile.FileTitle();
					CString strDesc;

					// Get the shortcut name and description.
					oNameMap.Find(strFile, strName);
					oDescMap.Find(strFile, strDesc);

					// Append shortcut extension.
					strName += ".lnk";

					CPath strLink   = CPath(strIconsDir,   strName);
					CPath strTarget = CPath(strInstallDir, strFile);

					// Create it...
					if (!CFile::CreateShortcut(strLink, strTarget, strDesc))
					{
						strErr.Format("Failed to create Programs shortcut: %s\n\n%s", strLink, App.FormatError());

						throw strErr;
					}
				}
			}
		}

		// Create a Desktop shortcut?
		if (m_ckDeskIcon.IsChecked())
		{
			// Get the desktop folder.
			CPath strDesktop = CPath::SpecialDir(CSIDL_DESKTOPDIRECTORY);

			if (!strDesktop.Exists())
			{
				strErr = "Failed to locate the Shell Desktop folder.";

				throw strErr;
			}

			CStrArray astrDeskIcons;

			// Load the DeskIcons list.
			if (LoadList("DeskIcon", astrDeskIcons) > 0)
			{
				// Create all Desktop icons.
				for (int i = 0; i < astrDeskIcons.Size(); ++i)
				{
					CPath   strFile = astrDeskIcons[i];
					CString strName = strFile.FileTitle();
					CString strDesc;

					// Get the shortcut name and description.
					oNameMap.Find(strFile, strName);
					oDescMap.Find(strFile, strDesc);

					// Append shortcut extension.
					strName += ".lnk";

					CPath strLink   = CPath(strDesktop,    strName);
					CPath strTarget = CPath(strInstallDir, strFile);

					// Create it...
					if (!CFile::CreateShortcut(strLink, strTarget, strDesc))
					{
						strErr.Format("Failed to create Desktop shortcut: %s\n\n%s", strLink, App.FormatError());

						throw strErr;
					}
				}
			}
		}

		// Remove progress dialog.
		App.m_AppWnd.Enable(true);
		Dlg.Destroy();

		NotifyMsg("%s installed successfully.", App.m_strProduct);
	}
	catch (const CString& e)
	{
		// Remove progress dialog.
		App.m_AppWnd.Enable(true);
		Dlg.Destroy();

		FatalMsg(e);
	}

	// Close the app.
	App.m_AppWnd.Close();
}

/******************************************************************************
** Method:		LoadList()
**
** Description:	Loads a list of items from the setup config file.
**				The list is of the form:-
**				[Items]
**				Count=?
**				Item[0]=?
**
** Parameters:	pszItem		The name of the item.
**				astrList	The return array for the list.
**
** Returns:		The list size.
**
*******************************************************************************
*/

int CAppDlg::LoadList(const char* pszItem, CStrArray& astrList)
{
	CString strSection, strEntry;

	// Format the section name [Items].
	strSection.Format("%ss", pszItem);

	// Read the list size.
	int nItems = App.m_oIniFile.ReadInt(strSection, "Count", 0);

	// Read the list.
	for (int i = 0; i < nItems; ++i)
	{
		strEntry.Format("%s[%d]", pszItem, i);

		astrList.Add(App.m_oIniFile.ReadString(strSection, strEntry, ""));
	}

	return astrList.Size();
}

/******************************************************************************
** Method:		CopyFile()
**
** Description:	Copy a single file - querying the user if necessary to handle
**				any errors.
**
** Parameters:	strSrcFile		The source file.
**				strDstFile		The destination.
**
** Returns:		Nothing.
**
** Exceptions:	CString on error.
**
*******************************************************************************
*/

void CAppDlg::CopyFile(const CPath& strSrcFile, const CPath& strDstFile)
{
	CString strErr;

	// Destination file exists?
	if (strDstFile.Exists())
	{
		// Overwrite flags.
		static bool bDefYesAll = false;
		static bool bDefNoAll  = false;

		// Default is never overwrite?
		if (bDefNoAll)
			return;

		struct _stat oSrcInfo, oDstInfo;

		// Get source file properties.
		if (!CFile::QueryInfo(strSrcFile, oSrcInfo))
		{
			strErr.Format("Failed to query file properties:\n\n%s", strSrcFile);

			throw strErr;
		}

		// Get destination file properties.
		if (!CFile::QueryInfo(strDstFile, oDstInfo))
		{
			strErr.Format("Failed to query file properties:\n\n%s", strDstFile);

			throw strErr;
		}

		// Ignore if file Size AND TimeStamp are the same.
		if ( (oSrcInfo.st_size  == oDstInfo.st_size )
		  && (oSrcInfo.st_mtime == oDstInfo.st_mtime) )
			return;

		// Default is to query overwrites?
		if (!bDefYesAll)
		{
			CConflictDlg oQueryDlg;

			oQueryDlg.m_strFileName1.Format("%s", strDstFile);
			oQueryDlg.m_strFileInfo1.Format("%s - %u Bytes", CStrCvt::FormatDateTime(oDstInfo.st_mtime), oDstInfo.st_size);
			oQueryDlg.m_strFileName2.Format("%s", strSrcFile);
			oQueryDlg.m_strFileInfo2.Format("%s - %u Bytes", CStrCvt::FormatDateTime(oSrcInfo.st_mtime), oSrcInfo.st_size);

			// Query user for action.
			int nResult = oQueryDlg.RunModal(App.m_AppWnd);

			// Default answer returned?
			if (nResult == IDYESALL)	bDefYesAll = true;
			if (nResult == IDNOALL)		bDefNoAll  = true;

			// Ignore file?
			if ((nResult == IDNO) || (nResult == IDNOALL))
				return;

			// Abort install?
			if (nResult == IDCANCEL)
			{
				strErr = "Installation aborted";

				throw strErr;
			}
		}
	}

	// Copy file.
	if (!CFile::Copy(strSrcFile, strDstFile, true))
	{
		strErr.Format("Failed to copy file:\n\n%s to\n%s\n\n%s", strSrcFile, strDstFile, App.FormatError());

		throw strErr;
	}
}
