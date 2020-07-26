/***************************************************************
 * Name:      wxGLMain.cpp
 * Purpose:
 * Author:    Keith Coffman (dcoffm5261@gmail.com)
 * Created:   2018-06-22
 **************************************************************/

#include "wxGLMain.h"

const long wxGLFrame::ID_buttonLoadSF = wxNewId();
const long wxGLFrame::ID_choicePreset = wxNewId();
const long wxGLFrame::ID_checkboxStrumdir = wxNewId();
const long wxGLFrame::ID_sliderStrum = wxNewId();
const long wxGLFrame::ID_sliderVel = wxNewId();
const long wxGLFrame::ID_filterGrid = wxNewId();
const long wxGLFrame::ID_seqGrid = wxNewId();
const long wxGLFrame::ID_notebook = wxNewId();

BEGIN_EVENT_TABLE(wxGLFrame, wxFrame)
    EVT_CLOSE(wxGLFrame::OnClose)
    EVT_MENU(idMenuQuit, wxGLFrame::OnQuit)
    EVT_MENU(idMenuAbout, wxGLFrame::OnAbout)
    EVT_MENU(idMenuSave, wxGLFrame::SaveFile)
    EVT_MENU(idMenuLoad, wxGLFrame::LoadFile)
    EVT_BUTTON(wxGLFrame::ID_buttonLoadSF,wxGLFrame::OnLoadSFButton)
    EVT_CHOICE(wxGLFrame::ID_choicePreset,wxGLFrame::changePreset)
    EVT_NOTEBOOK_PAGE_CHANGED(wxGLFrame::ID_notebook,wxGLFrame::onTab)

    EVT_KEY_DOWN(wxGLFrame::onKeyDown)

END_EVENT_TABLE()


// Lay out the GUI
wxGLFrame::wxGLFrame(wxFrame *frame, const wxString& title)
    : wxFrame(frame, -1, title,wxDefaultPosition){

    int borderSize = 6;
    int cgCntr = 0; // counter for rows in control grid

    wxPanel* controlPanel = new wxPanel(this, wxID_ANY);
    wxGridBagSizer* controlGrid = new wxGridBagSizer();

    buttonLoadSF = new wxButton(controlPanel,ID_buttonLoadSF, "Load Soundfont");
    controlGrid->Add(buttonLoadSF,wxGBPosition(cgCntr,0),wxGBSpan(1,1),wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL,borderSize);

    textSF = new wxStaticText(controlPanel,-1,"<No soundfont loaded.>",wxDefaultPosition,wxDefaultSize,wxST_ELLIPSIZE_MIDDLE);
    controlGrid->Add(textSF,wxGBPosition(cgCntr++,1),wxGBSpan(1,1),wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxST_ELLIPSIZE_MIDDLE,borderSize);

    choicePreset = new wxChoice(controlPanel,ID_choicePreset);
    controlGrid->Add(choicePreset,wxGBPosition(cgCntr++,1),wxGBSpan(1,1),wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT,borderSize);

    wxStaticText* textStrum = new wxStaticText(controlPanel,-1,"Strum delay");
    controlGrid->Add(textStrum,wxGBPosition(cgCntr,0),wxGBSpan(1,1),wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL,borderSize);
    sliderStrum = new wxSlider(controlPanel,ID_sliderStrum,50,0,500);
    controlGrid->Add(sliderStrum,wxGBPosition(cgCntr++,1),wxGBSpan(1,1),wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT,borderSize);

    wxStaticText* textStrumdir = new wxStaticText(controlPanel,-1,"Strum descending");
    controlGrid->Add(textStrumdir,wxGBPosition(cgCntr,0),wxGBSpan(1,1),wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL,borderSize);

    checkboxStrumdir = new wxCheckBox(controlPanel,ID_checkboxStrumdir,"");
    controlGrid->Add(checkboxStrumdir,wxGBPosition(cgCntr++,1),wxGBSpan(1,1),wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT,borderSize);

    wxStaticText* textVel = new wxStaticText(controlPanel,-1,"MIDI velocity");
    controlGrid->Add(textVel,wxGBPosition(cgCntr,0),wxGBSpan(1,1),wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL,borderSize);
    sliderVel = new wxSlider(controlPanel,ID_sliderVel,85,0,127);
    controlGrid->Add(sliderVel,wxGBPosition(cgCntr++,1),wxGBSpan(1,1),wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT,borderSize);

    // Information output
    wxStaticBoxSizer* infoBox = new wxStaticBoxSizer(wxVERTICAL,controlPanel,"Status");
    controlGrid->Add(infoBox,wxGBPosition(cgCntr++,0),wxGBSpan(1,2),wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND, borderSize);
    infoText = new wxStaticText(controlPanel, wxID_ANY, " \n", wxPoint(0,0));
    infoBox->Add(infoText, wxALIGN_LEFT|wxALIGN_TOP, 0);

    // The Chord Grids
    seqGrid = new SequenceGrid(this,ID_seqGrid);
    filterGrid = new FilterGrid(this,ID_filterGrid);
    seqGrid->sibling = filterGrid;
    filterGrid->sibling = seqGrid;

    // Setting up fluidsynth
    settings = new_fluid_settings();
    synth = new_fluid_synth(settings);
    player = new_fluid_player(synth);
    fluid_settings_setstr(settings, "audio.driver", "dsound");
    adriver = new_fluid_audio_driver(settings, synth);

    // load up a default soundfont
    LoadSF("soundfonts\\Timbres Of Heaven GM_GS_XG_SFX V 3.2 Final.sf2");


    wxBoxSizer* vertSizer = new wxBoxSizer(wxVERTICAL);
    chordSizer = new wxBoxSizer(wxHORIZONTAL);
    vertSizer->Add(controlPanel,0,wxALIGN_LEFT,borderSize);
    vertSizer->Add(chordSizer,1,wxEXPAND,0);
    chordSizer->Add(seqGrid,1,wxEXPAND,0);
    chordSizer->Add(filterGrid,1,wxEXPAND,0);
    controlPanel->SetSizer(controlGrid);
    this->SetSizer(vertSizer);

     // Notebook to provide tabs for changing which grid(s) are displayed
    notebook = new wxNotebook(controlPanel,ID_notebook,wxDefaultPosition,wxSize(200,24),wxNB_TOP);
    controlGrid->Add(notebook,wxGBPosition(cgCntr++,0),wxGBSpan(1,2),wxLEFT,borderSize);
    notebook->SetPageSize(wxSize(0,0));
    wxWindow* dummypage = new wxWindow(notebook,-1);
    notebook->InsertPage(0,dummypage,"Sequence",false);
    notebook->InsertPage(1,dummypage,"Filter",false);
    notebook->InsertPage(2,dummypage,"Both",true);

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

    //Layout();
    infoPrint("Welcome! Click a note to get started.");
    SetMinSize(vertSizer->Fit(this));
    Maximize();
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

void wxGLFrame::onKeyDown(wxKeyEvent& event){
    if(event.ControlDown()){
        //infoPrint(wxString::Format("%c",event.GetUnicodeKey()));
        wxCommandEvent temp;
        switch (event.GetUnicodeKey()){
        case 'S' :
            SaveFile(temp);
            break;
        case 'L' :
            LoadFile(temp);
            break;
        case 'Z' :
            // TODO: simple undo
            break;
        }
    }
}

void wxGLFrame::onTab(wxNotebookEvent& event){
    int s2 = event.GetSelection();
    int w1,w2,h1,h2;
    seqGrid->GetSize(&w1,&h1);
    filterGrid->GetSize(&w2,&h2);
    seqGrid->SetMaxSize(wxSize(10000,10000));
    filterGrid->SetMaxSize(wxSize(10000,10000));
    if(s2==0)
        filterGrid->SetMaxSize(wxSize(0,10000));
    if(s2==1)
        seqGrid->SetMaxSize(wxSize(0,10000));

    Layout();
    event.Skip();
}

void wxGLFrame::SaveFile(wxCommandEvent& event){
    wxFileDialog d(this,"Save a chord sequence","chords","","Chord file(*.chord)|*chord",wxFD_SAVE);
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
            wxString rest, rest2, line, line2, str;
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
    infoText->SetLabel(str);
    //infoText->Wrap(controlGrid->GetSize().x-2*(borderSize+1));
    //infoBox->Fit(infoText);
    Layout();
}

void wxGLFrame::fluidPlayNote(Note& note){
    fluidEndChord();
    fluid_synth_noteon(synth, 0, note.val, sliderVel->GetValue());
}

void wxGLFrame::fluidPlayChord(Chorde& chord){
    fluidEndChord();
    if(checkboxStrumdir->GetValue()){
        for(auto i=chord.notes.rbegin(); i!=chord.notes.rend(); ++i){
            fluid_synth_noteon(synth, 0, (*i).val, sliderVel->GetValue());
            Sleep(sliderStrum->GetValue()); // offset on note start for strumming effect
        }
    }else{
        for(size_t i=0;i<chord.notes.size();i++){
            fluid_synth_noteon(synth, 0, chord.notes[i].val, sliderVel->GetValue());
            Sleep(sliderStrum->GetValue());
        }
    }
}

void wxGLFrame::fluidEndChord(){
    for(int i=0;i<128;i++){
        fluid_synth_noteoff(synth, 0, i);
    }
}


