#pragma once
#include <wx/wx.h>

class ClassLogin
{
private:
	wxString id;
	wxString usuario;
	wxString token;
public:
	ClassLogin(bool load_config);
	~ClassLogin();
public:
	bool IsValidToken(void) const;
	bool Login(wxString usuario, wxString senha);
	void Logoff(wxFrame *main_frame);
	wxString Usuario(void) const;
	wxString Token(void) const;
	void LoadConfig(void);
	void SaveConfig(void) const;
};

