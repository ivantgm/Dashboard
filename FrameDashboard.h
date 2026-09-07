#ifndef FRAMEDASHBOARD
#define FRAMEDASHBOARD

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "ClassSQLite.h"
#include <wx/grid.h>
#include <wx/splitter.h>
#include "PanelBarChart.h"

class FrameDashboard : public wxFrame
{
public:
    FrameDashboard();
    ~FrameDashboard();
private:
    wxSplitterWindow* m_splitPrincipal;
    wxSplitterWindow* m_splitSuperior;
    wxGrid* m_grid_agrupamento_mes;
    wxGrid* m_grid_cupons;
    PanelBarChart *m_chart;
private:
    wxString token;
    ClassSQLite *sqlite;    
private:
    void Load(void);
private:
    void OnLogoff(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnTerminalSQL(wxCommandEvent& event);
    void OnSincronizar(wxCommandEvent& event);
    void OnGridAgrupamentoMesSelectCell(wxGridEvent& event);
    void OnGridCuponsCellDoubleClickedl(wxGridEvent& event);
    void OnGridAgrupamentoMesCellDoubleClickedl(wxGridEvent& event);
};

#endif