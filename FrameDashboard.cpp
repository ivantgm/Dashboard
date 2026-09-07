#include "FrameDashboard.h"
#include "ClassLogin.h"

#include "FrameTerminalSQL.h"
#include "FrameSincronizar.h"
#include "FrameCupom.h"
#include "PanelPieChart.h"
#include <algorithm>

enum {
  ID_Logoff = 1,
  ID_TerminalSQL,
  ID_Sincronizar
};

FrameDashboard::FrameDashboard()
    : wxFrame(NULL, wxID_ANY, "Cupom Fiscal Miliogo - Dashboard")
{
    ClassLogin login(true);
    this->token = login.Token();
    
    sqlite = new ClassSQLite();
    wxString sqlite_erro = sqlite->Erro();
    if (sqlite_erro != "") {
        wxMessageBox(sqlite_erro, "Erro", wxOK | wxICON_ERROR);
    }

    wxMenu* menuConta = new wxMenu;

    menuConta->Append(ID_Logoff, "&Logoff\tCtrl-Q",  "Sair da sua conta");
    menuConta->AppendSeparator();
    menuConta->Append(wxID_EXIT, "&Sair\tAlt-F4", "Fechar o programa");

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT, "Sobre Miliogo", "Sobre Miliogo");

    wxMenu *menuFerramentas = new wxMenu;
    menuFerramentas->Append(ID_TerminalSQL, "Terminal SQL", "Execute comandos diretamente no banco de dados");
    menuFerramentas->Append(ID_Sincronizar, "Sincronizar", "Faz download de novos cupons");

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuConta, "&Conta");
    menuBar->Append(menuFerramentas, "&Ferramentas");
    menuBar->Append(menuHelp, "&Sobre");

    SetMenuBar(menuBar);

    CreateStatusBar();
    SetStatusText(wxString::Format("Bem vindo %s!", login.Usuario()));

    Bind(wxEVT_MENU, &FrameDashboard::OnLogoff, this, ID_Logoff);
    Bind(wxEVT_MENU, &FrameDashboard::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &FrameDashboard::OnExit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &FrameDashboard::OnTerminalSQL, this, ID_TerminalSQL);
    Bind(wxEVT_MENU, &FrameDashboard::OnSincronizar, this, ID_Sincronizar);


    m_splitPrincipal = new wxSplitterWindow(
        this,
        wxID_ANY,
        wxDefaultPosition,
        wxDefaultSize,
        wxSP_LIVE_UPDATE | wxSP_3D);

    wxPanel* painelSuperior = new wxPanel(
        m_splitPrincipal,
        wxID_ANY);

    m_splitSuperior = new wxSplitterWindow(
        painelSuperior,
        wxID_ANY,
        wxDefaultPosition,
        wxDefaultSize,
        wxSP_LIVE_UPDATE | wxSP_3D);

    m_grid_agrupamento_mes = new wxGrid(
        m_splitSuperior,
        wxID_ANY);
    m_grid_agrupamento_mes->SetMinSize(wxSize(250, 200));
    m_grid_agrupamento_mes->EnableEditing(false); 
    m_grid_agrupamento_mes->SetTabBehaviour(wxGrid::Tab_Leave);    

    m_grid_cupons = new wxGrid(
        m_splitSuperior,
        wxID_ANY);
    m_grid_cupons->SetMinSize(wxSize(450, 200));
    m_grid_cupons->EnableEditing(false);
    m_grid_cupons->SetTabBehaviour(wxGrid::Tab_Leave);
        
    m_splitSuperior->SplitVertically(
        m_grid_agrupamento_mes,
        m_grid_cupons,
        250
    );
    wxBoxSizer* sizerSuperior =
        new wxBoxSizer(wxVERTICAL);

    sizerSuperior->Add(
        m_splitSuperior,
        1,
        wxEXPAND
    );

    painelSuperior->SetSizer(sizerSuperior);

    m_chart = new PanelBarChart(
        m_splitPrincipal
    );
    m_chart->SetMinSize(wxSize(400, 300));

    m_splitPrincipal->SplitHorizontally(
        painelSuperior,
        m_chart,
        150
    );

    wxBoxSizer* sizerPrincipal =
        new wxBoxSizer(wxVERTICAL);

    sizerPrincipal->Add(
        m_splitPrincipal,
        1,
        wxEXPAND
    );

    SetSizer(sizerPrincipal);
    sizerPrincipal->SetSizeHints(this);
    this->Fit(); 
    
    m_grid_agrupamento_mes->CreateGrid(1, 1);
    m_grid_agrupamento_mes->SetRowLabelSize(24);
    m_grid_agrupamento_mes->SetColLabelSize(24);
    m_grid_agrupamento_mes->Bind(wxEVT_GRID_CELL_LEFT_DCLICK, &FrameDashboard::OnGridAgrupamentoMesCellDoubleClickedl, this);

    m_grid_cupons->CreateGrid(1, 1);
    m_grid_cupons->SetRowLabelSize(24);
    m_grid_cupons->SetColLabelSize(24);
    m_grid_cupons->Bind(wxEVT_GRID_CELL_LEFT_DCLICK, &FrameDashboard::OnGridCuponsCellDoubleClickedl, this);

    Load();

}

FrameDashboard::~FrameDashboard()
{
    delete sqlite;
}

void FrameDashboard::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void FrameDashboard::OnAbout(wxCommandEvent& event)
{
    wxMessageBox(L"Este software é livre!\nObrigado por participar :-)",
        L"miliogo.com", wxOK | wxICON_INFORMATION);
}

void FrameDashboard::OnTerminalSQL(wxCommandEvent& event)
{
    FrameTerminalSQL* terminal_sql = new FrameTerminalSQL(this, sqlite);
    terminal_sql->Show();
}

void FrameDashboard::OnSincronizar(wxCommandEvent& event)
{
    FrameSincronizar frame_sincronizar(this, sqlite);
    frame_sincronizar.ShowModal();
    Load();
}

void FrameDashboard::OnGridAgrupamentoMesSelectCell(wxGridEvent& event)
{
    const char* sql_cupons =
        "select\n"
        "substr(c.data_hora_emissao, 7, 2) || '/' ||\n"
        "substr(c.data_hora_emissao, 5, 2) || '/' ||\n"
        "substr(c.data_hora_emissao, 1, 4) as data,\n"
        "c.valor_total,\n"
        "e.fantasia,\n"
        "e.endereco,\n"
        "c.id\n"
        "from cupom c\n"
        "join emitente e on c.cnpj_emitente = e.cnpj\n"
        "where c.id in(%s)\n"
        "order by c.data_hora_emissao desc\n"
        ;
    std::map<wxString, wxString> col_labels;
    col_labels["data"] = L"Data";
    col_labels["valor_total"] = L"Valor";
    col_labels["fantasia"] = L"Estabelecimento";
    col_labels["endereco"] = L"Endereço";
    wxString id_list;
    int row = std::max(event.GetRow(), 0);
    if (m_grid_agrupamento_mes->GetNumberRows()) {
        id_list = m_grid_agrupamento_mes->GetCellValue(wxGridCellCoords(row, 3));
    }
    else {
        id_list = "0";
    }
    sqlite->Execute(wxString::Format(sql_cupons, id_list), m_grid_cupons, col_labels);
    m_grid_cupons->HideCol(4);

    m_chart->SetData(m_grid_agrupamento_mes, 0, 1, row);

}

void FrameDashboard::OnGridCuponsCellDoubleClickedl(wxGridEvent& event)
{
    int row = event.GetRow();

    int id_cupom = wxAtoi(m_grid_cupons->GetCellValue(wxGridCellCoords(row, 4)));

    FrameCupom* cupom = new FrameCupom(this, sqlite, id_cupom);
    cupom->Show();

    event.Skip();
}

void FrameDashboard::OnGridAgrupamentoMesCellDoubleClickedl(wxGridEvent& event)
{
    int row = event.GetRow();
    event.Skip();

    wxString mes_ano = m_grid_agrupamento_mes->GetCellValue(wxGridCellCoords(row, 0));
    wxString id_list = m_grid_agrupamento_mes->GetCellValue(wxGridCellCoords(row, 3));
    
    FramePieChart* chart = new FramePieChart(this, mes_ano, sqlite, id_list);
    chart->Show();
}

void FrameDashboard::Load(void)
{
    const char* sql_agrupamento_mes =
        "select\n"
        "substr(c.data_hora_emissao, 5, 2) || '/' ||\n"
        "substr(c.data_hora_emissao, 1, 4) as data,\n"
        "round(sum(c.valor_total), 2) as valor_total,\n"
        "count(c.id) as qtde_cupons,\n"
        "group_concat(c.id) as cupom_ids\n"
        "from cupom c\n"
        "group by substr(c.data_hora_emissao, 1, 6)\n"
        "order by substr(c.data_hora_emissao, 1, 6) desc\n"
        ;
    std::map<wxString, wxString> col_labels;
    col_labels["data"] = L"Mês/Ano";
    col_labels["valor_total"] = L"Valor";
    col_labels["qtde_cupons"] = L"Cupons";

    sqlite->Execute(sql_agrupamento_mes, m_grid_agrupamento_mes, col_labels);
    m_grid_agrupamento_mes->HideCol(3);
    m_grid_agrupamento_mes->Bind(
        wxEVT_GRID_SELECT_CELL,
        &FrameDashboard::OnGridAgrupamentoMesSelectCell,
        this
    );

    auto ge = wxGridEvent();
    OnGridAgrupamentoMesSelectCell(ge);   


}

void FrameDashboard::OnLogoff(wxCommandEvent& event)
{
    int resposta = wxMessageBox(
        L"Deseja realmente fazer logoff?", 
        L"Confirmação",                            
        wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION
    );
    if (resposta == wxYES) {
        ClassLogin login(false);
        login.Logoff(this);
    }
}
