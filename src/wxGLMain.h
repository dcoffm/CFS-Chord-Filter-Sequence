/***************************************************************
 * Name:      wxGLMain.h
 * Purpose:
 * Author:    Keith Coffman (dcoffm5261@gmail.com)
 * Created:   2018-06-22
 **************************************************************/

#ifndef WXGLMAIN_H
#define WXGLMAIN_H

// wxWidgets includes
#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/gbsizer.h>
#include <wx/file.h>
#include <wx/wfstream.h>
#include <wx/listctrl.h>
#include <wx/grid.h>
#include <wx/generic/grid.h>
#include <wx/colour.h>
#include <wx/event.h>
#include <wx/notebook.h>

// My files
#include "wxGLApp.h"
#include "Notes.h"
#include "ChordGrid.h"

#include <fluidsynth.h>


class wxGLFrame: public wxFrame
{
    public:
        wxGLFrame(wxFrame *frame, const wxString& title);
        ~wxGLFrame();
        wxString info = ""; // String to store status text
        void infoPrint(const char* str);

        Chorde clipboard; // copying and pasting chords between sequence, matches, input
        void fluidPlayNote(Note& note);
        void fluidPlayChord(Chorde& chord);
        void fluidEndChord();

    protected:
        DECLARE_EVENT_TABLE()

    private:
        enum{
            idMenuQuit = 1000,
            idMenuSave,
            idMenuLoad,
            idMenuAbout
        };

        // fluidsynth stuff
        fluid_settings_t* settings;
        fluid_synth_t* synth;
        fluid_player_t* player;
        fluid_audio_driver_t* adriver;
        int SFID; // id number for soundfont currently loaded
        fluid_preset_t* preset;
        fluid_sfont_t* sf;

        // Event callbacks
        void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void onTab(wxNotebookEvent& event);
        void OnLoadSFButton(wxCommandEvent& event);
        void LoadSF(wxString filepath);
        void changePreset(wxCommandEvent& event);
        void onKeyDown(wxKeyEvent& event);

        void LoadFile(wxCommandEvent& event);
        void SaveFile(wxCommandEvent& event);

        // Widget IDs for things I need info from
        static const long ID_buttonLoadSF;
        static const long ID_choicePreset;
        static const long ID_checkboxStrumdir;
        static const long ID_sliderStrum;
        static const long ID_filterGrid;
        static const long ID_seqGrid;
        static const long ID_notebook;
        static const long ID_sliderVel;

        // Declarations for widgets
        wxGridBagSizer* controlGrid;
        wxButton*       buttonLoadSF;
        wxChoice*       choicePreset;
        wxStaticText*   textSF;
        wxCheckBox*     checkboxStrumdir;
        wxSlider*       sliderStrum;
        wxSlider*       sliderVel;
        wxStaticBoxSizer* infoBox;
        wxStaticText*   infoText;
        FilterGrid*     filterGrid;
        SequenceGrid*   seqGrid;
        wxBoxSizer*     chordSizer;
        wxNotebook*     notebook;
};

#endif // WXGLMAIN_H
