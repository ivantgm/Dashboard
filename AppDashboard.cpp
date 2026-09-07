#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "FrameDashboard.h"
#include "FrameLogin.h"
#include "ClassLogin.h"


class AppDashboard : public wxApp
{
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(AppDashboard);

bool AppDashboard::OnInit() {

    ClassLogin login(true);
    if (!login.IsValidToken()) {
        FrameLogin frame_login;
        frame_login.ShowModal();
        if (!frame_login.Ok()) {
            return false;
        }
    }

    FrameDashboard *frame = new FrameDashboard();
    frame->Show(true);
    return true;
}



