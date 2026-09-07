#pragma once

#include <wx/wx.h>
#include <wx/webrequest.h>
#include <wx/stc/stc.h>
#include <nlohmann/json.hpp>
#include "ClassSQLite.h"

#include <atomic>
#include <thread>

class FrameSincronizar : public wxDialog
{
public:
    explicit FrameSincronizar(wxWindow* parent, ClassSQLite* sqlite);
    ~FrameSincronizar();

private:
    wxString token;
    ClassSQLite* sqlite;
    wxStyledTextCtrl* m_progress;
    wxButton* m_btnProcessar;
    wxButton* m_btnCancelar;

    std::thread m_thread;
    std::atomic_bool m_cancelRequested{ false };
    std::atomic_bool m_running{ false };

    void OnProcessar(wxCommandEvent& event);
    void OnCancelar(wxCommandEvent& event);

    void Processar();

    void Finalizar(bool sucesso, const wxString& mensagem);

    // Executada pela thread.
    bool BaixarJSON(wxString& json);

    // Apenas exemplo.
    bool SalvarNoSQLite(nlohmann::json &data);

    wxDECLARE_EVENT_TABLE();
};