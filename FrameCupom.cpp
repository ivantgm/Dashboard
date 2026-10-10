#include "FrameCupom.h"
#include <wx/artprov.h>
#include <wx/clipbrd.h>

FrameCupom::FrameCupom(wxWindow* parent, ClassSQLite* sqlite, int id_cupom)
    : wxFrame(parent, wxID_ANY, "Cupom")
{
    this->id_cupom = id_cupom;
    this->sqlite = sqlite;
    
    // ============================================================
    // Sizer principal
    // ============================================================

    auto* mainSizer = new wxBoxSizer(wxVERTICAL);


    // ============================================================
    // Panel superior
    // ============================================================

    m_panelTop = new wxPanel(this);

    auto* topSizer = new wxBoxSizer(wxVERTICAL);


    // ============================================================
    // LINHA 1
    //
    // Data/Hora | Valor Total | Número | Série | Chave de Acesso
    // ============================================================

    auto* linha1 = new wxBoxSizer(wxHORIZONTAL);

    m_txtDataHora = CriarCampo(
        m_panelTop,
        linha1,
        L"Data / Hora",
        3
    );

    m_txtValorTotal = CriarCampo(
        m_panelTop,
        linha1,
        L"Valor Total",
        2
    );

    m_txtNumero = CriarCampo(
        m_panelTop,
        linha1,
        L"Número",
        1
    );

    m_txtSerie = CriarCampo(
        m_panelTop,
        linha1,
        L"Série",
        1
    );

    m_txtChaveAcesso = CriarCampo(
        m_panelTop,
        linha1,
        L"Chave de Acesso",
        5
    );

    topSizer->Add(
        linha1,
        0,
        wxEXPAND | wxALL,
        5
    );


    // ============================================================
    // LINHA 2
    //
    // URL
    // ============================================================

    auto* linha2 = new wxBoxSizer(wxHORIZONTAL);

    m_txtUrl = CriarCampo(
        m_panelTop,
        linha2,
        L"URL",
        1
    );

    auto copyIcon = wxArtProvider::GetBitmap(wxART_COPY, wxART_BUTTON, wxSize(16, 16));
    auto copyButton = new wxButton(m_panelTop, wxID_ANY, "Copiar URL");
    copyButton->SetBitmap(copyIcon);
    copyButton->SetBitmapPosition(wxLEFT);
    copyButton->Bind(wxEVT_BUTTON, &FrameCupom::OnCopyURL, this);
    linha2->Add(copyButton, 0, wxALIGN_BOTTOM, 0);

    auto saveIcon = wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_BUTTON, wxSize(16, 16));
    auto saveButton = new wxButton(m_panelTop, wxID_ANY, "Salvar TXT");
    saveButton->SetBitmap(saveIcon);
    saveButton->SetBitmapPosition(wxLEFT);
    saveButton->Bind(wxEVT_BUTTON, &FrameCupom::OnSalvarTXT, this);
    linha2->Add(saveButton, 0, wxALIGN_BOTTOM, 0);

    topSizer->Add(
        linha2,
        0,
        wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM,
        5
    );


    // ============================================================
    // LINHA 3
    //
    // Emitente Nome | Endereço | CNPJ
    // ============================================================

    auto* linha3 = new wxBoxSizer(wxHORIZONTAL);

    m_txtEmitenteNome = CriarCampo(
        m_panelTop,
        linha3,
        L"Emitente",
        2
    );

    m_txtEmitenteEndereco = CriarCampo(
        m_panelTop,
        linha3,
        L"Endereço",
        3
    );

    m_txtEmitenteCnpj = CriarCampo(
        m_panelTop,
        linha3,
        L"CNPJ",
        1
    );

    topSizer->Add(
        linha3,
        0,
        wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM,
        5
    );


    m_panelTop->SetSizer(topSizer);


    // ============================================================
    // GRID
    // ============================================================

    m_grid = new wxGrid(this, wxID_ANY);
    m_grid->SetMinSize(wxSize(250, 200));
    m_grid->EnableEditing(false);
    m_grid->SetTabBehaviour(wxGrid::Tab_Leave);
    m_grid->CreateGrid(1, 1);
    m_grid->SetRowLabelSize(24);
    m_grid->SetColLabelSize(24);
    const char* sql_select_itens =
        "select\n"
        "i.seq,\n"
        "i.codigo,\n"
        "i.descricao,\n"
        "i.qtde,\n"
        "i.un,\n"
        "i.valor_unit,\n"
        "i.valor_total,\n"
        "i.desconto,\n"
        "round(i.valor_total - i.desconto, 2) as valor_liquido\n"
        "from cupom_item i\n"
        "where i.id_cupom = %d\n"
        "order by i.seq\n"
        ;
    std::map<wxString, wxString> col_labels;
    col_labels["seq"] = L"Seq";
    col_labels["codigo"] = L"Código";
    col_labels["descricao"] = L"Descrição";
    col_labels["qtde"] = L"Qtde";
    col_labels["un"] = L"Unid";
    col_labels["valor_unit"] = L"V.Unit";
    col_labels["valor_total"] = L"V.Total";
    col_labels["desconto"] = L"Desconto";
    col_labels["valor_liquido"] = L"V.Líquido";
    sqlite->Execute(wxString::Format(sql_select_itens, id_cupom), m_grid, col_labels);
    m_grid->SetFocus();

    const char* sql_select_cupom = 
        "select\n"                
        "substr(c.data_hora_emissao, 7, 2) || '/' || \n"
        "substr(c.data_hora_emissao, 5, 2) || '/' || \n"
        "substr(c.data_hora_emissao, 1, 4) as data, \n"
        "substr(c.data_hora_emissao, 9, 2) || ':' || \n"
        "substr(c.data_hora_emissao, 11, 2) || ':' || \n"
        "substr(c.data_hora_emissao, 13, 2) as hora, \n"
        "c.valor_total, \n"
        "c.numero_cfe,\n"
        "c.numero_serie_sat,\n"
        "c.cnpj_emitente, \n"
        "e.fantasia, \n"
        "e.endereco, \n"
        "c.chave_acesso, \n"
        "c.url_consulta\n"
        "from cupom c\n"
        "join emitente e on c.cnpj_emitente = e.cnpj\n"
        "where c.id = %d\n"
        ;
    std::map<wxString, wxString> row;
    sqlite->Execute(wxString::Format(sql_select_cupom, id_cupom), row);

    m_txtDataHora->SetValue(row["data"] + " " + row["hora"]);
    m_txtValorTotal->SetValue(row["valor_total"]);
    m_txtNumero->SetValue(row["numero_cfe"]);
    m_txtSerie->SetValue(row["numero_serie_sat"]);
    m_txtChaveAcesso->SetValue(row["chave_acesso"]);
    m_txtUrl->SetValue(row["url_consulta"]);
    m_txtEmitenteNome->SetValue(row["fantasia"]);
    m_txtEmitenteEndereco->SetValue(row["endereco"]);
    m_txtEmitenteCnpj->SetValue(row["cnpj_emitente"]);




    // ============================================================
    // Layout principal
    // ============================================================

    mainSizer->Add(
        m_panelTop,
        0,
        wxEXPAND
    );

    mainSizer->Add(
        m_grid,
        1,
        wxEXPAND | wxALL,
        5
    );

    SetSizer(mainSizer);
    this->SetMinSize(wxSize(800, 350));
}


// ================================================================
// Cria um campo com o rótulo acima
// ================================================================

wxTextCtrl* FrameCupom::CriarCampo(
    wxWindow* parent,
    wxBoxSizer* sizer,
    const wxString& rotulo,
    int proportion)
{
    auto* campoSizer = new wxBoxSizer(wxVERTICAL);

    auto* label = new wxStaticText(
        parent,
        wxID_ANY,
        rotulo
    );

    auto* campo = new wxTextCtrl(
        parent,
        wxID_ANY
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

void FrameCupom::OnCopyURL(wxCommandEvent& event)
{
    if (wxTheClipboard->Open()) {
        wxTheClipboard->SetData(new wxTextDataObject(m_txtUrl->GetValue()));
        wxTheClipboard->Close();
    }
}

void FrameCupom::OnSalvarTXT(wxCommandEvent& event)
{

}
