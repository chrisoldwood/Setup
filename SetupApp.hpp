/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		SETUPAPP.HPP
** COMPONENT:	The Application.
** DESCRIPTION:	The CSetupApp class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef SETUPAPP_HPP
#define SETUPAPP_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <WCL/App.hpp>
#include "AppWnd.hpp"
#include "AppCmds.hpp"
#include <WCL/IniFile.hpp>

/******************************************************************************
** 
** The application class.
**
*******************************************************************************
*/

class CSetupApp : public CApp
{
public:
	//
	// Constructors/Destructor.
	//
	CSetupApp();
	~CSetupApp();

	//
	// Members
	//
	CAppWnd		m_AppWnd;		// Main window.
	CAppCmds	m_AppCmds;		// Command handler.
	CIniFile	m_oIniFile;		// Config File.
	bool		m_bWinNT;		// Is Windows NT line?
	CString		m_strProduct;	// Product name.
	CString		m_strAuthor;	// Product author.

	//
	// Constants.
	//
	static const char* VERSION;

protected:
	//
	// Startup and Shutdown template methods.
	//
	virtual	bool OnOpen();
	virtual	bool OnClose();
};

/******************************************************************************
**
** Global variables.
**
*******************************************************************************
*/

// The application object.
extern CSetupApp App;

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/


#endif //SETUPAPP_HPP
