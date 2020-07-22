
#include "wxGLMain.h"

void wxGLFrame::thread_(){
    if(!running){
        info = ""; // Reset info text
        if(CreateThread(wxTHREAD_JOINABLE) !=  wxTHREAD_NO_ERROR){
            infoPrint("Error: Failed to create new thread.\n");
            return;
        }
        if(GetThread()->Run() != wxTHREAD_NO_ERROR){
            infoPrint("Error: Failed to create new thread.\n");
            return;
        }
        running = true;
    }
    else
        wxMessageBox("Another thread is already running");
//        infoPrint("Error: Another action is already in progress.\n");
}

void wxGLFrame::SaveFile(wxCommandEvent& event){
    task = HELPTHREAD_SAVE; thread_();
}
void wxGLFrame::LoadFile(wxCommandEvent& event){
    task = HELPTHREAD_LOAD; thread_();
}

wxThread::ExitCode wxGLFrame::Entry(){
    switch (task){
    case HELPTHREAD_LOAD:
        thread_Load(); break;
    case HELPTHREAD_SAVE:
        thread_Save(); break;
    }
    return (wxThread::ExitCode)0;
}

// EVENTS SENT FROM THREAD
void wxGLFrame::sendUpdate(int type, const char* msg){
    wxThreadEvent evt(wxEVT_THREAD_UPDATE, wxID_ANY);
    evt.SetInt(type);
    evt.SetString(msg);
    AddPendingEvent(evt);
}
void wxGLFrame::sendBuffer(int index, bool setCurrent){
    wxThreadEvent evt(wxEVT_THREAD_BUFFER, wxID_ANY);
    index += 1;
    if(!setCurrent)
        index *= -1;
    evt.SetInt(index);
    AddPendingEvent(evt);
}

// EVENTS PROCESSED BY FRAME
void wxGLFrame::OnThreadUpdate(wxThreadEvent& event){
    //if(event.GetInt() == 1) // Asks the OpenGL canvas to reset the view on behalf of the thread
        //glcan->resetView();

    if(event.GetInt() == 0){ // Status message; thread still running
        info += event.GetString();
        infoPrint(info);
    }
    else if(event.GetInt() == -1){ // Termination message; thread has closed (error or completion)
        info += event.GetString();
        infoPrint(info);
        running = false;
    }
}
void wxGLFrame::OnThreadBuffer(wxThreadEvent& event){
    int index = event.GetInt();   // NOTE: this is one-indexed for the sign
    //bool makeCurrent = index > 0; // positive means make it current
    index = abs(index)-1;
    //glcan->doBuffer(index,makeCurrent); // Send data to the OpenGL buffer
}
