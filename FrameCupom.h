#pragma once

#include <wx/wx.h>
#include <wx/grid.h>
#include "ClassSQLite.h"

class FrameCupom : public wxFrame
{
public:
    FrameCupom(wxWindow* parent, ClassSQLite* sqlite, int id_cupom);

private:
    ClassSQLite* sqlite;
    int id_cupom;
    wxPanel* m_panelTop;
    wxGrid* m_grid;

    wxTextCtrl* m_txtDataHora;
    wxTextCtrl* m_txtValorTotal;
    wxTextCtrl* m_txtNumero;
    wxTextCtrl* m_txtSerie;
    wxTextCtrl* m_txtChaveAcesso;

    wxTextCtrl* m_txtUrl;

    wxTextCtrl* m_txtEmitenteNome;
    wxTextCtrl* m_txtEmitenteEndereco;
    wxTextCtrl* m_txtEmitenteCnpj;

    wxTextCtrl* CriarCampo(wxWindow* parent,
        wxBoxSizer* sizer,
        const wxString& rotulo,
        int proportion = 0);

    wxStaticText* CriarRotulo(wxWindow* parent,
        wxBoxSizer* sizer,
        const wxString& texto);
private:
    void OnCopyURL(wxCommandEvent& event);
    void OnSalvarTXT(wxCommandEvent& event);
    wxString GridToPrettyTable(wxGrid* grid, 
        bool includeHeaders, 
        std::vector<int> right_cols,
        std::vector<int> sum_cols);
};