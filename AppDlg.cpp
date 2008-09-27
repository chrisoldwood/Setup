/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		APPDLG.CPP
** COMPONENT:	The Application.
** DESCRIPTION:	CAppDlg class definition.
**
*******************************************************************************
*/

#include "Common.hpp"
#include "AppDlg.hpp"
#include "SetupApp.hpp"
#include "AboutDlg.hpp"
#include <shlobj.h>
#include "ProgressDlg.hpp"
#include "ConflictDlg.hpp"
#include <Legacy/TMap.hpp>
#include <Legacy/FileFinder.hpp>
#include <WCL/StrTok.hpp>
#include <WCL/File.hpp>
#include <WCL/StrCvt.hpp>
#include <WCL/DateTime.hpp>

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
		CMD_CTRLMSG(IDC_BROWSE,		BN_CLICKED,	&CAppDlg::OnBrowse  )
		CMD_CTRLMSG(IDC_PROG_ICON,	BN_CLICKED,	&CAppDlg::OnProgIcon)
		CMD_CTRLMSG(IDC_ALL_USERS,	BN_CLICKED,	&CAppDlg::OnAllUsers)
		CMD_CTRLMSG(IDC_CREATE,		BN_CLICKED,	&CAppDlg::OnNewGroup)
		CMD_CTRLMSG(IDC_EXISTING,	BN_CLICKED,	&CAppDlg::OnOldGroup)
		CMD_CTRLMSG(IDC_INSTALL,	BN_CLICKED,	&CAppDlg::OnInstall )
		CMD_CTRLMSG(IDC_CANCEL,		BN_CLICKED,	&CAppDlg::OnClose   )
		CMD_CTRLMSG(IDC_ABOUT,		BN_CLICKED, &CAppDlg::OnAbout   )
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
	CPath   strRootDir = App.m_oIniFile.ReadString(TXT("Main"), TXT("DefRoot"),   TXT("%ProgramFiles%"));
	CPath   strAppDir  = App.m_oIniFile.ReadString(TXT("Main"), TXT("DefFolder"), TXT(""));
	bool    bProgIcon  = App.m_oIniFile.ReadBool  (TXT("Main"), TXT("ProgIcon"),  false);
	bool    bAllUsers  = App.m_oIniFile.ReadBool  (TXT("Main"), TXT("AllUsers"),  false);
	CString strProgGrp = App.m_oIniFile.ReadString(TXT("Main"), TXT("ProgGroup"), TXT(""));
	bool    bNewGroup  = App.m_oIniFile.ReadBool  (TXT("Main"), TXT("NewGroup"),  true );
	bool    bDeskIcon  = App.m_oIniFile.ReadBool  (TXT("Main"), TXT("DeskIcon"),  false);

	// Is always "All Users" on 9x.
	if (!App.m_bWinNT)
		bAllUsers = true;

	// Format the default installation path.
	CPath strDefDir = strRootDir / strAppDir;

	strDefDir.ExpandVars();

	// Initialise the controls.
	m_ebAppFolder.Text(strDefDir);
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

	if (strPath.SelectDir(App.m_AppWnd, TXT("Select Installation Folder"), strPath))
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
	oFinder.Find(strStartDir, TXT("*.*"), false, oFileTree);

	// Initialise Start Menu folders combo.
	for (size_t i = 0; i < oFileTree.Root()->m_oData.m_astrDirs.Size(); ++i)
	{
		CPath strDir = oFileTree.Root()->m_oData.m_astrDirs[i];

		// Ignore "this" and "parent" folders.
		if ( (strDir == TXT(".")) || (strDir == TXT("..")) )
			continue;

		m_cbOldGroup.Add(strDir);
	}

	// Select 1st item by default.
	if (m_cbOldGroup.Count() > 0)
		m_cbOldGroup.CurSel(0U);

	// Warn user about required privilages.
	if (bAllUsers && App.m_bWinNT)
		NotifyMsg(TXT("Please ensure you have Administrator rights before installing for all users."));
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
	AboutDlg Dlg;

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
	Dlg.Title(TXT("Installing"));
	Dlg.UpdateLabel(TXT("Copying files..."));
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
		CStrStrMap oFolderMap;
		CStrStrMap oNameMap;
		CStrStrMap oDescMap;

		// Load the file list.
		if (LoadList(TXT("File"), astrFileList) > 0)
		{
			// Parse file list entries.
			for (size_t i = 0; i < astrFileList.Size(); ++i)
			{
				CStrArray astrFields;
				CString   strFile, strFolder, strName, strDesc;

				// Split into File + Folder + Name + Description.
				int nFields = CStrTok::Split(astrFileList[i], TXT(','), astrFields);

				if (nFields >= 1)	strFile   = astrFields[0];
				if (nFields >= 2)	strFolder = astrFields[1];
				if (nFields >= 3)	strName   = astrFields[2];
				if (nFields >= 4)	strDesc   = astrFields[3];

				// Add to collections.
				astrFiles.Add(strFile);
				oFolderMap.Add(strFile, ParseFolder(strFolder, strInstallDir));
				oNameMap.Add(strFile, strName);
				oDescMap.Add(strFile, strDesc);
			}

			// Verify the file list.
			for (size_t i = 0; i < astrFiles.Size(); ++i)
			{
				CString strFileName = astrFiles[i];
				CPath   strFilePath = CPath(strSetupDir, strFileName);

				if (strFileName == TXT(""))
					throw CString::Fmt(TXT("Failed to verify the File List.\nFile entry [%d] is empty"), i);

				if (!strFilePath.Exists())
					throw CString::Fmt(TXT("Failed to verify the File List.\nThe following file is missing:-\n\n%s"), strFilePath);
			}
		}

		// Create the installation folder, if required.
		if (!strInstallDir.Exists())
		{
			if (!CFile::CreateFolder(strInstallDir, true))
			{
				strErr.Format(TXT("Failed to create folder:-\n\n%s\n\n%s"), strInstallDir, CStrCvt::FormatError());

				throw strErr;
			}
		}

		// Initialise progress bar.
		Dlg.UpdateLabel(TXT("Copying files..."));
		Dlg.InitMeter(astrFiles.Size());

		// Copy the files to the installation folder...
		for (size_t i = 0; i < astrFiles.Size(); ++i)
		{
			CString strFile    = astrFiles[i];
			CString strFolder  = oFolderMap.Find(strFile); 
			CPath   strSrcFile = CPath(strSetupDir, strFile);
			CPath   strDstFile = CPath(strFolder,   strFile);

			Dlg.UpdateLabelAndMeter(TXT("Copying file: ") + (CString)strFile, i);

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
					strErr.Format(TXT("Failed to create folder:-\n\n%s\n\n%s"), strIconsDir, CStrCvt::FormatError());

					throw strErr;
				}
			}

			CStrArray astrShortcuts;

			// Load the Shortcut list.
			if (LoadList(TXT("Shortcut"), astrShortcuts) > 0)
			{
				// Create all Shortcuts.
				for (size_t i = 0; i < astrShortcuts.Size(); ++i)
				{
					CPath   strFile = astrShortcuts[i];
					CString strName = strFile.FileTitle();
					CString strDesc;

					// Get the shortcut name and description, if set.
					oNameMap.Find(strFile, strName);
					oDescMap.Find(strFile, strDesc);

					// Append shortcut extension.
					strName += TXT(".lnk");

					CPath strLink   = CPath(strIconsDir,   strName);
					CPath strTarget = CPath(strInstallDir, strFile);

					// Create it...
					if (!CFile::CreateShortcut(strLink, strTarget, strDesc))
					{
						strErr.Format(TXT("Failed to create Programs shortcut:-\n\n%s\n\n%s"), strLink, CStrCvt::FormatError());

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
				strErr = TXT("Failed to locate the Shell Desktop folder.");

				throw strErr;
			}

			CStrArray astrDeskIcons;

			// Load the DeskIcons list.
			if (LoadList(TXT("DeskIcon"), astrDeskIcons) > 0)
			{
				// Create all Desktop icons.
				for (size_t i = 0; i < astrDeskIcons.Size(); ++i)
				{
					CPath   strFile = astrDeskIcons[i];
					CString strName = strFile.FileTitle();
					CString strDesc;

					// Get the shortcut name and description.
					oNameMap.Find(strFile, strName);
					oDescMap.Find(strFile, strDesc);

					// Append shortcut extension.
					strName += TXT(".lnk");

					CPath strLink   = CPath(strDesktop,    strName);
					CPath strTarget = CPath(strInstallDir, strFile);

					// Create it...
					if (!CFile::CreateShortcut(strLink, strTarget, strDesc))
					{
						strErr.Format(TXT("Failed to create Desktop shortcut:-\n\n%s\n\n%s"), strLink, CStrCvt::FormatError());

						throw strErr;
					}
				}
			}
		}

		// Remove progress dialog.
		App.m_AppWnd.Enable(true);
		Dlg.Destroy();

		NotifyMsg(TXT("%s installed successfully."), App.m_strProduct);
	}
	catch (const CString& e)
	{
		// Remove progress dialog.
		App.m_AppWnd.Enable(true);
		Dlg.Destroy();

		FatalMsg(TXT("%s"), e);
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

int CAppDlg::LoadList(const tchar* pszItem, CStrArray& astrList)
{
	CString strSection, strEntry;

	// Format the section name [Items].
	strSection.Format(TXT("%ss"), pszItem);

	// Read the list size.
	int nItems = App.m_oIniFile.ReadInt(strSection, TXT("Count"), 0);

	// Read the list.
	for (int i = 0; i < nItems; ++i)
	{
		strEntry.Format(TXT("%s[%d]"), pszItem, i);

		astrList.Add(App.m_oIniFile.ReadString(strSection, strEntry, TXT("")));
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
			strErr.Format(TXT("Failed to query file properties:\n\n%s"), strSrcFile);

			throw strErr;
		}

		// Get destination file properties.
		if (!CFile::QueryInfo(strDstFile, oDstInfo))
		{
			strErr.Format(TXT("Failed to query file properties:\n\n%s"), strDstFile);

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
			CDateTime    dtDstModTime = CDateTime::FromLocalTime(oDstInfo.st_mtime);
			CDateTime    dtSrcModTime = CDateTime::FromLocalTime(oSrcInfo.st_mtime);

			oQueryDlg.m_strFileName1.Format(TXT("%s"), strDstFile);
			oQueryDlg.m_strFileInfo1.Format(TXT("%s - %u Bytes"), dtDstModTime.ToString(), oDstInfo.st_size);
			oQueryDlg.m_strFileName2.Format(TXT("%s"), strSrcFile);
			oQueryDlg.m_strFileInfo2.Format(TXT("%s - %u Bytes"), dtSrcModTime.ToString(), oSrcInfo.st_size);

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
				strErr = TXT("Installation aborted");

				throw strErr;
			}
		}
	}

	// Copy file.
	if (!CFile::Copy(strSrcFile, strDstFile, true))
	{
		strErr.Format(TXT("Failed to copy file:\n\n%s to\n%s\n\n%s"), strSrcFile, strDstFile, CStrCvt::FormatError());

		throw strErr;
	}
}

/******************************************************************************
** Method:		ParseFolder()
**
** Description:	Parse the folder name and substitute any %variables%. If the
**				name is empty it defaults to %TargetDir%.
**
** Parameters:	strFolder		The folder to parse.
**				strTargetDir	The %TargetDir% folder.
**
** Returns:		The parsed string.
**
*******************************************************************************
*/

CPath CAppDlg::ParseFolder(const CPath& strFolder, const CPath& strTargetDir)
{
	// %TargetDir% is the default.
	if (strFolder == TXT(""))
		return strTargetDir;

	CPath str = strFolder;

	// Do a crude search and replace on all possible matches.
	if (str.Find(TXT('%')) != -1)
		str.Replace(TXT("%TargetDir%"), strTargetDir);

	if (str.Find(TXT('%')) != -1)
		str.ExpandVars();

	return str;
}
