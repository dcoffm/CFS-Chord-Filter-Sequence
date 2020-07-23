

#include "ChordGrid.h"
#include "Notes.h"
#include "wxGLMain.h"

//
// Chord Grid implementation
//
ChordGrid::ChordGrid(wxGLFrame *parnt, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
            : wxGrid(parnt,id,pos,size,style,name)
{
    parent = parnt;
    DisableDragGridSize();
    EnableEditing(false);
    SetCellHighlightPenWidth(0);
    SetDefaultRenderer(new MyGridCellRenderer);

    wxGridSizesInfo gzi = wxGridSizesInfo();
    gzi.m_sizeDefault = 38; this->SetColSizes(gzi);
    gzi.m_sizeDefault = 20; this->SetRowSizes(gzi);

    CreateGrid(NNotes,1); // first column is a list of all notes

    Bind(wxEVT_GRID_CELL_LEFT_CLICK, &ChordGrid::PlayCell, this);
    Bind(wxEVT_GRID_LABEL_LEFT_CLICK, &ChordGrid::ColumnLeftClick, this);
}
ChordGrid::~ChordGrid(){ }


void ChordGrid::paintInput(){
    this->SetColLabelValue(0,input.label);
    for(int r=0; r<NNotes; r++){
        Note rownote = topNote-r;
        if(rownote.chroma()==NOTE_C)
            this->SetRowLabelValue(r, wxString::Format("%i",rownote.octave()) );
        else
            this->SetRowLabelValue(r,wxEmptyString);
        this->SetCellValue(r,0,rownote.toString());

        // determine if note is to be painted green
        bool paint = false;
        for(size_t i=0;i<input.notes.size();i++){
            if(input.notes[i] == rownote)
                paint=true;
        }
        if(paint)
            this->SetCellBackgroundColour(r,0,selColor);
        else
            this->SetCellBackgroundColour(r,0,unselColor);
    }
}

void ChordGrid::paintCol(int c){
    for(int r=0; r<NNotes; r++){
        Note rownote = topNote-r;
        Chorde chord = chordList[c-1];

        if(r==0){
            wxString colLabel = chord.label;
            colLabel.Replace(" ","\n",false);
            this->SetColLabelValue(c,colLabel);
        }

        int paintLvl = 0;
        if(chord.chromaList[rownote.chroma()]){ // if it's in the chord
            paintLvl = 1;
            for(size_t i=0;i<chord.notes.size();i++){
                if(chord.notes[i] == rownote)      // if actually played
                    paintLvl = 2;
                if(chord.root == rownote)
                    paintLvl = 3;
            }
        }

        switch(paintLvl) {
            case 0 :
                SetCellBackgroundColour(r,c,unselColor);
                SetCellValue(r,c,"");
                break;
            case 1 :
                SetCellValue(r,c,rownote.toString());
                break;
            case 2 :
                SetCellValue(r,c,rownote.toString());
                SetCellBackgroundColour(r,c,selColor);
                break;
            case 3 :
                SetCellValue(r,c,rownote.toString());
                SetCellBackgroundColour(r,c,rootColor);
        }
    }
}

void ChordGrid::appendChord(Chorde& chord){
    chordList.push_back(chord);
    this->AppendCols(1);
    this->paintCol(chordList.size());
};

void ChordGrid::removeChord(int c){
    if(c>0 &&  c <= int(chordList.size())){
        chordList.erase(chordList.begin()+(c-1));
        this->DeleteCols(c);
    }
    if(c==0){
        input.notes.clear();
        paintInput();
    }
};

void ChordGrid::copyChord(int c){
    if(c==0)
        this->parent->clipboard = this->input;
    else if(c>0 && c<=int(this->chordList.size()))
        this->parent->clipboard = this->chordList[c-1];
    else{
        this->parent->clipboard = Chorde();
    }
}

void ChordGrid::ToggleCell(wxGridEvent& evt){
    int r = evt.GetRow();
    int c = evt.GetCol();
    Note rownote = topNote-r;

    if(GetCellBackgroundColour(r,c)==selColor){
        if(c==0){
            this->input.removeNote(rownote);
            this->paintInput();
        }
        if(c>0){
            this->chordList[c-1].removeNote(rownote);
            this->paintCol(c);
        }
    }
    else if(this->GetCellBackgroundColour(r,c)==unselColor && !this->GetCellValue(r,c).IsEmpty()){
        if(c==0){
            this->input.insertNote(rownote);
            this->paintInput();
        }
        if(c>0){
            this->chordList[c-1].insertNote(rownote);
            this->paintCol(c);
        }
    }

    parent->Refresh();
    parent->Update();
}

void ChordGrid::PlayCell(wxGridEvent& evt){
    int r = evt.GetRow();
    int c = evt.GetCol();
    Note rownote = topNote-r;

    if(!this->GetCellValue(r,c).IsEmpty())
        this->parent->fluidPlayNote(rownote);
}

void ChordGrid::playChord(int c){
    if(c==0)
        this->parent->fluidPlayChord(input);
    else if(c>0 && c<=int(chordList.size())){
        this->parent->fluidPlayChord(chordList[c-1]);
    }
    else
        this->parent->fluidEndChord();
}

void ChordGrid::ColumnLeftClick(wxGridEvent& evt){
    int c = evt.GetCol();
    if(evt.ShiftDown())
        copyChord(c);
    else
        playChord(c);
}


//
// Sequence Grid implementation
//
SequenceGrid::SequenceGrid(wxGLFrame *parnt, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
            : ChordGrid(parnt,id,pos,size,style,name)
{
    this->AppendCols(1);
    this->SetColLabelValue(1,"-");
    this->paintInput();
    Bind(wxEVT_GRID_CELL_RIGHT_CLICK, &SequenceGrid::ToggleCell, this);
    Bind(wxEVT_GRID_LABEL_RIGHT_CLICK, &SequenceGrid::ColumnRightClick, this);
}
SequenceGrid::~SequenceGrid(){ }

void SequenceGrid::clearChords(){ // run the first time to make the row labels and cols
    int ncols = this->GetNumberCols();
    if(ncols>2)
        this->DeleteCols(1,ncols-2);
    this->chordList.clear();
}

void SequenceGrid::ColumnRightClick(wxGridEvent& evt){
    size_t c = evt.GetCol();
    int r = evt.GetRow();
    if(r==-1){
    if( evt.ShiftDown() &&  evt.ControlDown()){ // insert chord before
        if(c>0 && c<=this->chordList.size()){
            chordList.insert(this->chordList.begin()+(c-1),this->parent->clipboard);
            this->InsertCols(c);
            this->paintCol(c);
        }
        if(c>this->chordList.size())
            this->appendChord(this->parent->clipboard);
    }
    if( evt.ShiftDown() && !evt.ControlDown()){ // paste chord (replace)
        if(c==0){
            this->input = this->parent->clipboard;
            this->input.label = "";
            this->paintInput();
        }
        if(c>0 && c<=this->chordList.size()){
            this->chordList[c-1] = this->parent->clipboard;
            paintCol(c);
        }
        if(c>this->chordList.size())
            this->appendChord(this->parent->clipboard);
    }
    if(!evt.ShiftDown() && !evt.ControlDown()) // delete chord
        this->removeChord(c);
    }
}




//
// Filter Grid implementation
//
FilterGrid::FilterGrid(wxGLFrame *parnt, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
            : ChordGrid(parnt,id,pos,size,style,name)
{
    this->input.label = "?";
    this->paintInput();
    Bind(wxEVT_GRID_CELL_RIGHT_CLICK, &FilterGrid::ToggleCell, this);
    Bind(wxEVT_GRID_LABEL_RIGHT_CLICK, &FilterGrid::ColumnRightClick, this);
}
FilterGrid::~FilterGrid(){ }

void FilterGrid::clearChords(){ // run the first time to make the row labels and cols
    int ncols = this->GetNumberCols();
    if(ncols>1)
        this->DeleteCols(1,ncols-1);
    chordList.clear();
}

void FilterGrid::ColumnRightClick(wxGridEvent& evt){
    size_t c = evt.GetCol();
    int r = evt.GetRow();
    if(r==-1){
    if( evt.ShiftDown() && !evt.ControlDown()){ // paste chord (replace)
        if(c==0){ // only meaningful to replace inputs
            input = this->parent->clipboard;
            input.label = "?";
            paintInput();
            applyFilter();
        }
    }
    if(!evt.ShiftDown() && !evt.ControlDown())
        this->removeChord(c);
    }
}

void FilterGrid::ToggleCell(wxGridEvent& evt){
    ChordGrid::ToggleCell(evt);
    if(evt.GetCol()==0)
        applyFilter();
}

void FilterGrid::applyFilter(){

    clearChords();

    if(input.notes.size()>0){
    for(int i=CHORD_M; i!=CHORD_UNK; i++){
        for(int j=-6; j<6; j++){ // choose j'th note as root. Does it find all the others?

            Note rootNote = input.notes[0]+j;

            Chorde thisChord(rootNote,static_cast<ChordType>(i));

            bool containsInput = false;
            for(size_t k=0; k<input.notes.size(); k++){
                containsInput = thisChord.chromaList[input.notes[k].chroma()];
                if(!containsInput)
                    break;
            }

            if(containsInput){ // found a chord that contains what we want; now check uniqueness

                bool isUniq = true;
                for(size_t k=0;k<chordList.size();k++){
                    bool isIdentical = true;
                    for(int c=0; c<12; c++){
                        if(thisChord.chromaList[c] != chordList[k].chromaList[c]){
                            isIdentical = false;
                            break;
                        }
                    }
                    if(isIdentical){
                        isUniq = false;
                        break;
                    }
                }

                if(isUniq){
                    // put the specific input notes into the chord
                    for(size_t k=0; k<input.notes.size(); k++){
                        thisChord.insertNote(input.notes[k]);
                    }
                    chordList.push_back(thisChord);
                }
            }
        }
    }}

    this->AppendCols(chordList.size());
    for(size_t c=1; c<=chordList.size(); c++){
        this->paintCol(c);
    }

    this->parent->infoPrint(wxString::Format("There are %i chords",int(chordList.size())));
}
