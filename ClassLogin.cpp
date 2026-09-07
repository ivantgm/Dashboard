#include "ClassLogin.h"
#include <wx/config.h>
#include <wx/webrequest.h>
#include <nlohmann/json.hpp>

ClassLogin::ClassLogin(bool load_config) {
	if (load_config) {
		LoadConfig();
	}
}

ClassLogin::~ClassLogin() {
	
}

bool ClassLogin::IsValidToken(void) const
{
	if (!token.empty()) {
		auto request = wxWebSessionSync::GetDefault().CreateRequest("https://miliogo.com/cupom/api.php");

		if (request.IsOk()) {
			request.SetHeader("secret-key", token);

			auto result = request.Execute();

			if (!result) {
				return false;
			} else if (request.State_Completed) {
				return true;
			}
		}
	}
	return false;
}

bool ClassLogin::Login(wxString usuario, wxString senha) {
	auto request = wxWebSessionSync::GetDefault().CreateRequest("https://miliogo.com/cupom/usuario.php");
	if (request.IsOk()) {
		nlohmann::json j;
		j["nome"] = usuario;
		j["senha"] = senha;
		j["funcao"] = "login";
		auto data = wxString::FromUTF8(j.dump());
		wxString contentType = "application/json; charset=UTF-8";		
		request.SetData(data, contentType);
		auto result = request.Execute();
		if (!result) {
			return false;
		}
		else if (request.State_Completed) {
			wxString response = request.GetResponse().AsString();
			nlohmann::json j = nlohmann::json::parse(response);
			wxString secret = wxString::FromUTF8(j["secret"].get<std::string>());
			wxString id = wxString::FromUTF8(j["secret"].get<std::string>());
			this->id = id;
			this->usuario = usuario;
			this->token = secret;
			SaveConfig();
			return true;
		}
	}
	return false;
}

void ClassLogin::Logoff(wxFrame *main_frame) {
	wxConfig config("CupomMiliogoDashboard");
	config.Write("/Login/Id", "");
	config.Write("/Login/Usuario", "");
	config.Write("/Login/Token", "");
	if (main_frame) {
		main_frame->Close();
	}
}

wxString ClassLogin::Usuario(void) const {
	return usuario;
}

wxString ClassLogin::Token(void) const {
	return token;
}

void ClassLogin::LoadConfig(void) {
	wxConfig config("CupomMiliogoDashboard");
	config.Read("/Login/Id", &id, "");
	config.Read("/Login/Usuario", &usuario, "");
	config.Read("/Login/Token", &token, "");
}

void ClassLogin::SaveConfig(void) const {
	wxConfig config("CupomMiliogoDashboard");
	config.Write("/Login/Id", id);
	config.Write("/Login/Usuario", usuario);
	config.Write("/Login/Token", token);
}
