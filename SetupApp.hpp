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

	//
	// Constants.
	//
	static const char* INI_FILE_VER;
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
