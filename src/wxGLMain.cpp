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
const long wxGLFrame::ID_chordGrid = wxNewId();
const long wxGLFrame::ID_seqGrid = wxNewId();

BEGIN_EVENT_TABLE(wxGLFrame, wxFrame)
    EVT_CLOSE(wxGLFrame::OnClose)
    EVT_MENU(idMenuQuit, wxGLFrame::OnQuit)
    EVT_MENU(idMenuAbout, wxGLFrame::OnAbout)
    EVT_MENU(idMenuSave, wxGLFrame::SaveFile)
    EVT_MENU(idMenuLoad, wxGLFrame::LoadFile)
    EVT_BUTTON(wxGLFrame::ID_buttonLoadSF,wxGLFrame::OnLoadSFButton)
    EVT_CHOICE(wxGLFrame::ID_choicePreset,wxGLFrame::changePreset)

    EVT_GRID_CMD_CELL_LEFT_CLICK(wxGLFrame::ID_chordGrid,wxGLFrame::PlayCellMatches)
    EVT_GRID_CMD_CELL_RIGHT_CLICK(wxGLFrame::ID_chordGrid,wxGLFrame::ToggleCellMatches)
    EVT_GRID_CMD_CELL_LEFT_CLICK(wxGLFrame::ID_seqGrid,wxGLFrame::PlayCellSequence)
    EVT_GRID_CMD_CELL_RIGHT_CLICK(wxGLFrame::ID_seqGrid,wxGLFrame::ToggleCellSequence)

    EVT_GRID_CMD_LABEL_LEFT_CLICK(wxGLFrame::ID_chordGrid,wxGLFrame::LeftClickMatches)
    EVT_GRID_CMD_LABEL_RIGHT_CLICK(wxGLFrame::ID_chordGrid,wxGLFrame::RightClickMatches)
    EVT_GRID_CMD_LABEL_LEFT_CLICK(wxGLFrame::ID_seqGrid,wxGLFrame::LeftClickSequence)
    EVT_GRID_CMD_LABEL_RIGHT_CLICK(wxGLFrame::ID_seqGrid,wxGLFrame::RightClickSequence)

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

    seqGrid = new wxGrid(this,ID_seqGrid,wxPoint(0,y0),wxSize(width,height));
    seqGrid->DisableDragGridSize();
    seqGrid->EnableEditing(false);
    seqGrid->SetCellHighlightPenWidth(0);
    seqGrid->SetDefaultRenderer(new MyGridCellRenderer);
    wxGridSizesInfo gzi = wxGridSizesInfo();
    gzi.m_sizeDefault = 38;
    seqGrid->SetColSizes(gzi);
    gzi.m_sizeDefault = 20;
    seqGrid->SetRowSizes(gzi);
    makeSeqGrid();

    wxStaticBoxSizer* chordBox = new wxStaticBoxSizer(wxVERTICAL,controlPanel,"Chords");
    mainGrid->Add(chordBox,wxGBPosition(0,1),wxGBSpan(1,1),wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND, borderSize);
    chordGrid = new wxGrid(this,ID_chordGrid,wxPoint(width+1,y0),wxSize(width,height));
    chordGrid->DisableDragGridSize();
    chordGrid->EnableEditing(false);
    chordGrid->SetCellHighlightPenWidth(0);
    chordGrid->SetDefaultRenderer(new MyGridCellRenderer);
    gzi.m_sizeDefault = 38;
    chordGrid->SetColSizes(gzi);
    gzi.m_sizeDefault = 20;
    chordGrid->SetRowSizes(gzi);
    chordGrid->CreateGrid(NNotes,1);
    makeChordGrid();

    input.label = "?";

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
            for(size_t i=0; i<chordSequence.size(); i++){
                str  = chordSequence[i].label;
                str += "\t";
                for(size_t j=0; j<chordSequence[i].notes.size(); j++){
                    str += chordSequence[i].notes[j].toString(false,true) +",";
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
            clearSeqGrid();

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
                    chordSequence.push_back(thisChord);
                    seqGrid->InsertCols(chordSequence.size());
                    paintCol(chordSequence.size());
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

void wxGLFrame::LeftClickMatches(wxGridEvent& evt){
    size_t c = evt.GetCol();
    int r = evt.GetRow();
    if(r==-1){
    if(evt.ShiftDown()){
        if(c==0)
            clipboard = input;
        else
            clipboard = matches[c-1];
    }else{
        if(c==0)
            fluidPlayChord(input);
        else
            fluidPlayChord(matches[c-1]);
    }
    }
}

void wxGLFrame::RightClickMatches(wxGridEvent& evt){
    size_t c = evt.GetCol();
    int r = evt.GetRow();
    if(r==-1){
    if( evt.ShiftDown() &&  evt.ControlDown()){ // insert chord before
        // meaningless in context of matches grid
    }
    if( evt.ShiftDown() && !evt.ControlDown()){ // paste chord (replace)
        if(c==0){ // only meaningful to replace inputs
            input = clipboard;
            input.label = "?";
            filterChords();
        }
    }
    if(!evt.ShiftDown() && !evt.ControlDown()){ // delete chord (a match you're not interested in, except input...)
        if(c!=0)
            matches.erase(matches.begin()+c-1);
    }
    }
    makeChordGrid();
}

void wxGLFrame::LeftClickSequence(wxGridEvent& evt){
    size_t c = evt.GetCol();
    int r = evt.GetRow();
    if(r==-1){
    if(evt.ShiftDown()){
        if(c>0 && c<=chordSequence.size())
            clipboard = chordSequence[c-1];
        if(c>chordSequence.size())
            clipboard = Chorde();
    }else{
        if(c>0 && c<=chordSequence.size())
            fluidPlayChord(chordSequence[c-1]);

    }
    }
}

void wxGLFrame::RightClickSequence(wxGridEvent& evt){
    size_t c = evt.GetCol();
    int r = evt.GetRow();
    if(r==-1){
    if( evt.ShiftDown() &&  evt.ControlDown()){ // insert chord before
        if(c>0 && c<=chordSequence.size()){
            chordSequence.insert(chordSequence.begin()+(c-1),clipboard);
            seqGrid->InsertCols(c);
            paintCol(c);
        }
        if(c>chordSequence.size()){
            chordSequence.push_back(clipboard);
            seqGrid->InsertCols(c);
            paintCol(c);
        }
    }
    if( evt.ShiftDown() && !evt.ControlDown()){ // paste chord (replace)
        if(c>0 && c<=chordSequence.size()){
            chordSequence[c-1] = clipboard;
            paintCol(c);
        }
        if(c>chordSequence.size()){
            chordSequence.push_back(clipboard);
            seqGrid->InsertCols(c);
            paintCol(c);
        }

    }
    if(!evt.ShiftDown() && !evt.ControlDown()){ // delete chord
        if(c!=0 && c<=chordSequence.size()){
            chordSequence.erase(chordSequence.begin()+(c-1));
            seqGrid->DeleteCols(c);
        }

    }
    }
}

void wxGLFrame::PlayCellMatches(wxGridEvent& evt){
    int r = evt.GetRow();
    int c = evt.GetCol();
    Note note = Note(topNote-r);

    if(checkboxKill->GetValue())
        fluidEndChord();
    if(!chordGrid->GetCellValue(r,c).IsEmpty()){
        fluid_synth_noteon(synth, 0, note.val, 100);
    }
}

void wxGLFrame::PlayCellSequence(wxGridEvent& evt){
    int r = evt.GetRow();
    int c = evt.GetCol();
    Note note = Note(topNote-r);

    if(checkboxKill->GetValue())
        fluidEndChord();
    if(!seqGrid->GetCellValue(r,c).IsEmpty()){
        fluid_synth_noteon(synth, 0, note.val, 100);
    }
}

void wxGLFrame::ToggleCellMatches(wxGridEvent& evt){
    int r = evt.GetRow();
    int c = evt.GetCol();
    if(!chordGrid->GetCellValue(r,c).IsEmpty()){ // if the cell is a valid option
    if(c==0){
        if(chordGrid->GetCellBackgroundColour(r,c)==selColor){
            chordGrid->SetCellBackgroundColour(r,c,unselColor);
            input.removeNote(Note(topNote-r));
        }else{
            chordGrid->SetCellBackgroundColour(r,c,selColor);
            input.insertNote(Note(topNote-r));
        }

        filterChords();
    }
    if(c>0){
        if(chordGrid->GetCellBackgroundColour(r,c)==selColor || chordGrid->GetCellBackgroundColour(r,c)==rootColor){
            chordGrid->SetCellBackgroundColour(r,c,unselColor);
            matches[c-1].removeNote(Note(topNote-r));
        }else{
            if(topNote-r==matches[c-1].root.val)
                chordGrid->SetCellBackgroundColour(r,c,rootColor);
            else
                chordGrid->SetCellBackgroundColour(r,c,selColor);
            matches[c-1].insertNote(Note(topNote-r));
        }
    }}

    Refresh();
    Update();
}

void wxGLFrame::ToggleCellSequence(wxGridEvent& evt){
    int r = evt.GetRow();
    int c = evt.GetCol();
    if(c==0){
        if(seqGrid->GetCellBackgroundColour(r,c)==selColor){
            seqGrid->SetCellBackgroundColour(r,c,unselColor);
            key.removeNote(Note(topNote-r));
        }else{
            seqGrid->SetCellBackgroundColour(r,c,selColor);
            key.insertNote(Note(topNote-r));
        }
    }

    if(!seqGrid->GetCellValue(r,c).IsEmpty() && c>0){ // if the cell is a valid option
        if(seqGrid->GetCellBackgroundColour(r,c)==selColor){
            seqGrid->SetCellBackgroundColour(r,c,unselColor);
            chordSequence[c-1].removeNote(Note(topNote-r));
        }else{
            seqGrid->SetCellBackgroundColour(r,c,selColor);
            chordSequence[c-1].insertNote(Note(topNote-r));
        }
    }

    Refresh();
    Update();
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

void wxGLFrame::filterChords(){
    matches.clear();
    matches_.clear();

    if(input.notes.size()>0){
    for(int i=CHORD_M; i!=CHORD_UNK; i++){
        for(int j=-6; j<6; j++){ // choose j'th note as root. Does it find all the others?
            int rootNote = input.notes[0].val+j;

            Chorde thisChord = input; // re-copy the input list
            thisChord.ct = static_cast<ChordType>(i);
            thisChord.root = Note(rootNote);
            thisChord.defaultLabel();

            for(int k=0; k<12; k++){ absList.l[k]=false; }
            int m=0;

            for(int l=0;l<5;l++){
                // check if this chroma is already in the notes of thisChord, and if not add it
                int absNote = (rootNote+chordSteps[i][l])%12;
                absList.l[absNote] = true;

                // check if this satisfies one of the inputs
                for(size_t k=0; k<input.notes.size(); k++){
                    int thisNote = input.notes[k].val%12;
                    if(thisNote== absNote && (chordSteps[i][l]!=0 || l==0)){
                        m++;
                    }
                }

                // add the note to the matched chord
                bool alreadyPresent = false;
                for(size_t k=0; k<thisChord.notes.size(); k++){
                    int thisNote = thisChord.notes[k].val%12;
                    if(thisNote== absNote && (chordSteps[i][l]!=0 || l==0)){
                        alreadyPresent = true;
                    }
                }
                if(!alreadyPresent)
                    thisChord.insertNote(Note(rootNote+chordSteps[i][l]));
            }
            if(m==(int)input.notes.size()){ // found a chord that contains what we want; now check uniqueness
                bool isUniq = true;
                for(size_t k=0;k<matches_.size();k++){
                    bool somethingNew = false;
                    for(int l=0;l<12;l++){
                        somethingNew = somethingNew || (matches_[k].l[l] != absList.l[l]) ;
                    }
                    if(!somethingNew){isUniq = false; break;}
                }
                if(isUniq){
                    matches.push_back(thisChord);
                    matches_.push_back(absList);
                }
            }
        }
    }}
    nChords = matches.size();
    makeChordGrid();
    infoPrint(wxString::Format("There are %i chords",nChords));
}

void wxGLFrame::makeChordGrid(){

    if(chordGrid->GetNumberCols()>1)
        chordGrid->DeleteCols(1,chordGrid->GetNumberCols()-1);
    chordGrid->AppendCols(nChords);

    for(int r=0; r<NNotes; r++){
        Note note;
        note.val = topNote-r;
        if(note.val%12==0)
            chordGrid->SetRowLabelValue(r,wxString::Format("%i",note.val/12-1));
        else
            chordGrid->SetRowLabelValue(r,wxEmptyString);

        for(int c=0; c<=nChords; c++){
            if(c==0){
                chordGrid->SetColLabelValue(c,"In-\nput");
                chordGrid->SetCellValue(r,c,note.toString());
                bool isInput = false;
                for(size_t i=0;i<input.notes.size();i++){
                    if(note.val==input.notes[i].val)
                        isInput = true;
                }
                if(isInput)
                    chordGrid->SetCellBackgroundColour(r,c,selColor);
                else
                    chordGrid->SetCellBackgroundColour(r,c,unselColor);
            }
            else{
                Chorde a = matches[c-1];

                wxString colLabel = a.label;
                colLabel.Replace(" ","\n",false);
                chordGrid->SetColLabelValue(c,colLabel);

                for(size_t i=0;i<a.notes.size();i++){
                    if(note.val == a.notes[i].val)
                        chordGrid->SetCellBackgroundColour(r,c,selColor);
                    if(note.val%12 == a.notes[i].val%12)
                        chordGrid->SetCellValue(r,c,note.toString());
                }
                if(note.val == a.root.val)
                    chordGrid->SetCellBackgroundColour(r,c,rootColor);
            }
        }
    }

}

void wxGLFrame::paintCol(int c){
        for(int r=0; r<NNotes; r++){
        Note note;
        note.val = topNote-r;

        Chorde chord = chordSequence[c-1];

        if(r==0){
            wxString colLabel = chord.label;
            colLabel.Replace(" ","\n",false);
            seqGrid->SetColLabelValue(c,colLabel);
        }

        bool paint = false;
        bool text = false;
        for(size_t i=0;i<chord.notes.size();i++){
            if(chord.notes[i].val == note.val)      // if actually played
                paint = true;
            if(chord.notes[i].val%12 == note.val%12) // if it shares the chroma of selected notes
                text = true;
        }

        if(paint){seqGrid->SetCellBackgroundColour(r,c,selColor);}
        else{seqGrid->SetCellBackgroundColour(r,c,unselColor);}
        if(text){seqGrid->SetCellValue(r,c,note.toString());}
        else{seqGrid->SetCellValue(r,c,"");}

    }
}

void wxGLFrame::makeSeqGrid(){ // run the first time to make the row labels and cols

    seqGrid->CreateGrid(NNotes,2); // first column is all notes, second column is empty for inserting
    seqGrid->SetColLabelValue(0,"");
    seqGrid->SetColLabelValue(1,"-");

    for(int r=0; r<NNotes; r++){
        Note note;
        note.val = topNote-r;
        if(note.val%12==0)
            seqGrid->SetRowLabelValue(r,wxString::Format("%i",note.val/12-1));
        else
            seqGrid->SetRowLabelValue(r,wxEmptyString);
        seqGrid->SetCellValue(r,0,note.toString());
    }
}
void wxGLFrame::clearSeqGrid(){ // run the first time to make the row labels and cols
    seqGrid->DeleteCols(1,seqGrid->GetNumberCols()-2);
    chordSequence.clear();
}
