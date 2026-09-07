#pragma once

#include <wx/wx.h>

class FrameLogin : public wxDialog {
public:
    FrameLogin();
public:
    bool Ok(void) const;
private:
    bool ok;
    wxTextCtrl* txtUsuario;
    wxTextCtrl* txtSenha;
    void OnLogin(wxCommandEvent& event);
};
