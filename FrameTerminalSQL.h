#pragma once

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/treectrl.h>
#include <wx/grid.h>
#include <wx/splitter.h>
#include <wx/stc/stc.h>
#include "ClassSQLite.h"

class FrameTerminalSQL : public wxFrame
{
public:
    FrameTerminalSQL(wxWindow *parent, ClassSQLite *sqlite);
private:
    ClassSQLite *sqlite;
    wxSplitterWindow *m_splitPrincipal;
    wxSplitterWindow *m_splitSuperior;
    wxTreeCtrl *m_tree;
    wxStyledTextCtrl *m_texto;
    wxGrid *m_grid;
private:
    void OnExecutarSQL(wxCommandEvent &event);
};

