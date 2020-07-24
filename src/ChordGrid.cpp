

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
    thisWin = GetGridWindow();
    DisableDragGridSize();
    DisableDragColSize();
    DisableDragRowSize();
    EnableEditing(false);
    SetCellHighlightPenWidth(0);
    SetDefaultRenderer(new MyGridCellRenderer);

    wxGridSizesInfo gzi = wxGridSizesInfo();
    gzi.m_sizeDefault = 38; SetColSizes(gzi);
    gzi.m_sizeDefault = 20; SetRowSizes(gzi);

    CreateGrid(NNotes,1); // first column is a list of all notes

    Bind(wxEVT_GRID_CELL_LEFT_CLICK, &ChordGrid::PlayCell, this);
    Bind(wxEVT_GRID_LABEL_LEFT_CLICK, &ChordGrid::ColumnLeftClick, this);
}
ChordGrid::~ChordGrid(){ }


void ChordGrid::paintInput(){
    SetColLabelValue(0,input.label);
    for(int r=0; r<NNotes; r++){
        Note rownote = topNote-r;
        if(rownote.chroma()==NOTE_C)
            SetRowLabelValue(r, wxString::Format("%i",rownote.octave()) );
        else
            SetRowLabelValue(r,wxEmptyString);
        SetCellValue(r,0,rownote.toString());

        // determine if note is to be painted green
        bool paint = false;
        for(size_t i=0;i<input.notes.size();i++){
            if(input.notes[i] == rownote)
                paint=true;
        }
        if(paint)
            SetCellBackgroundColour(r,0,selColor);
        else
            SetCellBackgroundColour(r,0,unselColor);
    }
}

void ChordGrid::paintCol(int c){
    Chorde chord = chordList[c-1];
    wxString colLabel = chord.label;
    colLabel.Replace(" ","\n",false);
    SetColLabelValue(c,colLabel);

    for(int k=0; k<12; k++){    // paint the text for all cells sharing the chroma, and reset the background colors
        wxString text = "";
        if(chord.chromaList[k])
            text = Note(k).toString();
        for(int r=(12-k)%12; r<GetNumberRows(); r+=12){
            SetCellValue(r,c,text);
            SetCellBackgroundColour(r,c,unselColor);
        }
    }
    for(int k=0; k<int(chord.notes.size()); k++){ // paint played cells green
        Note n = chord.notes[k];
        int r = topNote - n;
        if(n==chord.root)
            SetCellBackgroundColour(r,c,rootColor);
        else
            SetCellBackgroundColour(r,c,selColor);
    }
}

void ChordGrid::appendChord(Chorde& chord){
    chordList.push_back(chord);
    InsertCols(chordList.size()); // insert instead of append to avoid messing up the extra "end" column in sequence grid
    paintCol(chordList.size());
};

void ChordGrid::removeChord(int c){
    if(c>0 &&  c <= int(chordList.size())){
        chordList.erase(chordList.begin()+(c-1));
        DeleteCols(c);
    }
    if(c==0){
        input.notes.clear();
        paintInput();
    }
};

void ChordGrid::copyChord(int c){
    if(c==0)
        parent->clipboard = input;
    else if(c>0 && c<=int(chordList.size()))
        parent->clipboard = chordList[c-1];
    else{
        parent->clipboard = Chorde();
    }
}

void ChordGrid::ToggleCell(wxGridEvent& evt){
    int r = evt.GetRow();
    int c = evt.GetCol();
    Note rownote = topNote-r;

    wxColor cc = GetCellBackgroundColour(r,c);

    if(cc==selColor || cc==rootColor){
        if(c==0){
            input.removeNote(rownote);
            paintInput();
        }
        if(c>0){
            chordList[c-1].removeNote(rownote);
            paintCol(c);
        }
    }
    else if(cc==unselColor && !GetCellValue(r,c).IsEmpty() ){
        if(c==0){
            input.insertNote(rownote);
            paintInput();
        }
        if(c>0){
            chordList[c-1].insertNote(rownote);
            paintCol(c);
        }
    }
    thisWin->Refresh();
}

void ChordGrid::PlayCell(wxGridEvent& evt){
    int r = evt.GetRow();
    int c = evt.GetCol();
    Note rownote = topNote-r;

    if(!GetCellValue(r,c).IsEmpty())
        parent->fluidPlayNote(rownote);
    else
        parent->fluidEndChord();
}

void ChordGrid::playChord(int c){
    if(c==0)
        parent->fluidPlayChord(input);
    else if(c>0 && c<=int(chordList.size())){
        parent->fluidPlayChord(chordList[c-1]);
    }
    else
        parent->fluidEndChord();
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
    AppendCols(1);
    SetColLabelValue(1,"-");
    paintInput();
    Bind(wxEVT_GRID_CELL_RIGHT_CLICK, &SequenceGrid::ToggleCell, this);
    Bind(wxEVT_GRID_CELL_RIGHT_DCLICK, &SequenceGrid::ToggleCell, this);
    Bind(wxEVT_GRID_LABEL_RIGHT_CLICK, &SequenceGrid::ColumnRightClick, this);
    Bind(wxEVT_GRID_LABEL_RIGHT_DCLICK, &SequenceGrid::ColumnRightClick, this);
}
SequenceGrid::~SequenceGrid(){ }

void SequenceGrid::clearChords(){ // run the first time to make the row labels and cols
    int ncols = GetNumberCols();
    if(ncols>2)
        DeleteCols(1,ncols-2);
    chordList.clear();
}

void SequenceGrid::ColumnRightClick(wxGridEvent& evt){
    size_t c = evt.GetCol();
    int r = evt.GetRow();
    if(r==-1){
    if( evt.ShiftDown() &&  evt.ControlDown()){ // insert chord before
        if(c>0 && c<=chordList.size()){
            chordList.insert(chordList.begin()+(c-1),parent->clipboard);
            InsertCols(c);
            paintCol(c);
        }
        if(c>chordList.size())
            appendChord(parent->clipboard);
    }
    if( evt.ShiftDown() && !evt.ControlDown()){ // paste chord (replace)
        if(c==0){
            input = parent->clipboard;
            input.label = "";
            paintInput();
        }
        if(c>0 && c<=chordList.size()){
            chordList[c-1] = parent->clipboard;
            paintCol(c);
        }
        if(c>chordList.size())
            appendChord(parent->clipboard);
    }
    if(!evt.ShiftDown() && !evt.ControlDown()) // delete chord
        removeChord(c);
    }
}




//
// Filter Grid implementation
//
FilterGrid::FilterGrid(wxGLFrame *parnt, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
            : ChordGrid(parnt,id,pos,size,style,name)
{
    input.label = "?";
    paintInput();
    Bind(wxEVT_GRID_CELL_RIGHT_CLICK, &FilterGrid::ToggleCell, this);
    Bind(wxEVT_GRID_CELL_RIGHT_DCLICK, &FilterGrid::ToggleCell, this);
    Bind(wxEVT_GRID_LABEL_RIGHT_CLICK, &FilterGrid::ColumnRightClick, this);
    Bind(wxEVT_GRID_LABEL_RIGHT_DCLICK, &FilterGrid::ColumnRightClick, this);
}
FilterGrid::~FilterGrid(){ }

void FilterGrid::clearChords(){ // run the first time to make the row labels and cols
    int ncols = GetNumberCols();
    if(ncols>1)
        DeleteCols(1,ncols-1);
    chordList.clear();
}

void FilterGrid::ColumnRightClick(wxGridEvent& evt){
    size_t c = evt.GetCol();
    int r = evt.GetRow();
    if(r==-1){
    if( evt.ShiftDown() && !evt.ControlDown()){ // paste chord (replace)
        if(c==0){ // only meaningful to replace inputs
            input = parent->clipboard;
            input.label = "?";
            paintInput();
            applyFilter();
        }
    }
    if(!evt.ShiftDown() && !evt.ControlDown())
        removeChord(c);
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

    AppendCols(chordList.size());

    // Not calling paintCol(c) iteratively here because re-painting empty cells causes noticeable delay for many chords (single input case)
    // Instead, here is the same function but assuming cells are empty beforehand
    for(size_t c=1; c<=chordList.size(); c++){
        Chorde chord = chordList[c-1];
        wxString colLabel = chord.label;
        colLabel.Replace(" ","\n",false);
        SetColLabelValue(c,colLabel);

        for(int k=0; k<12; k++){    // paint the text for all cells sharing the chroma, and reset the background colors
            if(chord.chromaList[k]){
                wxString text = Note(k).toString();
                for(int r=(12-k)%12; r<GetNumberRows(); r+=12){
                    SetCellValue(r,c,text);
                }
            }
        }
        for(int k=0; k<int(chord.notes.size()); k++){ // paint played cells green
            Note n = chord.notes[k];
            int r = topNote - n;
            if(n==chord.root)
                SetCellBackgroundColour(r,c,rootColor);
            else
                SetCellBackgroundColour(r,c,selColor);
        }
    }

    parent->infoPrint(wxString::Format("There are %i matching chords",int(chordList.size())));
}
