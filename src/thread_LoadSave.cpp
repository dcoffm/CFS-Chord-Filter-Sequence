
#include "wxGLMain.h"

void wxGLFrame::thread_Save(){
    wxFileDialog d(this,"Save a chord sequence","","","Chord file(*.chord)|*chord",wxFD_SAVE);
    if(d.ShowModal() == wxID_OK){
        wxString path = d.GetPath();
        wxLogDebug("%s",path);
        d.GetPath().EndsWith(".chord",&path);
        wxLogDebug("%s",path);
        wxFile file(path+".chord", wxFile::write);
        if(!file.IsOpened()){
            wxMessageBox("Save failed.");
            sendUpdate(-1,"");
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
            sendUpdate(-1,"Save successful.");
        }
    }
    sendUpdate(-1,"");
}

void wxGLFrame::thread_Load(){
    wxFileDialog d(this,"Select a chord file","chords","","Chord file(*.chord)|*chord",wxFD_OPEN);
    if(d.ShowModal() == wxID_OK){
        wxFile file(d.GetPath(), wxFile::read);
        if(!file.IsOpened()){
            wxMessageBox("Failed to open file.");
            sendUpdate(-1,"");
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
                //if(line.length()>2){ // very crude check to see if this line actually has a chord in it
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
                //}
            }
            file.Close();
        }
    }else{ // Cancel button
        sendUpdate(-1,"");
        return;
    }
    sendUpdate(-1,"Load successful");
}
