/***************************************************************
 * Name:      wxGLApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Keith Coffman (dcoffm5261@gmail.com)
 * Created:   2018-06-22
 * Copyright: Keith Coffman ()
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "wxGLApp.h"
#include "wxGLMain.h"

IMPLEMENT_APP(glApp);

bool glApp::OnInit()
{
    wxGLFrame* frame = new wxGLFrame(0L, "Chord Finder");
    frame->SetIcon(wxICON(aaaa)); // To Set App Icon
    frame->Show();

    return true;
}
