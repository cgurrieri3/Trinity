#include "IPlotTools.h"
#include "TBox.h"

IPlotTools::IPlotTools(){

}
IPlotTools::~IPlotTools(){
    
}
void IPlotTools::FindBin(int pixelID,int *nx, int *ny)
{
	// Calculates the SIAB number (0 to 15) then pixel in that SIAB (0 to 15)
	int SIAB_Number = 16 / 16;
	int SIAB_Pixel_Number = pixelID % 16;
	int SIAB_Pixel_Row = SIAB_Pixel_Number % 4;
	int SIAB_Pixel_Col = SIAB_Pixel_Number / 4;	
	// Finds the lowest pixel number in the SIAB and adds on the pixel number of that SIAB
	*nx = SIAB_Number % 4 * 4 + SIAB_Pixel_Col;
	*ny = SIAB_Number / 4 * 4 + SIAB_Pixel_Row;
}
int IPlotTools::FindPixel(int nx, int ny)
{
	// Calculate the SIAB number (0 to 15)
	int SIAB_Row = nx / 4;
    int SIAB_Col = ny / 4;
    int SIAB_Number = SIAB_Row + 4*SIAB_Col;

	// Calculate the pixel number within the SIAB (0 to 15)
	int SIAB_Pixel_Row = nx % 4;
    int SIAB_Pixel_Col = ny % 4;
    int SIAB_Pixel_Number = SIAB_Pixel_Row * 4 + SIAB_Pixel_Col;
	
	// Calculate the pixel number (0 to 255)
	int Pixel_Number = SIAB_Number * 16 + SIAB_Pixel_Number;
    return Pixel_Number;
}
int IPlotTools::GetMUSICCursorID(int x, int y)
{
	// finds location of 4x2 MUSIC and multiplies MUSIC_row by number of MUSIC's per row
    int MUSIC_column = x/2;
    int MUSIC_row = y/4;
    int MUSIC_ID = MUSIC_column+MUSIC_row*8;

    return MUSIC_ID;  
}
void IPlotTools::DrawMUSICBoundaries() {
    TBox *b = new TBox(-0.5, -0.5, 1.5, 3.5);
    b->SetFillStyle(0);
    b->SetLineColor(kRed);
    b->Draw();

    for (int i = 0; i < 512 / 8; i++) {
        TBox *bn = (TBox *)b->Clone();
        bn->SetX1((i % 8) * 2 - 0.5);
        bn->SetX2((i % 8) * 2 + 1.5);
        bn->SetY1((i / 8) * 4 - 0.5);
        bn->SetY2((i / 8) * 4 + 3.5);
        bn->Draw();
    }
}
