/***************************************************************
 * Name:      wxGLApp.h
 * Purpose:   Defines Application Class
 * Author:    Keith Coffman (dcoffm5261@gmail.com)
 * Created:   2018-06-22
 * Copyright: Keith Coffman ()
 * License:
 **************************************************************/

#ifndef WXGLAPP_H
#define WXGLAPP_H

#include <wx/app.h>

class glApp : public wxApp
{
    public:
        virtual bool OnInit();
};

#endif // WXGLAPP_H
