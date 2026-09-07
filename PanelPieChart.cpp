#include "PanelPieChart.h"
#include <wx/spinctrl.h>
#include <wx/splitter.h>

PanelPieChart::PanelPieChart(wxWindow* parent) : wxPanel(parent)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    Bind(wxEVT_PAINT,
        &PanelPieChart::OnPaint,
        this);

    Bind(wxEVT_SIZE,
        &PanelPieChart::OnSize,
        this);
}

void PanelPieChart::SetData(const std::vector<PieData>& data)
{
    m_data = data;
    Refresh();
}

wxColour PanelPieChart::GetColor(size_t index)
{
    static const wxColour colors[] =
    {
        wxColour(70, 130, 180),
        wxColour(220, 100, 100),
        wxColour(100, 180, 120),
        wxColour(230, 180,  70),
        wxColour(150, 110, 190),
        wxColour(90, 180, 180),
        wxColour(190, 140, 100),
        wxColour(140, 140, 140)
    };

    constexpr size_t colorCount =
        sizeof(colors) / sizeof(colors[0]);

    return colors[index % colorCount];
}

void PanelPieChart::OnPaint(wxPaintEvent&)
{
    wxAutoBufferedPaintDC dc(this);

    wxSize size = GetClientSize();

    dc.SetBackground(
        wxBrush(wxColour(245, 245, 245))
    );
    dc.Clear();

    if (m_data.empty()) {
        return;
    }        

    double total = 0.0;

    for (const auto& item : m_data) {
        if (item.value > 0.0) {
            total += item.value;
        }            
    }

    if (total <= 0.0) {
        return;
    }        

    const int margin = 20;

    int chartWidth =  size.GetWidth() - margin * 2;
    int chartHeight =  size.GetHeight() - margin * 2;

    if (chartWidth <= 0 ||  chartHeight <= 0) {
        return;
    }

    int centerX = margin + chartWidth / 2;

    int centerY = margin + chartHeight / 2;

    int radius =
        std::min(
            chartWidth,
            chartHeight
        ) / 2;

    radius -= 10;

    if (radius <= 0) {
        return;
    }        

    wxFont font(
        10,
        wxFONTFAMILY_DEFAULT,
        wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_NORMAL
    );

    dc.SetFont(font);


    //
    // wxWidgets considera:
    //
    //       90°
    //        |
    //        |
    // 180° --+-- 0°
    //        |
    //        |
    //      270°
    //
    // O desenho é feito no sentido anti-horário.
    //
    // Começamos em 90°, ou seja, no topo.
    //

    double startAngle = 90.0;

    for (size_t i = 0; i < m_data.size(); ++i) {
        
        const PieData& item = m_data[i];

        if (item.value <= 0.0)
            continue;

        double percentage =
            item.value / total;

        double sweepAngle =
            percentage * 360.0;

        double endAngle =
            startAngle + sweepAngle;

        wxColour color = GetColor(i);

        if (item.outros) {
            color = *wxLIGHT_GREY;
        }

        dc.SetBrush(wxBrush(color));

        dc.SetPen(
            wxPen(
                *wxWHITE,
                2
            )
        );
        
        if (item.selected) {
            double rad = (startAngle+(endAngle-startAngle)/2) * M_PI / 180.0;
            int cx = centerX + 20 * cos(rad);
            int cy = centerY - 20 * sin(rad);
            dc.DrawEllipticArc(
                cx - radius,
                cy - radius,
                radius * 2,
                radius * 2,
                startAngle,
                endAngle
            );
        }
        else {
            dc.DrawEllipticArc(
                centerX - radius,
                centerY - radius,
                radius * 2,
                radius * 2,
                startAngle,
                endAngle
            );
        }

        if (percentage >= 0.04)
        {
            double middleAngle = startAngle + sweepAngle / 2.0;
            double radians = middleAngle * M_PI / 180.0;

            // Distância do centro até o texto
            double textRadius =
                radius * 0.62;

            int textX =
                centerX +
                static_cast<int>(
                    std::cos(radians) *
                    textRadius
                    );

            int textY =
                centerY -
                static_cast<int>(
                    std::sin(radians) *
                    textRadius
                    );

            wxString text =
                wxString::Format(
                    "%.1f%%",
                    percentage * 100.0
                );

            wxFont percentageFont(
                10,
                wxFONTFAMILY_DEFAULT,
                wxFONTSTYLE_NORMAL,
                wxFONTWEIGHT_BOLD
            );

            dc.SetFont(percentageFont);

            wxSize textSize = dc.GetTextExtent(text);

            dc.SetTextForeground(*wxWHITE);

            dc.DrawText(
                text,
                textX -
                textSize.GetWidth() / 2,
                textY -
                textSize.GetHeight() / 2
            );

            dc.SetFont(font);
        }

        startAngle = endAngle;
    }



}


void PanelPieChart::OnSize(wxSizeEvent& event)
{
    Refresh();

    event.Skip();
}

FramePieChart::FramePieChart(wxWindow* parent, wxString caption, ClassSQLite* sqlite, wxString id_list)
    : wxFrame(
        parent,
        wxID_ANY,
        caption,
        wxDefaultPosition,
        wxSize(800, 500)
    )
{
    auto* sizer = new wxBoxSizer(wxHORIZONTAL);
    auto* sizer_left = new wxBoxSizer(wxVERTICAL);
    auto* sizer_spin = new wxBoxSizer(wxHORIZONTAL);    

    auto* split = new wxSplitterWindow(
        this,
        wxID_ANY,
        wxDefaultPosition,
        wxDefaultSize,
        wxSP_LIVE_UPDATE | wxSP_3D);

    auto* painelLeft = new wxPanel(split, wxID_ANY);
    chart = new PanelPieChart(split);
    
    auto* label_spin = new wxStaticText(painelLeft, wxID_ANY, L"Número máximo de fatias na pizza: ");
    spin = new wxSpinCtrl(painelLeft, wxID_ANY, L"10",
        wxDefaultPosition, wxDefaultSize,
        wxSP_ARROW_KEYS,
        2, 100, 10);
    spin->Bind(wxEVT_SPINCTRL, &FramePieChart::OnSpinChange, this);    
    grid = new wxGrid(painelLeft, wxID_ANY);
    grid->SetMinSize(wxSize(300, 300));
    grid->EnableEditing(false);
    grid->SetTabBehaviour(wxGrid::Tab_Leave);
    grid->CreateGrid(1, 1);
    grid->SetRowLabelSize(24);
    grid->SetColLabelSize(24);
    grid->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);
    grid->Bind(
        wxEVT_GRID_SELECT_CELL,
        &FramePieChart::OnGridSelectCell,
        this
    );

    painelLeft->SetSizer(sizer_left);

    sizer_spin->Add(label_spin, 0, wxEXPAND | wxALL, 4);
    sizer_spin->Add(spin, 1, wxEXPAND | wxALL, 4);
    
    sizer_left->Add(sizer_spin, 0, wxEXPAND | wxALL, 4);
    sizer_left->Add(grid, 1, wxEXPAND | wxALL, 4);

    sizer->Add(
        split,
        1,
        wxEXPAND,
        4
    );
    SetSizer(sizer);
    split->SplitVertically(painelLeft, chart, 300);

    const char* sql_agrupamento_mes =
        "select\n"
        "round(sum(valor_total - desconto), 2) as total,\n"
        "0 as perc,"
        "i.descricao\n"
        "from cupom_item i\n"
        "where i.id_cupom in(%s)\n"
        "group by i.descricao\n"
        "order by total desc\n"
        ;
    std::map<wxString, wxString> col_labels = {
        {"total","Valor"},
        {"perc"," (%) "},
        {"descricao","Produto"}
    };
    sqlite->Execute(wxString::Format(sql_agrupamento_mes, id_list), grid, col_labels);
    double sum = 0;
    for (int i = 0; i < grid->GetNumberRows(); ++i) {
        wxString valor = grid->GetCellValue(i, 0);
        sum += wxAtof(valor);
    }
    for (int i = 0; i < grid->GetNumberRows(); ++i) {
        double valor = wxAtof(grid->GetCellValue(i, 0));
        double perc = valor * 100 / sum;
        grid->SetCellValue(i, 1, wxString::Format("%.1f", perc));
    }
    grid->AutoSizeColumns();

    AtualizaGrafico(-1);

    
     

}

void FramePieChart::AtualizaGrafico(int selected_row)
{
    std::vector<PieData> data;
    int max_fatias = spin->GetValue();

    if (selected_row < 0) {
        selected_row = grid->GetGridCursorRow();
    }
    
    for (int row = 0; row < grid->GetNumberRows(); row++) {
        if (data.size() < max_fatias) {
            wxColour color = chart->GetColor(row);
            data.push_back(
                {
                    grid->GetCellValue(row, 2).SubString(0, 1),
                    wxAtof(grid->GetCellValue(row, 0)),
                    row == selected_row,
                    false
                }
            );
            grid->SetCellBackgroundColour(row, 1, color);
        } else {
            auto it = std::prev(data.end());
            it->label = "Outros";
            it->selected = it->selected || (row == selected_row);
            it->value += wxAtof(grid->GetCellValue(row, 0));
            it->outros = true;
            grid->SetCellBackgroundColour(row-1, 1, *wxLIGHT_GREY);
            grid->SetCellBackgroundColour(row, 1, *wxLIGHT_GREY);
        }
    }
    chart->SetData(data);
    Refresh();
}

void FramePieChart::OnSpinChange(wxSpinEvent& event)
{
    AtualizaGrafico(-1);
}

void FramePieChart::OnGridSelectCell(wxGridEvent& event)
{
    int row = std::max(event.GetRow(), 0);
    AtualizaGrafico(row);
}