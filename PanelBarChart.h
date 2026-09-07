#pragma once

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/grid.h>
#include <vector>
#include <algorithm>
#include <cmath>

struct BarData
{
	wxString label;
	double value;
	bool selected;
};

class PanelBarChart : public wxPanel
{
public:
	PanelBarChart(wxWindow* parent);
	void SetData(wxGrid *grid, int col_label, int col_value, int selected_row);
private:
	std::vector<BarData> m_data;
	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
};

