
#ifndef NOTES_H
#define NOTES_H

#include <wx/string.h>

enum noteNum{
    NOTE_C = 0,
    NOTE_Db,
    NOTE_D,
    NOTE_Eb,
    NOTE_E,
    NOTE_F,
    NOTE_Gb,
    NOTE_G,
    NOTE_Ab,
    NOTE_A,
    NOTE_Bb,
    NOTE_B
};

struct Note{

    Note(int inVal=0, int inVel=100){val = inVal; vel=inVel;}
    Note(wxString str, int inVel=100){
        vel = inVel;
        val = toInt(str);
    }

    int vel = 100;
    int val = 0;


    wxString toString(bool flat=false, bool addOct=false){
        wxString str;
        int oct = val/12 - 1;
        switch (val%12){ // if flat, use flat representation instead of sharp representation
        case 0 :
            str =       "C "; break;
        case 1 :
            str = (flat?"Db":"C#"); break;
        case 2 :
            str =       "D ";       break;
        case 3 :
            str = (flat?"Eb":"D#"); break;
        case 4 :
            str =       "E ";       break;
        case 5 :
            str =       "F ";       break;
        case 6 :
            str = (flat?"Gb":"F#"); break;
        case 7 :
            str =       "G ";       break;
        case 8 :
            str = (flat?"Ab":"G#"); break;
        case 9 :
            str =       "A ";       break;
        case 10:
            str = (flat?"Bb":"A#"); break;
        case 11:
            str =       "B ";       break;
        }
        if(addOct)
            str << oct;
        return str;
    }
    int toInt(wxString str){
        long int oct = 3;
        int chroma = 0;
        int accidental = 0;
        str.Right(1).ToLong(&oct);
        str = str.Left(2);
        if(str.Right(1)=="#")
            accidental =  1;
        if(str.Right(1)=="b")
            accidental = -1;
        if(str.Left(1)=="C")
            chroma =  0;
        if(str.Left(1)=="D")
            chroma =  2;
        if(str.Left(1)=="E")
            chroma =  4;
        if(str.Left(1)=="F")
            chroma =  5;
        if(str.Left(1)=="G")
            chroma =  7;
        if(str.Left(1)=="A")
            chroma =  9;
        if(str.Left(1)=="B")
            chroma = 11;
        return chroma+accidental + 12*(oct+1);
    }


};

enum ChordType{
    CHORD_M = 0,
    CHORD_m,
    CHORD_dim,
    CHORD_aug,
    CHORD_sus,
    CHORD_sus2,
    CHORD_sus3,
    CHORD_6,
    CHORD_69,
    CHORD_M7,
    CHORD_M75,
    CHORD_M9,
    CHORD_add9,
    CHORD_7,
    CHORD_7sus4,
    CHORD_75,
    CHORD_7b5,
    CHORD_7s5,
    CHORD_9,
    CHORD_b9,
    CHORD_s9,
    CHORD_13,
    CHORD_m6,
    CHORD_m65,
    CHORD_m7,
    CHORD_m75,
    CHORD_m7b5,
    CHORD_m7s5,
    CHORD_m9,
    CHORD_mad9,
    CHORD_mb9,
    CHORD_mM7,
    CHORD_dim7,
    CHORD_dim5,
    CHORD_5,
    CHORD_3,
    CHORD_aug9,
    CHORD_UNK
};
static const wxString chordNames[] = {
    [CHORD_M] = "M",
    [CHORD_m] = "m",
    [CHORD_dim] = "dim",
    [CHORD_aug] = "+",
    [CHORD_sus] = "sus",
    [CHORD_sus2] = "sus2",
    [CHORD_sus3] = "sus3",
    [CHORD_6] = "6",
    [CHORD_69] = "6/9",
    [CHORD_M7] = "M7",
    [CHORD_M75] = "M75",
    [CHORD_M9] = "M9",
    [CHORD_add9] = "add9",
    [CHORD_7] = "7",
    [CHORD_7sus4] = "7sus4",
    [CHORD_75] = "75",
    [CHORD_7b5] = "7b5",
    [CHORD_7s5] = "7#5",
    [CHORD_9] = "9",
    [CHORD_b9] = "b9",
    [CHORD_s9] = "#9",
    [CHORD_13] = "13",
    [CHORD_m6] = "m6",
    [CHORD_m65] = "m65",
    [CHORD_m7] = "m7",
    [CHORD_m75] = "m75",
    [CHORD_m7b5] = "m7b5",
    [CHORD_m7s5] = "m7s5",
    [CHORD_m9] = "m9",
    [CHORD_mad9] = "mad9",
    [CHORD_mb9] = "mb9",
    [CHORD_mM7] = "mM7",
    [CHORD_dim7] = "dim7",
    [CHORD_dim5] = "dim5",
    [CHORD_5] = "5",
    [CHORD_3] = "3",
    [CHORD_aug9] = "+9",
    [CHORD_UNK] = ""
};

static const int chordSteps[][5] = {
    [CHORD_M]       = { 0, 4, 7, 0, 0},
    [CHORD_m]       = { 0, 3, 7, 0, 0},
    [CHORD_dim]     = { 0, 3, 6, 0, 0},
    [CHORD_aug]     = { 0, 4, 8, 0, 0},
    [CHORD_sus]     = { 0, 5, 7, 0, 0},
    [CHORD_sus2]    = { 0, 2, 7, 0, 0},
    [CHORD_sus3]    = { 0, 5, 7,14, 0},
    [CHORD_6]       = { 0, 4, 0, 9, 0},
    [CHORD_69]      = { 0, 4, 0, 9,14},
    [CHORD_M7]      = { 0, 4, 0,11, 0},
    [CHORD_M75]     = { 0, 4, 7,11, 0},
    [CHORD_M9]      = { 0, 4, 7,11,14},
    [CHORD_add9]    = { 0, 4, 7, 0,14},
    [CHORD_7]       = { 0, 4, 0,10, 0},
    [CHORD_7sus4]   = { 0, 5, 7, 9, 0},
    [CHORD_75]      = { 0, 4, 7,10, 0},
    [CHORD_7b5]     = { 0, 4, 6,10, 0},
    [CHORD_7s5]     = { 0, 4, 8,10, 0},
    [CHORD_9]       = { 0, 4, 0,10,14},
    [CHORD_b9]      = { 0, 4, 0,10,13},
    [CHORD_s9]      = { 0, 4, 0,10,15},
    [CHORD_13]      = { 0, 4, 9,10, 0},
    [CHORD_m6]      = { 0, 3, 0, 9, 0},
    [CHORD_m65]     = { 0, 3, 7, 9, 0},
    [CHORD_m7]      = { 0, 3, 0,10, 0},
    [CHORD_m75]     = { 0, 3, 7,10, 0},
    [CHORD_m7b5]    = { 0, 3, 6,10, 0},
    [CHORD_m7s5]    = { 0, 3, 8,10, 0},
    [CHORD_m9]      = { 0, 3, 0,10,14},
    [CHORD_mad9]    = { 0, 3, 7, 0,14},
    [CHORD_mb9]     = { 0, 3, 0,10,13},
    [CHORD_mM7]     = { 0, 3, 7,11, 0},
    [CHORD_dim7]    = { 0, 3, 6, 9, 0},
    [CHORD_dim5]    = { 0, 0, 6, 0, 0},
    [CHORD_5]       = { 0, 0, 7, 0, 0},
    [CHORD_3]       = { 0, 4, 0, 0, 0},
    [CHORD_aug9]    = { 0, 4, 8,15, 0},
    [CHORD_UNK]     = { 0, 0, 0, 0, 0}
};

struct AbsList{
    bool l[12];
};

typedef std::vector<Note> Notes;

struct Chorde{
    Notes notes;    // the notes to be played when articulating this chord
    Note root;      // note stored only for intervals and labeling purposes; may or may not be articulated
    ChordType ct;   // defines what notes are possible for articulation
    wxString label; // user-defined label for the chord. Defaults to chordNames[ct],
    void defaultLabel(){
        label = root.toString() + " " + chordNames[ct];
    }
    void insertNote(Note in){
        bool inserted=false;
        for(Notes::const_iterator it=notes.begin(); it!=notes.end(); it++){
            if(in.val == it->val){ // no duplicates
                inserted=true;
                break;
            }
            if(in.val < it->val){
                notes.insert(it,in);
                inserted=true;
                break;
            }
        }
        if(!inserted)
            notes.push_back(in);
    }
    void removeNote(Note in){
        for(Notes::const_iterator it=notes.begin(); it!=notes.end(); it++){
            if(in.val == it->val){
                notes.erase(it);
                break;
            }
        }
    }
};


/*
const int priority[] = {
    1,
    1,
    0,
    0,
    0,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    ,
    ,
    ,
    ,
    ,
    ,
    ,
    ,
    ,
    ,
    ,
    ,
    ,
    ,
    ,
    ,
    ,
    ,
    ,
    ,
    ,
};*/
#endif
