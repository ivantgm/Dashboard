#pragma once
#include <wx/wx.h>
#include <sqlite3.h>
#include <wx/grid.h>
#include <nlohmann/json.hpp>
#include <functional>
#include <map>
#include <vector>

class ClassSQLite
{
public:
	ClassSQLite(void);
	~ClassSQLite(void);
public:
	wxString Erro(void) const;
	void GetTableNames(wxArrayString &table_list);
	void GetFieldNames(wxString table_name, wxArrayString &field_list);
	void GetFieldNames(wxString table_name, wxArrayString &field_list, wxArrayString &type_list);
	void Execute(const wxString& sql, wxGrid* grid);
	void Execute(const wxString &sql, wxGrid *grid, std::map<wxString, wxString>& col_labels);
	void Execute(const wxString& sql, std::map<wxString, wxString>& row);
	void Execute(const wxString& sql, std::vector<std::map<wxString, wxString>> &dataset);
	void GetLastCupomId(int &ultimo_id);
	void Importar(nlohmann::json& data, std::function<bool(wxString)>callback);
	void CriarBanco(void);
private:
	sqlite3 *db;
	wxString erro;
};

