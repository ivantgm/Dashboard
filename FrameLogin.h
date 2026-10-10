#pragma once

#include <wx/wx.h>

class FrameLogin : public wxDialog {
public:
    FrameLogin();
public:
    bool Ok(void) const;
private:
    bool m_ok;
    wxTextCtrl* m_txtUsuario;
    wxTextCtrl* m_txtSenha;
    void OnLogin(wxCommandEvent& event);
    wxTextCtrl* CriarCampo(wxWindow* parent,
        wxBoxSizer* sizer,
        const wxString& rotulo,
        int proportion = 0,
        bool password = false);
};
