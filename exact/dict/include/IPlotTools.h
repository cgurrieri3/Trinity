#include<iostream>


class IPlotTools{
	/**
	 * \class IPlotTools
	 * \brief Methods to facilitate plotting the camera in ExACT
	 * 
	 * */
	public:
		/**
		 * Public constructor
		 * */
		IPlotTools();
		/**
		 * Public destructor
		 * */
		~IPlotTools();
		/**
		 * A method to find the bin to which a pixel ID belongs to, based on a camera that is 16 rows by 32 columns and follows the EUSO-SPB2 numbering standard
		 * 
		 * @param pixelID An int with the ID of the pixel
		 * @param nx An int* where to store the column bin number
		 * @param ny An int* where to store the row bin number
		 * */
		static void FindBin(int pixelID, int *nx, int *ny);
		/**
		 * A method to calculate the pixel ID corresponding to a certain row and column in the camera
		 * 
		 * @param nx An int with the column number
		 * @param ny An int with the row number
		 * 
		 * @return An int with the pixel number
		 * */
		static int FindPixel(int nx, int ny);
		/**
		 * A method used to calculate the MUSIC ID (trigger cell) over which the mouse is hovering on a ROOT plot of the camera
		 * 
		 * @param x An int with the x coordinate of the cursor
		 * @param y An int with the y coordinate of the cursor
		 * 
		 * @return An int witht the MUSIC ID.
		 * */
		static int GetMUSICCursorID(int x, int y);
		/**
		 * A method used to draw the trigger cell boundaries grouping the pixels connected to the same discriminator together
		 * 
		 * */
		static void DrawMUSICBoundaries();

};
