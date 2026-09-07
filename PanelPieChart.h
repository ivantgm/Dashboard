#pragma once

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/grid.h>
#include <vector>
#include <cmath>
#include "ClassSQLite.h"

struct PieData
{
    wxString label;
    double value;
    bool selected;
    bool outros;
};

class PanelPieChart : public wxPanel
{
public:
	PanelPieChart(wxWindow* parent);
    void SetData(const std::vector<PieData>& data);
    wxColour GetColor(size_t index);
private:
    std::vector<PieData> m_data;    
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
};

class FramePieChart : public wxFrame
{
private:
    wxGrid* grid;
    PanelPieChart* chart;
    wxSpinCtrl* spin;
    void AtualizaGrafico(int selected_row);
    void OnSpinChange(wxSpinEvent& event);
    void OnGridSelectCell(wxGridEvent& event);
public:
    FramePieChart(wxWindow* parent, wxString caption, ClassSQLite* sqlite, wxString id_list);
};

