
#ifndef CHORDGRID_H
#define CHORDGRID_H

#include "Notes.h"
#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/generic/grid.h>

class wxGLFrame;

class ChordGrid: public wxGrid
{
public:

    ChordGrid(wxGLFrame *parnt, wxWindowID id, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxWANTS_CHARS, const wxString &name=wxGridNameStr);
    ~ChordGrid();

    const std::vector<Chorde>& getChordList(){return this->chordList;};
    void ToggleCell(wxGridEvent& evt);
    void PlayCell(wxGridEvent& evt);
    void playChord(int c);
    void ColumnLeftClick(wxGridEvent& evt);
    void appendChord(Chorde& chord);

protected:
    wxGLFrame* parent;
    Chorde input;
    std::vector<Chorde> chordList;

    Note topNote = wxString("C 8");
    Note botNote = wxString("C 0");
    int NNotes = topNote-botNote; // Number of notes (rows) in the grid.
    wxColor selColor = wxColor(34,177,76);
    wxColor rootColor = wxColor(20,107,47);
    wxColor unselColor = wxColor(255,255,255);

    void paintInput();
    void paintCol(int c);
    void removeChord(int c);
    void copyChord(int c);
};
class MyGridCellRenderer : public wxGridCellStringRenderer
{
public:
    virtual void Draw(wxGrid& grid,wxGridCellAttr& attr,wxDC& dc,const wxRect& rect,int row, int col,bool isSelected){
        wxGridCellStringRenderer::Draw(grid, attr, dc, rect, row, col, false);
    }
};



class SequenceGrid: public ChordGrid
{
public:
    SequenceGrid(wxGLFrame *parnt, wxWindowID id, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxWANTS_CHARS, const wxString &name=wxGridNameStr);
    ~SequenceGrid();

    void ColumnRightClick(wxGridEvent& evt);
    void clearChords();
};



class FilterGrid: public ChordGrid
{
public:
    FilterGrid(wxGLFrame *parnt, wxWindowID id, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxWANTS_CHARS, const wxString &name=wxGridNameStr);
    ~FilterGrid();

    void ColumnRightClick(wxGridEvent& evt);
    void ToggleCell(wxGridEvent& evt);
    void clearChords();
    void applyFilter();
};

#endif