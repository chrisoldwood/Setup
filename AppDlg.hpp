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
	void OnNewGroup();
	void OnOldGroup();
	void OnInstall();
	void OnClose();
	void OnAbout();
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

#endif //APPDLG_HPP
