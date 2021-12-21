# CFS-Chord-Filter-Sequence
A simple application for manually identifying chords and constructing chord progressions using the mouse.
I wrote this program for myself as a way to bridge the gap between my basic music theory knowledge and my lack of ear training.

The program has two tables. The right table corresponds to a list of "all" chords containing the notes highlighted in the input column.
Each row corresponds to a note and each column corresponds to a different chord.
The input acts as a filter to narrow down the range of possible chords to look through.
Highlighted cells are are the notes of the chord that will be played if you click the column heading,
while cells with text in them that are not highlighted are also in the chord but will not be played.
The articulation of the chord ("inversion") can be changed from the initial result by selecting or deselecting notes.
The darker shade corresponds to the root of the chord.

The left table corresponds to a sequence of chords that can be added to by copying/pasting chords (e.g. from the right table).
The sequence can then be saved to a text file (\*.chord) and loaded later from the File menu.

Controls for both tables are similar:
- LCLICK:			Play a note or chord
- RCLICK:			Select/deselect a note, or delete a chord
- SHIFT+LCLICK:	Copy a chord
- SHIFT+RCLICK:	Paste a chord (into chord sequence or input column)
- CTRL+SHIFT+RCLICK: Insert a chord into chord sequence

When you play a note or chord, the program uses the currently loaded soundfont to synthesize the sound.
Notes played contiue to play until a new note or chord is played (or, if you "play" an empty cell to stop them).
You can choose whether all notes in a chord are played simultaneously, or are played in an ascending/descending order with a small delay by moving the "Strum delay" slider.
You can load a new soundfont with the "Load Soundfont" button, or choose a different instrument preset with the dropdown menu.
The program comes with a default soundfont ("Timbres of Heaven" by Don Allen, http://midkar.com/soundfonts/ ) but you can find others to use.
You can change the default startup values in the config.ini file (e.g. the soundfont loaded on startup, save path, and more).


An example use case:
- Listen to a song whose chords you want to transcribe
- Left click the input column to play various notes until you find one or more that seem to be in the chord, then highlight them to generate a list of possible chords
- Left click the chord labels (at the top of the table) to play different chords and see if they match what you're looking for
- Once you find the chord you want, shift-click to copy it and then paste it into the sequence table.
- Repeat this process for the next chord, and so on.
- Save your progress to a file to refrence it later.
- Click on chords in succession from the sequence table to hear the progression you've built up.
- Sing along, share the sequence, or reference the sequence as you create an arrangement.

Tips:
- You can copy and paste to/from the input column. Chords pasted from the input column will be labelled as "?".
- Playing chord(s) leading up to the current chord your are trying to identify is helpful.
You can do this by clicking in the sequence table.
- Often the melody is easiest to identify and you may not be certain when a chord changes.
You can paste individual notes from the input column into the chord sequence and then reference them when figuring out the progression.
- The leftmost column in the sequence table is there to provide note labels,
but you can also highlight these cells to act as a reference e.g. for identifying what notes are in the key of the song.
