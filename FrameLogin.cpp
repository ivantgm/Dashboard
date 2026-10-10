#include "FrameLogin.h"
#include "ClassLogin.h"

FrameLogin::FrameLogin() : wxDialog(nullptr, wxID_ANY, "Login - Dashboard", wxDefaultPosition, wxDefaultSize) {
    Center();
    
    m_ok = false;    

    auto* mainSizer = new wxBoxSizer(wxVERTICAL);

    auto* m_panelTop = new wxPanel(this);
    auto* topSizer = new wxBoxSizer(wxVERTICAL);

    auto* linha1 = new wxBoxSizer(wxHORIZONTAL);
    m_txtUsuario = CriarCampo(
        m_panelTop,
        linha1,
        L"Usuário",
        1
    );
    topSizer->Add(
        linha1,
        0,
        wxEXPAND | wxALL,
        1
    );

    auto* linha2 = new wxBoxSizer(wxHORIZONTAL);
    m_txtSenha = CriarCampo(
        m_panelTop,
        linha2,
        L"Senha",
        1,
        true
    );
    topSizer->Add(
        linha2,
        0,
        wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM,
        1
    );




    // Botão de envio
    wxButton* btnLogin = new wxButton(m_panelTop, wxID_ANY, L"Entrar");

    // Organização das margens no Sizer Principal
    mainSizer->Add(topSizer, 1, wxALL | wxEXPAND, 20);
    mainSizer->Add(btnLogin, 0, wxALIGN_CENTER | wxBOTTOM, 20);

    m_panelTop->SetSizer(mainSizer);

    // Evento do botão
    btnLogin->Bind(wxEVT_BUTTON, &FrameLogin::OnLogin, this);

    this->SetMinSize(wxSize(400, 350));
}

void FrameLogin::OnLogin(wxCommandEvent& event) {
    wxString usuario = m_txtUsuario->GetValue();
    wxString senha = m_txtSenha->GetValue();

    ClassLogin login(false);

    if (login.Login(usuario, senha)) {
        m_ok = true;
        Close();       
    }
    else {
        wxMessageBox(L"Usuário ou senha inválidos.", L"Erro", wxOK | wxICON_ERROR);
    }
}

bool FrameLogin::Ok(void) const {
    return m_ok;
}

wxTextCtrl* FrameLogin::CriarCampo(
    wxWindow* parent,
    wxBoxSizer* sizer,
    const wxString& rotulo,
    int proportion,
    bool password)
{
    auto* campoSizer = new wxBoxSizer(wxVERTICAL);

    auto* label = new wxStaticText(
        parent,
        wxID_ANY,
        rotulo
    );

    auto* campo = new wxTextCtrl(
        parent,
        wxID_ANY,
        "", 
        wxDefaultPosition, 
        wxDefaultSize, 
        password ? wxTE_PASSWORD : 0L
    );

    campoSizer->Add(
        label,
        0,
        wxBOTTOM,
        2
    );

    campoSizer->Add(
        campo,
        0,
        wxEXPAND
    );

    sizer->Add(
        campoSizer,
        proportion,
        wxEXPAND | wxLEFT | wxRIGHT,
        4
    );

    return campo;
}
