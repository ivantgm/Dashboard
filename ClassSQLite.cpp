#include "ClassSQLite.h"
#include <exception>
#include <stdexcept>


ClassSQLite::ClassSQLite(void)
{    
    erro = "";
    if (sqlite3_open("banco.db", &db) != SQLITE_OK) {
        erro = wxString::Format("Erro ao abrir banco de dados SQLite: %s!", sqlite3_errmsg(db));
    }
    CriarBanco();
}

ClassSQLite::~ClassSQLite(void)
{
    sqlite3_close(db);
}

wxString ClassSQLite::Erro(void) const
{
    return erro;
}

void ClassSQLite::GetTableNames(wxArrayString &table_list) 
{
    erro = "";
    sqlite3_stmt *stmt;
    wxString sql = "SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%' ORDER BY name;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        erro = wxString::Format("Erro ao preparar consulta: %s!", sqlite3_errmsg(db));
    } else {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            table_list.Add(sqlite3_column_text(stmt, 0));
        }
        sqlite3_finalize(stmt);
    }
}

void ClassSQLite::GetFieldNames(wxString table_name, wxArrayString &field_list)
{
    erro = "";
    sqlite3_stmt* stmt;
    wxString sql = wxString::Format("SELECT name FROM pragma_table_info('%s')", table_name);
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        erro = wxString::Format("Erro ao preparar consulta: %s!", sqlite3_errmsg(db));
    }
    else {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            field_list.Add(sqlite3_column_text(stmt, 0));
        }
        sqlite3_finalize(stmt);
    }
}

void ClassSQLite::GetFieldNames(wxString table_name, wxArrayString &field_list, wxArrayString &type_list)
{
    erro = "";
    sqlite3_stmt* stmt;
    wxString sql = wxString::Format("SELECT name, type FROM pragma_table_info('%s')", table_name);
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        erro = wxString::Format("Erro ao preparar consulta: %s!", sqlite3_errmsg(db));
    }
    else {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            field_list.Add(sqlite3_column_text(stmt, 0));
            type_list.Add(sqlite3_column_text(stmt, 1));
        }
        sqlite3_finalize(stmt);
    }
}

void ClassSQLite::Execute(const wxString& sql, wxGrid* grid)
{
    std::map<wxString, wxString> col_labels;
    Execute(sql, grid, col_labels);
}

void ClassSQLite::Execute(const wxString& sql, std::map<wxString, wxString>& row)
{
    erro = "";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        erro = wxString::Format("Erro ao preparar consulta: %s!", sqlite3_errmsg(db));
    }
    else {
        if ((sqlite3_step(stmt) == SQLITE_DONE) && (!sqlite3_column_count(stmt))) {
            int linhas_afetadas = sqlite3_changes(db);
            row["linhas_afetadas"] = wxString::Format("%d", linhas_afetadas);
        }
        else {
            int total_colunas = sqlite3_column_count(stmt);
            sqlite3_reset(stmt);
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                for (int i = 0; i < total_colunas; i++) {
                    wxString valor = wxString::FromUTF8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)));
                    wxString nome_coluna = wxString::FromUTF8(sqlite3_column_name(stmt, i));
                    row[nome_coluna] = valor;
                }
            }
        }
        sqlite3_finalize(stmt);
    }
}

void ClassSQLite::Execute(const wxString& sql, std::vector<std::map<wxString, wxString>> &dataset)
{
    erro = "";
    sqlite3_stmt* stmt;    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        erro = wxString::Format("Erro ao preparar consulta: %s!", sqlite3_errmsg(db));
    }
    else {
        if ((sqlite3_step(stmt) == SQLITE_DONE) && (!sqlite3_column_count(stmt))) {
            int linhas_afetadas = sqlite3_changes(db);
            std::map<wxString, wxString> row;
            row["linhas_afetadas"] = wxString::Format("%d", linhas_afetadas);
            dataset.push_back(row);
        }
        else {
            int total_colunas = sqlite3_column_count(stmt);
            sqlite3_reset(stmt);
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                std::map<wxString, wxString> row;
                for (int i = 0; i < total_colunas; i++) {
                    wxString valor = wxString::FromUTF8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)));
                    wxString nome_coluna = wxString::FromUTF8(sqlite3_column_name(stmt, i));
                    row[nome_coluna] = valor;
                }
                dataset.push_back(row);
            }
        }
        sqlite3_finalize(stmt);
    }
}

void ClassSQLite::Execute(const wxString &sql, wxGrid *grid, std::map<wxString, wxString>& col_labels)
{
    erro = "";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        erro = wxString::Format("Erro ao preparar consulta: %s!", sqlite3_errmsg(db));
    }
    else {
        grid->BeginBatch();
        if (grid->GetNumberRows() > 0) {
            grid->DeleteRows(0, grid->GetNumberRows());
        }
        if (grid->GetNumberCols() > 0) {
            grid->DeleteCols(0, grid->GetNumberCols());
        }

        if ((sqlite3_step(stmt) == SQLITE_DONE) && (!sqlite3_column_count(stmt))) {
            int linhas_afetadas = sqlite3_changes(db);
            grid->AppendCols(1);
            grid->SetColLabelValue(0, "registros afetados");
            grid->AppendRows(1);
            grid->SetCellValue(grid->GetNumberRows() - 1, 0, wxString::Format("%d", linhas_afetadas));
        }
        else {            
            int total_colunas = sqlite3_column_count(stmt);
            grid->AppendCols(total_colunas);
            for (int i = 0; i < total_colunas; i++) {
                wxString nome_coluna = wxString::FromUTF8(sqlite3_column_name(stmt, i));
                wxString label_coluna = col_labels[nome_coluna];
                if (label_coluna != "") {
                    grid->SetColLabelValue(i, label_coluna);
                }
                else {
                    grid->SetColLabelValue(i, nome_coluna);
                }
                int tipo = sqlite3_column_type(stmt, i);
                switch (tipo) {
                    case SQLITE_INTEGER: 
                    case SQLITE_FLOAT: {
                        wxGridCellAttr* attr = new wxGridCellAttr();
                        attr->SetAlignment(wxALIGN_RIGHT, wxALIGN_CENTER_VERTICAL);
                        grid->SetColAttr(i, attr);
                    }
                }
            }
            sqlite3_reset(stmt);
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                grid->AppendRows(1);
                for (int i = 0; i < total_colunas; i++) {
                    wxString valor = wxString::FromUTF8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)));
                    grid->SetCellValue(grid->GetNumberRows() - 1, i, valor);
                }
            }
        }
        grid->AutoSizeColumns();
        grid->EndBatch();
        /*
        grid->SelectBlock(wxGridCellCoords(0, 0), wxGridCellCoords(0, 0));
        grid->SetGridCursor(wxGridCellCoords(0, 0));
        grid->MakeCellVisible(wxGridCellCoords(0, 0));        
        */
        sqlite3_finalize(stmt);
    }
}

void ClassSQLite::GetLastCupomId(int &ultimo_id) 
{
    erro = "";
    ultimo_id = 0;
    sqlite3_stmt* stmt;
    wxString sql = wxString::Format("SELECT MAX(id_miliogo) FROM cupom");
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        ultimo_id = 0;
    }
    else {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            ultimo_id = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
}

void ClassSQLite::Importar(nlohmann::json &data, std::function<bool(wxString)>callback)
{    
    const char* sql_insert_emitente =
        "INSERT OR REPLACE INTO emitente("
        "    cnpj,"
        "    ie,"
        "    im,"
        "    nome,"
        "    fantasia,"
        "    endereco,"
        "    bairro,"
        "    cep,"
        "    municipio"
        ")"
        "VALUES(? , ? , ? , ? , ? , ? , ? , ? , ? )"
    ;
    const char* sql_insert_cupom =
        "INSERT INTO cupom("
        "    data_hora_emissao,"
        "    numero_cfe,"
        "    numero_serie_sat,"
        "    chave_acesso,"
        "    url_consulta,"
        "    valor_total,"
        "    obs_cupom,"
        "    obs_inf,"
        "    total_tributos,"
        "    cnpj_emitente,"
        "    cpf_consumidor,"
        "    razao_social_consumidor,"
        "    id_miliogo"
        ")"
        "VALUES(? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? )"
        "RETURNING id"
    ;
    const char* sql_insert_cupom_item =
        "INSERT INTO cupom_item("
        "    id_cupom,"
        "    seq,"
        "    codigo,"
        "    descricao,"
        "    qtde,"
        "    un,"
        "    valor_unit,"
        "    tributos,"
        "    valor_total,"
        "    desconto"
        ")"
        "VALUES(? , ? , ? , ? , ? , ? , ? , ? , ? , ? )"
    ;
    
    CriarBanco();

    sqlite3_stmt* stmt;
    int last_insert_id_cupom = 0;


    sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
    int count_cupons = 0;
    for (auto d : data) {
        int id_miliogo = d.value("id", 0);
        if ((!d.contains("chave_acesso") || d["chave_acesso"].is_null())) {
            if (!callback(wxString::Format("%d\tsem chave de acesso, pulando\n", id_miliogo))) {
                return;
            }
            continue;
        }
        count_cupons++;
       
        wxString data_hora_emissao = wxString::FromUTF8(d["data_hora_emissao"].get<std::string>());
        wxString numero_cfe = wxString::FromUTF8(d["numero_cfe"].get<std::string>());
        wxString numero_serie_sat = wxString::FromUTF8(d["numero_serie_sat"].get<std::string>());
        wxString chave_acesso = wxString::FromUTF8(d["chave_acesso"].get<std::string>());
        wxString url_consulta;
        if (d.contains("url_consulta") && !d["url_consulta"].is_null()) {
            url_consulta = wxString::FromUTF8(d["url_consulta"].get<std::string>());
        }
        double valor_total = d["valor_total"].get<double>();
        wxString obs_cupom;
        if (d.contains("obs_cupom") && !d["obs_cupom"].is_null()) {
            obs_cupom = wxString::FromUTF8(d["obs_cupom"].get<std::string>()); 
        }         
        wxString obs_inf;
        if (d.contains("obs_inf") && !d["obs_inf"].is_null()) {
            obs_inf = wxString::FromUTF8(d["obs_inf"].get<std::string>());
        }
        double total_tributos = d["total_tributos"].get<double>();
        wxString emitente_cnpj = wxString::FromUTF8(d["emitente"]["cnpj"].get<std::string>());
        wxString emitente_ie = wxString::FromUTF8(d["emitente"]["ie"].get<std::string>());
        wxString emitente_im = wxString::FromUTF8(d["emitente"]["im"].get<std::string>());
        wxString emitente_nome = wxString::FromUTF8(d["emitente"]["nome"].get<std::string>());
        wxString emitente_fantasia = wxString::FromUTF8(d["emitente"]["fantasia"].get<std::string>());
        wxString emitente_endereco = wxString::FromUTF8(d["emitente"]["endereco"].get<std::string>());
        wxString emitente_bairro = wxString::FromUTF8(d["emitente"]["bairro"].get<std::string>());
        wxString emitente_cep = wxString::FromUTF8(d["emitente"]["cep"].get<std::string>());
        wxString emitente_municipio = wxString::FromUTF8(d["emitente"]["municipio"].get<std::string>());
        wxString cpf_consumidor = "";
        wxString razao_social_consumidor = "";

        if (sqlite3_prepare_v2(db, sql_insert_emitente, -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error(sqlite3_errmsg(db));
        }
        sqlite3_bind_text(stmt, 1, emitente_cnpj.ToUTF8(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, emitente_ie.ToUTF8(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, emitente_im.ToUTF8(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, emitente_nome.ToUTF8(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, emitente_fantasia.ToUTF8(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 6, emitente_endereco.ToUTF8(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 7, emitente_bairro.ToUTF8(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 8, emitente_cep.ToUTF8(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 9, emitente_municipio.ToUTF8(), -1, SQLITE_STATIC);
        int result = sqlite3_step(stmt);
        if (result != SQLITE_DONE) {
            throw std::runtime_error(sqlite3_errmsg(db));
        }
        sqlite3_finalize(stmt);

        if (sqlite3_prepare_v2(db, sql_insert_cupom, -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error(sqlite3_errmsg(db));
        }
        sqlite3_bind_text(stmt, 1, data_hora_emissao.ToUTF8(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, numero_cfe.ToUTF8(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, numero_serie_sat.ToUTF8(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, chave_acesso.ToUTF8(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, url_consulta.ToUTF8(), -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 6, valor_total);
        sqlite3_bind_text(stmt, 7, obs_cupom.ToUTF8(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 8, obs_inf.ToUTF8(), -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 9, total_tributos);
        sqlite3_bind_text(stmt, 10, emitente_cnpj.ToUTF8(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 11, cpf_consumidor.ToUTF8(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 12, razao_social_consumidor.ToUTF8(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 13, id_miliogo);
        result = sqlite3_step(stmt);
        if (result == SQLITE_ROW) {
            last_insert_id_cupom = sqlite3_column_int(stmt, 0); 
        } else {
            throw std::runtime_error(sqlite3_errmsg(db));
        }
        sqlite3_finalize(stmt);

        if (sqlite3_prepare_v2(db, sql_insert_cupom_item, -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error(sqlite3_errmsg(db));
        }
        for (auto item : d["itens"]) {
            int seq = item.value("seq", 0);
            wxString codigo = wxString::FromUTF8(item["codigo"].get<std::string>());
            wxString descricao = wxString::FromUTF8(item["descricao"].get<std::string>());
            double qtde = item["qtde"].get<double>();
            wxString un = wxString::FromUTF8(item["un"].get<std::string>());
            double valor_unit = item["valor_unit"].get<double>();
            double tributos = item["tributos"].get<double>();
            double valor_total = item["valor_total"].get<double>();
            double desconto = item["desconto"].get<double>();
            
            sqlite3_bind_int(stmt, 1, last_insert_id_cupom);
            sqlite3_bind_int(stmt, 2, seq);
            sqlite3_bind_text(stmt, 3, codigo.ToUTF8(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 4, descricao.ToUTF8(), -1, SQLITE_STATIC);
            sqlite3_bind_double(stmt, 5, qtde);
            sqlite3_bind_text(stmt, 6, un.ToUTF8(), -1, SQLITE_STATIC);
            sqlite3_bind_double(stmt, 7, valor_unit);
            sqlite3_bind_double(stmt, 8, tributos);
            sqlite3_bind_double(stmt, 9, valor_total);
            sqlite3_bind_double(stmt, 10, desconto);
            result = sqlite3_step(stmt);
            if (result != SQLITE_DONE) {
                throw std::runtime_error(sqlite3_errmsg(db));
            }
            sqlite3_reset(stmt);
        }
        sqlite3_finalize(stmt);

        if (!callback(wxString::Format("%d\t%s\t%s\n", id_miliogo, data_hora_emissao, emitente_nome))) {
            return;
        }
    } 
    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
    if (!callback(wxString::Format("Foram adicionados %d cupons.\n", count_cupons))) {
        return;
    }
}

void ClassSQLite::CriarBanco(void)
{
    const char* sql_script =
        "CREATE TABLE IF NOT EXISTS emitente("
        "    cnpj TEXT UNIQUE,"
        "    ie TEXT,"
        "    im TEXT,"
        "    nome TEXT,"
        "    fantasia TEXT,"
        "    endereco TEXT,"
        "    bairro TEXT,"
        "    cep TEXT,"
        "    municipio TEXT"
        ");"
        "CREATE TABLE IF NOT EXISTS cupom("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    data_hora_emissao TEXT,"
        "    numero_cfe TEXT,"
        "    numero_serie_sat TEXT,"
        "    chave_acesso TEXT UNIQUE,"
        "    valor_total REAL,"
        "    total_tributos REAL,"
        "    obs_cupom TEXT,"
        "    obs_inf TEXT,"
        "    cnpj_emitente TEXT,"
        "    cpf_consumidor TEXT,"
        "    razao_social_consumidor TEXT,"
        "    enviado INTEGER DEFAULT 0,"
        "    url_consulta TEXT,"
        "    id_miliogo INTEGER"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_cupom_data_hora_emissao ON cupom(data_hora_emissao);"
        "CREATE INDEX IF NOT EXISTS idx_cupom_cnpj_emitente ON cupom(cnpj_emitente);"
        "CREATE TABLE IF NOT EXISTS cupom_item("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    id_cupom INTEGER,"
        "    seq INTEGER,"
        "    codigo TEXT,"
        "    descricao TEXT,"
        "    qtde REAL,"
        "    un TEXT,"
        "    valor_unit REAL,"
        "    tributos REAL,"
        "    valor_total REAL,"
        "    desconto REAL"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_cupom_item_id_cupom ON cupom_item(id_cupom);"
        "CREATE INDEX IF NOT EXISTS idx_cupom_item_codigo ON cupom_item(codigo);"
        "CREATE INDEX IF NOT EXISTS idx_cupom_item_descricao ON cupom_item(descricao);"
    ;
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql_script, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        throw std::runtime_error(errMsg);
    }

}

