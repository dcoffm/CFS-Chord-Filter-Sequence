/***************************************************************
 * Name:      wxGLMain.cpp
 * Purpose:
 * Author:    Keith Coffman (dcoffm5261@gmail.com)
 * Created:   2018-06-22
 **************************************************************/

#include "wxGLMain.h"

const long wxGLFrame::ID_buttonLoadSF = wxNewId();
const long wxGLFrame::ID_choicePreset = wxNewId();
const long wxGLFrame::ID_checkboxKill = wxNewId();
const long wxGLFrame::ID_sliderStrum = wxNewId();
const long wxGLFrame::ID_filterGrid = wxNewId();
const long wxGLFrame::ID_seqGrid = wxNewId();

BEGIN_EVENT_TABLE(wxGLFrame, wxFrame)
    EVT_CLOSE(wxGLFrame::OnClose)
    EVT_MENU(idMenuQuit, wxGLFrame::OnQuit)
    EVT_MENU(idMenuAbout, wxGLFrame::OnAbout)
    EVT_MENU(idMenuSave, wxGLFrame::SaveFile)
    EVT_MENU(idMenuLoad, wxGLFrame::LoadFile)
    EVT_BUTTON(wxGLFrame::ID_buttonLoadSF,wxGLFrame::OnLoadSFButton)
    EVT_CHOICE(wxGLFrame::ID_choicePreset,wxGLFrame::changePreset)

    /*
    EVT_GRID_CMD_CELL_LEFT_CLICK(wxGLFrame::ID_filterGrid,FilterGrid::PlayCell)
    EVT_GRID_CMD_CELL_RIGHT_CLICK(wxGLFrame::ID_filterGrid,FilterGrid::ToggleCell)
    EVT_GRID_CMD_CELL_LEFT_CLICK(wxGLFrame::ID_seqGrid,SequenceGrid::PlayCell)
    EVT_GRID_CMD_CELL_RIGHT_CLICK(wxGLFrame::ID_seqGrid,SequenceGrid::ToggleCell)

    EVT_GRID_CMD_LABEL_LEFT_CLICK(wxGLFrame::ID_filterGrid,FilterGrid::ColumnLeftClick)
    EVT_GRID_CMD_LABEL_RIGHT_CLICK(wxGLFrame::ID_filterGrid,FilterGrid::ColumnRightClick)
    EVT_GRID_CMD_LABEL_LEFT_CLICK(wxGLFrame::ID_seqGrid,SequenceGrid::ColumnLeftClick)
    EVT_GRID_CMD_LABEL_RIGHT_CLICK(wxGLFrame::ID_seqGrid,SequenceGrid::ColumnRightClick)
    */

    //EVT_KEY_DOWN(wxGLFrame::keyPressed)
END_EVENT_TABLE()


// Lay out the GUI
wxGLFrame::wxGLFrame(wxFrame *frame, const wxString& title)
    : wxFrame(frame, -1, title,wxPoint(0,0)){
    //: wxFrame(frame, -1, title,wxDefaultPosition){

    wxSize sz; // Generic size object for various uses
    wxGridBagSizer* mainGrid = new wxGridBagSizer(5,5); // Grid containing control menu and openGL window
    controlPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition,wxDefaultSize, wxTAB_TRAVERSAL, "mainPanel");
    this->SetSizer(mainGrid);
    mainGrid->Add(controlPanel,wxGBPosition(0,0),wxGBSpan(1,1),wxALIGN_LEFT,borderSize);

    controlGrid = new wxGridBagSizer(); int cgCntr = 0; // counter for rows in control grid
    controlPanel->SetSizer(controlGrid);

    buttonLoadSF = new wxButton(controlPanel,ID_buttonLoadSF, "Load Soundfont");
    controlGrid->Add(buttonLoadSF,wxGBPosition(cgCntr,0),wxGBSpan(1,1),wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL,borderSize);

    textSF = new wxStaticText(controlPanel,-1,"<No soundfont loaded.>",wxDefaultPosition,wxDefaultSize,wxST_ELLIPSIZE_MIDDLE);
    controlGrid->Add(textSF,wxGBPosition(cgCntr++,1),wxGBSpan(1,1),wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxST_ELLIPSIZE_MIDDLE,borderSize);

    choicePreset = new wxChoice(controlPanel,ID_choicePreset);
    controlGrid->Add(choicePreset,wxGBPosition(cgCntr++,1),wxGBSpan(1,1),wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT,borderSize);

    wxStaticText* textKill = new wxStaticText(controlPanel,-1,"Kill notes on new play");
    controlGrid->Add(textKill,wxGBPosition(cgCntr,0),wxGBSpan(1,1),wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL,borderSize);

    checkboxKill = new wxCheckBox(controlPanel,ID_checkboxKill,"");
    controlGrid->Add(checkboxKill,wxGBPosition(cgCntr++,1),wxGBSpan(1,1),wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT,borderSize);
    checkboxKill->SetValue(true);

    wxStaticText* textStrum = new wxStaticText(controlPanel,-1,"Strum delay");
    controlGrid->Add(textStrum,wxGBPosition(cgCntr,0),wxGBSpan(1,1),wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL,borderSize);
    sliderStrum = new wxSlider(controlPanel,ID_sliderStrum,50,0,500);
    controlGrid->Add(sliderStrum,wxGBPosition(cgCntr++,1),wxGBSpan(1,1),wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT,borderSize);


    // Information output
    wxStaticBoxSizer* infoBox = new wxStaticBoxSizer(wxVERTICAL,controlPanel,"Status");
    controlGrid->Add(infoBox,wxGBPosition(cgCntr++,0),wxGBSpan(1,2),wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND, borderSize);
    infoGrid = new wxGridBagSizer(1, 1);
    infoBox->Add(infoGrid);
    infoText = new wxStaticText(controlPanel, wxID_ANY, " \n", wxPoint(0,0));
    infoGrid->Add(infoText, wxGBPosition(0,0), wxGBSpan(1,2), wxALIGN_LEFT|wxALIGN_TOP, 0);

    int height = 750;
    int width = 900;
    int y0 = 180;

    seqGrid = new SequenceGrid(this,ID_seqGrid,wxPoint(0,y0),wxSize(width,height));

    wxStaticBoxSizer* chordBox = new wxStaticBoxSizer(wxVERTICAL,controlPanel,"Chords");
    mainGrid->Add(chordBox,wxGBPosition(0,1),wxGBSpan(1,1),wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND, borderSize);

    filterGrid = new FilterGrid(this,ID_filterGrid,wxPoint(width+1,y0),wxSize(width,height));


    // Setting up fluidsynth
    settings = new_fluid_settings();
    synth = new_fluid_synth(settings);
    player = new_fluid_player(synth);
    fluid_settings_setstr(settings, "audio.driver", "dsound");
    adriver = new_fluid_audio_driver(settings, synth);

    // load up a default soundfont
    LoadSF("soundfonts\\Timbres Of Heaven GM_GS_XG_SFX V 3.2 Final.sf2");


    // create a menu bar
    wxMenuBar* mbar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu("");
    fileMenu->Append(idMenuQuit, "&Quit\tAlt-F4", "Quit the application");
    fileMenu->Append(idMenuSave, "&Save", "Save the current chord sequence");
    fileMenu->Append(idMenuLoad, "&Load", "Load a chord sequence");
    mbar->Append(fileMenu, "&File");

    wxMenu* helpMenu = new wxMenu("");
    helpMenu->Append(idMenuAbout, "&About\tF1", "Show info about this application");
    mbar->Append(helpMenu, "&Help");
    SetMenuBar(mbar);

    Layout();
    SetMinSize(mainGrid->Fit(this));
    SetSize(1700,1000);

    //wxWindow* seqGridWin = seqGrid->GetGridWindow();

}

wxGLFrame::~wxGLFrame(){ }

void wxGLFrame::OnQuit(wxCommandEvent &event){
    delete_fluid_audio_driver(adriver);
    delete_fluid_player(player);
    delete_fluid_synth(synth);
    delete_fluid_settings(settings);
    Destroy();
}
void wxGLFrame::OnClose(wxCloseEvent &event) {
    delete_fluid_audio_driver(adriver);
    delete_fluid_player(player);
    delete_fluid_synth(synth);
    delete_fluid_settings(settings);
    Destroy();
}
void wxGLFrame::OnAbout(wxCommandEvent &event){
    wxString msg = "Made by\nKeith Coffman, dcoffm5261@gmail.com\n(2020)";
    wxMessageBox(msg, "About");
}

void wxGLFrame::SaveFile(wxCommandEvent& event){
    wxFileDialog d(this,"Save a chord sequence","","","Chord file(*.chord)|*chord",wxFD_SAVE);
    if(d.ShowModal() == wxID_OK){
        wxString path = d.GetPath();
        d.GetPath().EndsWith(".chord",&path);
        wxFile file(path+".chord", wxFile::write);
        if(!file.IsOpened()){
            wxMessageBox("Save failed.");
            return;
        }else{  // begin writing
            wxString str;
            const std::vector<Chorde>& sequence = seqGrid->getChordList();
            for(size_t i=0; i<sequence.size(); i++){
                str  = sequence[i].label;
                str += "\t";
                for(size_t j=0; j<sequence[i].notes.size(); j++){
                    str += sequence[i].notes[j].toString(true) +",";
                }
                str += "\n";
                file.Write(str);
            }
            file.Close();
            infoPrint("Save successful.");
        }
    }
}

void wxGLFrame::LoadFile(wxCommandEvent& event){
    wxFileDialog d(this,"Select a chord file","chords","","Chord file(*.chord)|*chord",wxFD_OPEN);
    if(d.ShowModal() == wxID_OK){
        wxFile file(d.GetPath(), wxFile::read);
        if(!file.IsOpened()){
            wxMessageBox("Failed to open file.");
            return;
        }else{
            seqGrid->clearChords();

            wxString rest;
            wxString rest2;
            wxString line;
            wxString line2;
            wxString str;
            file.ReadAll(&rest);
            while(!rest.IsEmpty()){
                line = rest.BeforeFirst('\n',&rest2); rest = rest2; // it messes up if you have the same rest destination as the source
                    Chorde thisChord = Chorde();
                    str = line.BeforeFirst('\t',&line2); line = line2;
                    thisChord.label = str;
                    while(!line.IsEmpty()){
                        str = line.BeforeFirst(',',&line2); line = line2;
                        if(str.length()==3) // very crude check to see if it is a valid note or garbage
                            thisChord.insertNote(Note(str));
                    }
                    seqGrid->appendChord(thisChord);
            }
            file.Close();
        }
    }else{ // Cancel button
        return;
    }
    infoPrint("Load successful.");
}

void wxGLFrame::OnLoadSFButton(wxCommandEvent& event){
    wxFileDialog d(this,"Select a soundfont file","soundfonts","","(*.sf2)|*.sf2",wxFD_OPEN);
    if(d.ShowModal() == wxID_OK){
        LoadSF(d.GetPath());
        return;
    }
    infoPrint("Failed to open file.");
}

void wxGLFrame::LoadSF(wxString filepath){
    int SFID_new = fluid_synth_sfload(synth, filepath, 1);
    if(SFID_new > 0){
        SFID = SFID_new;
        textSF->SetLabel(filepath);
        choicePreset->Clear();
        sf = fluid_synth_get_sfont_by_id(synth,SFID);
        fluid_sfont_iteration_start(sf);
        do{
            preset = fluid_sfont_iteration_next(sf); // will return null if no more soundfonts left
            if(preset != NULL){
                wxString str = "";
                str += wxString::Format("%03u",fluid_preset_get_banknum(preset));
                str += "-";
                str += wxString::Format("%03u",fluid_preset_get_num(preset));
                str += " ";
                str += fluid_preset_get_name(preset);
                choicePreset->Append(str);
            }
        }while(preset!= NULL);
        choicePreset->SetSelection(0);
        wxCommandEvent temp = wxCommandEvent();
        changePreset(temp);
        return;
    }
    wxMessageBox( wxT("Failed to load soundfont") );
}

void wxGLFrame::changePreset(wxCommandEvent& event){
    long banknum = 0;
    long num = 0;
    wxString str = choicePreset->GetString(choicePreset->GetSelection());
    str.Left(3).ToLong(&banknum);
    str.Mid(4,3).ToLong(&num);
    fluid_synth_program_select(synth,0,SFID,int(banknum),int(num));
}

// Utility function
void wxGLFrame::infoPrint(const char* str){
    //if(controlGrid->GetSize().y>glcan->getHeight()-100){
    //    wxString temp = "\n";
    //    info = info.AfterLast(temp.Last());
    //}
    infoText->SetLabel(str);
    infoText->Wrap(controlGrid->GetSize().x-2*(borderSize+1));
    infoGrid->Fit(infoText);
    Layout();
}

void wxGLFrame::fluidPlayNote(Note& note){
    if(checkboxKill->GetValue())
        fluidEndChord();
    fluid_synth_noteon(synth, 0, note.val, note.vel);
}

void wxGLFrame::fluidPlayChord(Chorde& chord){
    if(checkboxKill->GetValue())
        fluidEndChord();
    for(size_t i=0;i<chord.notes.size();i++){
        fluid_synth_noteon(synth, 0, chord.notes[i].val, chord.notes[i].vel);
        Sleep(sliderStrum->GetValue()); // offset on note start for strumming effect
    }
}

void wxGLFrame::fluidEndChord(){
    for(int i=0;i<128;i++){
        fluid_synth_noteoff(synth, 0, i);
    }
}


