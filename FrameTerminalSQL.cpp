#include "FrameTerminalSQL.h"

enum {
    ID_EXECUTAR_SQL = 10001
};

FrameTerminalSQL::FrameTerminalSQL(wxWindow *parent, ClassSQLite *sqlite)
	: wxFrame(parent, wxID_ANY, "Terminal SQL")
{
    this->sqlite = sqlite;
    wxAcceleratorEntry acc[1];
    acc[0].Set(wxACCEL_NORMAL, WXK_F5, ID_EXECUTAR_SQL);
    Bind(wxEVT_MENU, &FrameTerminalSQL::OnExecutarSQL, this, ID_EXECUTAR_SQL);    
    SetAcceleratorTable(wxAcceleratorTable(1, acc));

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

    m_tree = new wxTreeCtrl(
        m_splitSuperior,
        wxID_ANY,
        wxDefaultPosition,
        wxDefaultSize,
        wxTR_DEFAULT_STYLE |
        wxTR_HAS_BUTTONS |
        wxTR_LINES_AT_ROOT);

    wxTreeItemId raiz = m_tree->AddRoot("Estrutura dos dados");
    wxArrayString table_list;
    sqlite->GetTableNames(table_list);
    for (auto i : table_list) {
        wxTreeItemId tree_item = m_tree->AppendItem(raiz, i);
        wxArrayString field_list, type_list;
        sqlite->GetFieldNames(i, field_list, type_list);
        for (int j = 0; j < field_list.Count(); j++) {
            wxTreeItemId tree_item_2 = m_tree->AppendItem(tree_item, field_list[j]);
            m_tree->AppendItem(tree_item_2, type_list[j]);
        }
    }

    m_tree->Expand(raiz);

    m_texto = new wxStyledTextCtrl(
        m_splitSuperior,
        wxID_ANY
    );
    m_texto->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    m_texto->SetMarginWidth(0, 40); 
    m_texto->SetLexer(wxSTC_LEX_SQL);
    wxString keywords = "select insert update delete from where join left right inner group by having "
        "create table alter drop index view trigger procedure function returns set into values "
        "and or not is null in exists like primary key foreign unique constraint default check case when then else end";
    m_texto->SetKeyWords(0, keywords);
    wxFont font(11, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    m_texto->StyleSetFont(wxSTC_STYLE_DEFAULT, font);
    m_texto->StyleSetForeground(wxSTC_STYLE_DEFAULT, wxColour(30, 30, 30));
    m_texto->StyleClearAll(); 
    m_texto->StyleSetForeground(wxSTC_SQL_WORD, wxColour(0, 0, 200));        
    m_texto->StyleSetBold(wxSTC_SQL_WORD, true);
    m_texto->StyleSetForeground(wxSTC_SQL_STRING, wxColour(160, 32, 240));   
    m_texto->StyleSetForeground(wxSTC_SQL_CHARACTER, wxColour(160, 32, 240));
    m_texto->StyleSetForeground(wxSTC_SQL_NUMBER, wxColour(255, 100, 0));    
    m_texto->StyleSetForeground(wxSTC_SQL_COMMENT, wxColour(0, 150, 0));     
    m_texto->StyleSetForeground(wxSTC_SQL_COMMENTLINE, wxColour(0, 150, 0)); 
    m_texto->StyleSetForeground(wxSTC_SQL_COMMENTDOC, wxColour(0, 130, 0));  
    m_texto->StyleSetItalic(wxSTC_SQL_COMMENT, true);
    m_texto->StyleSetItalic(wxSTC_SQL_COMMENTLINE, true);

    m_splitSuperior->SplitVertically(
        m_tree,
        m_texto,
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

    m_grid = new wxGrid(
        m_splitPrincipal,
        wxID_ANY
    );
    m_grid->CreateGrid(20, 5);
    m_grid->SetDefaultColSize(120);

    m_splitPrincipal->SplitHorizontally(
        painelSuperior,
        m_grid,
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

}

void FrameTerminalSQL::OnExecutarSQL(wxCommandEvent &event)
{
    sqlite->Execute(m_texto->GetText(), m_grid);
    wxString sqlite_erro = sqlite->Erro();
    if (sqlite_erro != "") {
        wxMessageBox(sqlite_erro, "Erro", wxOK | wxICON_ERROR);
    }
}

