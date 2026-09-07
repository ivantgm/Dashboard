#include "FrameSincronizar.h"
#include "ClassLogin.h"
#include "wx/event.h"
#include <chrono>
#include <thread>

enum {
    ID_PROCESSAR = 10001,
    ID_CANCELAR
};

wxBEGIN_EVENT_TABLE(FrameSincronizar, wxDialog)
EVT_BUTTON(ID_PROCESSAR, FrameSincronizar::OnProcessar)
EVT_BUTTON(ID_CANCELAR, FrameSincronizar::OnCancelar)
wxEND_EVENT_TABLE()


FrameSincronizar::FrameSincronizar(wxWindow* parent, ClassSQLite* sqlite)
    : wxDialog(
        parent,
        wxID_ANY,
        "Sincronizar",
        wxDefaultPosition,
        wxSize(450, 450),
        wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    this->sqlite = sqlite;
    ClassLogin login(true);
    this->token = login.Token();

    auto* mainSizer = new wxBoxSizer(wxVERTICAL);

    m_progress = new wxStyledTextCtrl(
        this,
        wxID_ANY);

    m_btnProcessar = new wxButton(
        this,
        ID_PROCESSAR,
        "Processar");

    m_btnCancelar = new wxButton(
        this,
        ID_CANCELAR,
        "Cancelar");

    m_btnCancelar->Enable(false);

    auto* buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    buttonSizer->Add(
        m_btnProcessar,
        0,
        wxRIGHT,
        10);

    buttonSizer->Add(
        m_btnCancelar,
        0);

    mainSizer->Add(
        m_progress,
        1,
        wxEXPAND | wxALL,
        15);

    mainSizer->Add(
        buttonSizer,
        0,
        wxALIGN_RIGHT | wxLEFT | wxRIGHT | wxBOTTOM,
        15);

    SetSizer(mainSizer);
    Centre();
}


FrameSincronizar::~FrameSincronizar()
{
    if (m_thread.joinable()) {
        m_cancelRequested = true;
        m_thread.join();
    }
}


void FrameSincronizar::OnProcessar(wxCommandEvent& event)
{
    if (m_running)
        return;

    m_cancelRequested = false;
    m_running = true;

    m_progress->AddText("Processo iniciado.\n");
    m_progress->SetSelection(m_progress->GetLength(), m_progress->GetLength());
    m_progress->EnsureCaretVisible();


    m_btnProcessar->Enable(false);
    m_btnCancelar->Enable(true);

    m_thread = std::thread(
        &FrameSincronizar::Processar,
        this);
}


void FrameSincronizar::OnCancelar(wxCommandEvent& event)
{
    if (!m_running)
        return;

    m_cancelRequested = true;
    m_btnCancelar->Enable(false);
}


void FrameSincronizar::Processar()
{
    try  {

        wxString json;

        if (!BaixarJSON(json))
        {
            CallAfter(
                &FrameSincronizar::Finalizar,
                false,
                L"Erro ao baixar o JSON.");
            return;
        }

        if (m_cancelRequested)
        {
            CallAfter(
                &FrameSincronizar::Finalizar,
                false,
                L"Operação cancelada.");

            return;
        }
        int json_size = json.size();
        CallAfter(
            [this, json_size]()
            {
                if (m_progress) {
                    m_progress->AddText(wxString::Format("JSON baixado com sucesso: %d bytes.\n", json_size));
                    m_progress->SetSelection(m_progress->GetLength(), m_progress->GetLength());
                    m_progress->EnsureCaretVisible();

                }
            });



        nlohmann::json data;

        try {
            data = nlohmann::json::parse(
                json.ToStdString()
            );
        }
        catch (const std::exception& e)
        {
            CallAfter(
                &FrameSincronizar::Finalizar,
                false,
                wxString::Format(
                    "JSON inválido: %s",
                    e.what()));

            return;
        }
        int data_size = data.size();
        CallAfter(
            [this, data_size]()
            {
                if (m_progress) {
                    m_progress->AddText(L"Parser JSON concluído.\n");
                    m_progress->AddText(wxString::Format("Foram carregados %d cupons.\n", data_size));
                    m_progress->SetSelection(m_progress->GetLength(), m_progress->GetLength());
                    m_progress->EnsureCaretVisible();
                }
                    
            });


        if (m_cancelRequested)
        {
            CallAfter(
                &FrameSincronizar::Finalizar,
                false,
                L"Operação cancelada.");

            return;
        }


        if (!SalvarNoSQLite(data)) {
           
            if (m_cancelRequested)
            {
                CallAfter(
                    &FrameSincronizar::Finalizar,
                    false,
                    L"Operação cancelada.");

                return;
            }
            CallAfter(
                &FrameSincronizar::Finalizar,
                false,
                L"Erro ao salvar os dados no SQLite.");

            return;
        }

        if (m_cancelRequested)
        {
            CallAfter(
                &FrameSincronizar::Finalizar,
                false,
                L"Operação cancelada.");

            return;
        }

        CallAfter(
            &FrameSincronizar::Finalizar,
            true,
            L"Processamento concluído.");
    }
    catch (const std::exception& e)
    {
        CallAfter(
            &FrameSincronizar::Finalizar,
            false,
            wxString::Format(
                "Erro: %s",
                e.what()));
    }
    catch (...)
    {
        CallAfter(
            &FrameSincronizar::Finalizar,
            false,
            "Erro desconhecido.");
    }
}


bool FrameSincronizar::BaixarJSON(wxString& json)
{

    auto session = wxWebSessionSync::New();

    if (!session.IsOpened())
        return false;

    auto request = session.CreateRequest(
        "https://miliogo.com/cupom/consulta_cupom.php");

    if (!request.IsOk())
        return false;

    request.SetHeader(
        "Accept",
        "application/json");
    request.SetHeader(
        "secret-key",
        this->token);
    int ultimo_id;
    sqlite->GetLastCupomId(ultimo_id);

    nlohmann::json j;
    j["ultimo_id"] = ultimo_id;
    wxString data = wxString::FromUTF8(j.dump());
    wxString contentType = "application/json; charset=UTF-8";
    request.SetData(
        data,
        "application/json; charset=UTF-8"
    );

    auto result = request.Execute();

    if (!result)
        return false;

    json = request.GetResponse().AsString();

    return true;
}


bool FrameSincronizar::SalvarNoSQLite(nlohmann::json &data)
{
    sqlite->Importar(data,[this](wxString msg) -> bool {
        CallAfter(
            [this, msg]()
            {
                if (m_progress) {
                    m_progress->AddText(msg);
                    m_progress->SetSelection(m_progress->GetLength(), m_progress->GetLength());
                    m_progress->EnsureCaretVisible();
                }

            });
        return !m_cancelRequested;
    });
    return true;
}


void FrameSincronizar::Finalizar(
    bool sucesso,
    const wxString& mensagem)
{
    m_running = false;

    m_btnProcessar->Enable(true);
    m_btnCancelar->Enable(false);

    if (sucesso) {
        m_progress->AddText("Processo cocluído.\n");
        m_progress->SetSelection(m_progress->GetLength(), m_progress->GetLength());
        m_progress->EnsureCaretVisible();

    }

    if (m_thread.joinable())
        m_thread.join();

    wxMessageBox(
        mensagem,
        sucesso ? L"Concluído" : L"Processamento",
        sucesso ? wxOK | wxICON_INFORMATION
        : wxOK | wxICON_WARNING,
        this);
}