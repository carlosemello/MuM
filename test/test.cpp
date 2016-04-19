// tutorial1.cpp
#include "MuMaterial.h"
int main(void)
{
    MuMaterial mat, transp, seq;
    MuNote note;
    int i;
    short pattern[8] = {60, 64, 62, 65, 64, 67, 65, 69};
    note.SetInstr(1);
    note.SetDur(0.5);
    note.SetAmp(0.7);
    
    for( i = 0; i < 8; i++)
    {
        note.SetStart(i * 0.5);
        note.SetPitch(pattern[i]);
        mat.AddNote(note);
    }
    
    for( i = 1; i < 8; i++ )
    {
        transp = mat;
        transp.DiatonicTranspose(0,MAJOR_MODE,i,ASCENDING);
        seq.Append(0, transp, 0);
    }
    
    note.SetDur(1.0);
    note.SetPitch(72);
    transp.Clear();
    transp.AddNote(note);
    seq = seq + transp;
    
    seq.SetDefaultFunctionTables();
    seq.Score("./test/outFile");
    seq.Orchestra("./test/outFile");
    
    return 0;
}
