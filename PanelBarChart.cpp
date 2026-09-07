#include "PanelBarChart.h"
#include "PanelPieChart.h"

PanelBarChart::PanelBarChart(wxWindow* parent) : wxPanel(parent)
{
	SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &PanelBarChart::OnPaint, this);
    Bind(wxEVT_SIZE, &PanelBarChart::OnSize, this);
}

void PanelBarChart::SetData(wxGrid* grid, int col_label, int col_value, int selected_row)
{
    m_data.clear();
    for (int row = 0; row < grid->GetNumberRows(); row++) {
        m_data.push_back(
            {
                grid->GetCellValue(row, col_label).SubString(0, 1),
                wxAtof(grid->GetCellValue(row, col_value)),
                row == selected_row
            }           
        
        );
        
    }
    Refresh();
}

void PanelBarChart::OnPaint(wxPaintEvent& event)
    {
        wxAutoBufferedPaintDC dc(this);

        wxSize size = GetClientSize();

        // Fundo
        dc.SetBackground(wxBrush(wxColour(245, 245, 245)));
        dc.Clear();

        if (m_data.empty())
            return;

        // ----------------------------------------------------
        // Margens
        // ----------------------------------------------------

        const int left   = 70;
        const int right  = 30;
        const int top    = 30;
        const int bottom = 60;

        const int chartWidth =
            size.GetWidth() - left - right;

        const int chartHeight =
            size.GetHeight() - top - bottom;

        if (chartWidth <= 0 || chartHeight <= 0)
            return;


        // ----------------------------------------------------
        // Valor máximo
        // ----------------------------------------------------

        double maxValue = 0;

        for (const auto& item : m_data)
            maxValue = std::max(maxValue, item.value);

        if (maxValue <= 0)
            maxValue = 1;


        // ----------------------------------------------------
        // Ajusta o máximo para um valor "bonito"
        // ----------------------------------------------------

        double scale = std::pow(10.0,
                                std::floor(std::log10(maxValue)));

        double chartMax =
            std::ceil(maxValue / scale) * scale;

        // Garante pelo menos 5 divisões
        double step = chartMax / 5.0;


        // ----------------------------------------------------
        // Eixos
        // ----------------------------------------------------

        wxPen axisPen(wxColour(60, 60, 60), 2);

        dc.SetPen(axisPen);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);

        // eixo Y
        dc.DrawLine(
            left,
            top,
            left,
            top + chartHeight
        );

        // eixo X
        dc.DrawLine(
            left,
            top + chartHeight,
            left + chartWidth,
            top + chartHeight
        );


        // ----------------------------------------------------
        // Linhas horizontais e valores do eixo Y
        // ----------------------------------------------------

        dc.SetFont(
            wxFont(
                9,
                wxFONTFAMILY_DEFAULT,
                wxFONTSTYLE_NORMAL,
                wxFONTWEIGHT_NORMAL
            )
        );

        for (int i = 0; i <= 5; ++i)
        {
            double value = step * i;

            int y =
                top +
                chartHeight -
                static_cast<int>(
                    (value / chartMax) * chartHeight
                );

            // Linha da grade
            wxPen gridPen(
                wxColour(210, 210, 210),
                1,
                wxPENSTYLE_DOT
            );

            dc.SetPen(gridPen);

            dc.DrawLine(
                left,
                y,
                left + chartWidth,
                y
            );

            // Valor
            dc.SetTextForeground(
                wxColour(70, 70, 70)
            );

            wxString text =
                wxString::Format("%.0f", value);

            wxSize textSize =
                dc.GetTextExtent(text);

            dc.DrawText(
                text,
                left - textSize.GetWidth() - 8,
                y - textSize.GetHeight() / 2
            );
        }


        // ----------------------------------------------------
        // Barras
        // ----------------------------------------------------

        const int count =
            static_cast<int>(m_data.size());

        const double slotWidth =
            static_cast<double>(chartWidth) / count;

        // Espaço entre barras
        const double barWidth =
            slotWidth * 0.60;


        for (int i = 0; i < count; ++i)
        {
            const auto& item = m_data[i];

            // altura da barra
            int barHeight =
                static_cast<int>(
                    (item.value / chartMax) *
                    chartHeight
                );

            // posição X
            int x =
                left +
                static_cast<int>(
                    i * slotWidth +
                    (slotWidth - barWidth) / 2
                );

            // posição Y
            int y =
                top +
                chartHeight -
                barHeight;


            // ------------------------------------------------
            // Barra
            // ------------------------------------------------

            if (item.selected) {
                dc.SetBrush(
                    wxBrush(wxColour(35, 65, 90))
                );

            }
            else {
                dc.SetBrush(
                    wxBrush(wxColour(70, 130, 180))
                );
            }

            dc.SetPen(
                wxPen(wxColour(50, 100, 150))
            );

            dc.DrawRectangle(
                x,
                y,
                static_cast<int>(barWidth),
                barHeight
            );


            // ------------------------------------------------
            // Valor acima da barra
            // ------------------------------------------------

            wxString valueText =
                wxString::Format("%.0f", item.value);

            wxSize valueSize =
                dc.GetTextExtent(valueText);

            dc.SetTextForeground(
                wxColour(40, 40, 40)
            );

            dc.DrawText(
                valueText,
                x +
                    (static_cast<int>(barWidth) -
                     valueSize.GetWidth()) / 2,
                y - valueSize.GetHeight() - 4
            );


            // ------------------------------------------------
            // Label
            // ------------------------------------------------

            wxSize labelSize =
                dc.GetTextExtent(item.label);

            int labelX =
                x +
                (static_cast<int>(barWidth) -
                 labelSize.GetWidth()) / 2;

            int labelY =
                top +
                chartHeight +
                10;

            dc.DrawText(
                item.label,
                labelX,
                labelY
            );
        }
    }

void PanelBarChart::OnSize(wxSizeEvent& event)
{
    Refresh();
    event.Skip();
}