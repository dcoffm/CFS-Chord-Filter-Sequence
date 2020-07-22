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

#include <wx/thread.h>
#include <wx/event.h>

// My files
#include "wxGLApp.h"
#include "Notes.h"

#include <fluidsynth.h>




DECLARE_EVENT_TYPE(wxEVT_THREAD_UPDATE, -1)
DECLARE_EVENT_TYPE(wxEVT_THREAD_BUFFER, -1)

//extern std::vector<Material> matList;  // array of material types with properties (global)

class BasicGLPane;
class wxGLFrame: public wxFrame, public wxThreadHelper
{
    public:
        wxGLFrame(wxFrame *frame, const wxString& title);
        ~wxGLFrame();
        wxString info = ""; // String to store status text
        void infoPrint(const char* str);

        wxMutex* bufferMutex; // Mutex to make sure buffers are not changed while sending to GPU

    protected:
        DECLARE_EVENT_TABLE()

    private:
        enum{
            idMenuQuit = 1000,
            idMenuSave,
            idMenuLoad,
            idMenuAbout
        };

        enum ThreadTask{
            HELPTHREAD_LOAD = 0,
            HELPTHREAD_SAVE,
            HELPTHREAD_PLAY
        };

        // GUI variables
        int borderSize = 6;
        bool running=false; // Whether a simulation is already running
        int task = 0; // defines what task the thread should undertake when called

        // fluidsynth stuff
        fluid_settings_t* settings;
        fluid_synth_t* synth;
        fluid_player_t* player;
        fluid_audio_driver_t* adriver;
        int SFID; // id number for soundfont currently loaded
        fluid_preset_t* preset;
        fluid_sfont_t* sf;

        // utilities
        void fluidPlayChord(Chorde& chord);
        void fluidEndChord();
        void filterChords();
        void makeChordGrid();
        void makeSeqGrid();
        void clearSeqGrid();
        void paintCol(int c);

        // internal variables
        int strum_delay = 10;
        int nChords = 0;
        const int NChords = 35;
        const int NNotes = 97;
        const int topNote = 108;
        int Nseq = 0;
        int seqCntr = 0;

        Chorde input; // chord object representing input notes
        Chorde clipboard; // copying and pasting chords between sequence, matches, input
        Chorde key; // container to store key selection on the sequence grid
        AbsList absList;
        std::vector<AbsList>matches_; // list of sets of chroma (chords with different labels but same notes are not duplicated)
        std::vector<Chorde> matches; // list of chords containing the input notes
        std::vector<Chorde> chordSequence; // Sequence of chords making up a progression that the user has made or loaded

        wxColor selColor = wxColor(34,177,76);
        wxColor rootColor = wxColor(20,107,47);
        wxColor unselColor = wxColor(255,255,255);



        // Thread functions
        wxThread::ExitCode Entry();
        void sendUpdate(int type, const char* msg);
        void sendBuffer(int index, bool setCurrent);
        void OnThreadUpdate(wxThreadEvent& event);
        void OnThreadBuffer(wxThreadEvent& event);
        void thread_();     // general function to create the helper thread
        void thread_Save(); // Enumerated tasks
        void thread_Load();


        // Event callbacks
        void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnLoadSFButton(wxCommandEvent& event);
        void LoadSF(wxString filepath);
        void changePreset(wxCommandEvent& event);

        void LoadFile(wxCommandEvent& event);
        void SaveFile(wxCommandEvent& event);
        //void keyPressed(wxKeyEvent& event);
        //void keyReleased(wxKeyEvent& event);

        void LeftClickMatches(wxGridEvent& evt);
        void RightClickMatches(wxGridEvent& evt);
        void LeftClickSequence(wxGridEvent& evt);
        void RightClickSequence(wxGridEvent& evt);

        void PlayCellMatches(wxGridEvent& evt);
        void ToggleCellMatches(wxGridEvent& evt);
        void PlayCellSequence(wxGridEvent& evt);
        void ToggleCellSequence(wxGridEvent& evt);

        // Widget IDs for things I need info from
        static const long ID_buttonLoadSF;
        static const long ID_choicePreset;
        static const long ID_checkboxKill;
        static const long ID_sliderStrum;
        static const long ID_chordGrid;
        static const long ID_seqGrid;

        // Declarations for widgets
        wxPanel*        controlPanel;
        wxGridBagSizer* controlGrid;
        wxButton*       buttonLoadSF;
        wxChoice*       choicePreset;
        wxStaticText*   textSF;
        wxCheckBox*     checkboxKill;
        wxSlider*       sliderStrum;
        wxGridBagSizer* infoGrid;
        wxStaticText*   infoText;
        wxGrid*         chordGrid;
        wxGrid*         seqGrid;

};

class MyGridCellRenderer : public wxGridCellStringRenderer
{
public:
    virtual void Draw(wxGrid& grid,wxGridCellAttr& attr,wxDC& dc,const wxRect& rect,int row, int col,bool isSelected){
        wxGridCellStringRenderer::Draw(grid, attr, dc, rect, row, col, false);
    }
};

#endif // WXGLMAIN_H
