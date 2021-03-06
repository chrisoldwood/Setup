/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		APPDLG.HPP
** COMPONENT:	The Application.
** DESCRIPTION:	The CAppDlg class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef APPDLG_HPP
#define APPDLG_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <WCL/MainDlg.hpp>
#include <WCL/CommonUI.hpp>

// Forward declarations.
class CPath;

/******************************************************************************
** 
** This is the main application dialog.
**
*******************************************************************************
*/

class CAppDlg : public CMainDlg
{
public:
	//
	// Constructors/Destructor.
	//
	CAppDlg();
	
protected:
	//
	// Controls.
	//
	CEditBox	m_ebAppFolder;
	CCheckBox	m_ckProgIcon;
	CCheckBox	m_ckAllUsers;
	CRadioBtn	m_rbNewGroup;
	CEditBox	m_ebNewGroup;
	CRadioBtn	m_rbOldGroup;
	CComboBox	m_cbOldGroup;
	CCheckBox	m_ckDeskIcon;

	//
	// Message processors.
	//
	virtual void OnInitDialog();

	void OnBrowse();
	void OnProgIcon();
	void OnAllUsers();
	void OnNewGroup();
	void OnOldGroup();
	void OnInstall();
	void OnClose();
	void OnAbout();

	//
	// Internal methods.
	//
	size_t LoadList(const tchar* pszItem, CStrArray& astrList);
	void CopyFile(const CPath& strSrcFile, const CPath& strDstFile);
	CPath ParseFolder(const CPath& strFolder, const CPath& strTargetDir);
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

#endif //APPDLG_HPP
