#include "FrameLogin.h"
#include "ClassLogin.h"

FrameLogin::FrameLogin() : wxDialog(nullptr, wxID_ANY, "Login - Dashboard", wxDefaultPosition, wxDefaultSize) {
    Center();
    
    ok = false;    

    // Painel principal para fundo correto em todos os S.O.
    wxPanel* panel = new wxPanel(this, wxID_ANY);

    // Sizers para organização visual (Layout Responsivo)
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer* gridSizer = new wxFlexGridSizer(2, 2, 10, 10);

    // Componentes de texto e entrada
    wxStaticText* lblUsuario = new wxStaticText(panel, wxID_ANY, L"Usuário:");
    txtUsuario = new wxTextCtrl(panel, wxID_ANY, "");

    wxStaticText* lblSenha = new wxStaticText(panel, wxID_ANY, L"Senha:");
    txtSenha = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);

    // Adiciona componentes ao grid (2 colunas)
    gridSizer->Add(lblUsuario, 0, wxALIGN_CENTER_VERTICAL);
    gridSizer->Add(txtUsuario, 1, wxEXPAND);
    gridSizer->Add(lblSenha, 0, wxALIGN_CENTER_VERTICAL);
    gridSizer->Add(txtSenha, 1, wxEXPAND);

    // Permite que a coluna do campo de texto cresça horizontalmente
    gridSizer->AddGrowableCol(1, 1);

    // Botão de envio
    wxButton* btnLogin = new wxButton(panel, wxID_ANY, L"Entrar");

    // Organização das margens no Sizer Principal
    mainSizer->Add(gridSizer, 1, wxALL | wxEXPAND, 20);
    mainSizer->Add(btnLogin, 0, wxALIGN_CENTER | wxBOTTOM, 20);

    panel->SetSizer(mainSizer);

    // Evento do botão
    btnLogin->Bind(wxEVT_BUTTON, &FrameLogin::OnLogin, this);

    this->Fit();
}

void FrameLogin::OnLogin(wxCommandEvent& event) {
    wxString usuario = txtUsuario->GetValue();
    wxString senha = txtSenha->GetValue();

    ClassLogin login(false);

    if (login.Login(usuario, senha)) {
        ok = true;
        Close();       
    }
    else {
        wxMessageBox(L"Usuário ou senha inválidos.", L"Erro", wxOK | wxICON_ERROR);
    }
}

bool FrameLogin::Ok(void) const {
    return ok;
}

