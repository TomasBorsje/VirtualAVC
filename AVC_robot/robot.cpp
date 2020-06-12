#include "robot.hpp"
#include <algorithm>
#include <functional>
#include <cmath>

struct RGB  // Define RGB structure
{  
    int r; // R, G, and B values are 0-255
    int g;
    int b;
};

struct HSV // Define HSV structure - Example: https://upload.wikimedia.org/wikipedia/commons/f/f2/HSV_color_solid_cone.png
{  
    int H;  // Hue, 0 - 360 (what base colour this actually is (sees in the dark!))
    float S;  // Saturation, 0-1 
    float V; // Value, 0-1
};    

HSV returnHSV(int r, int g, int b) // RGB to HSV function I created by adapting the math's algorithm to c++
{
	double rRange = r/255.0;
	double gRange = g/255.0;
	double bRange = b/255.0;

	double cMax = std::max(std::max(rRange,gRange),bRange);
	double cMin = std::min(std::min(rRange,gRange),bRange);
	double delta = cMax - cMin;

	int H;
	double S;
	double V;

	if(cMax==rRange) // Hue calculation
	{
		H=60*fmod((gRange-bRange)/delta,6);
	}
	else if(cMax==gRange)
	{
		H=60*((bRange-rRange)/delta+2);
	}
	else if(cMax==bRange)
	{
		H=60*((rRange-gRange)/delta+4);
	}
	else
	{
		std::cout<<"Error in finding cMax comparison";
	}
	
	if(cMax==0.0) // Saturation calculation
	{
		S=0;
	}
	else
	{
		S=delta/cMax;
	}

	V=cMax; // Value calculation

	HSV hsvToReturn;
	hsvToReturn.H = H;
	hsvToReturn.S = S;
	hsvToReturn.V = V;

	return hsvToReturn; // Return HSV equivalent!
}

int isPixelWhite(int luminosity)
{
	if(luminosity > 240)
	{
		return 1; 
	}
	else 
	{
		return 0;
	}
}

int main(){
	// Declare constants
	const static int WADDLE_WIDTH = 50; 
	const static int WADDLE_HEIGHT = 40;
	const static int PIXEL_THRESHOLD = 5;
	const static double ROBOT_SPEED = 25.0;
	
	if (initClientRobot() !=0){
		std::cout<<" Error initializing robot"<<std::endl;
	}
    double vLeft = ROBOT_SPEED;
    double vRight = ROBOT_SPEED;
    ImagePPM currentFrame; // Variable to store current frame in

    while(1){
		takePicture(); // Take picture
		SavePPMFile("view.ppm",cameraView); // Save it as view.ppm
		
		OpenPPMFile("view.ppm", currentFrame); // Open view.ppm, and save the contents in currentFrame variable
		
		//std::cout<<"width:"<<currentFrame.width; // Print image stats
		//std::cout<<"height:"<<currentFrame.height;
		int leftWhitePixels = 0;
		int rightWhitePixels = 0;
		for(int currentRow = currentFrame.height - WADDLE_HEIGHT; currentRow < currentFrame.height; currentRow++) // For each of the bottom 40 rows
		{ 
			int pixelGrayscale;
			for(int leftColumn = 0; leftColumn < WADDLE_WIDTH; leftColumn++) // Split into left and right as we are not scanning the middle
			{
				pixelGrayscale = get_pixel(currentFrame, currentRow, leftColumn, 3); // Get grayscale of pixel
				leftWhitePixels += isPixelWhite(pixelGrayscale); // Use checker method to see if it is and add the returned value to white pixels var
			}
			for(int rightColumn = 2*WADDLE_WIDTH; rightColumn < currentFrame.width; rightColumn++) // Right side (pixels 100-150)
			{
				pixelGrayscale = get_pixel(currentFrame, currentRow, rightColumn, 3); // Same thing but for right side
				rightWhitePixels += isPixelWhite(pixelGrayscale);
			}			
		}
		
		if(leftWhitePixels > PIXEL_THRESHOLD && rightWhitePixels > PIXEL_THRESHOLD) // Go forward
		{
			vLeft = ROBOT_SPEED;
			vRight = ROBOT_SPEED;
		}
		else if(leftWhitePixels > PIXEL_THRESHOLD) // Go left
		{
			vLeft = 0.0;
			vRight = ROBOT_SPEED;
		}
		else if(rightWhitePixels > PIXEL_THRESHOLD) // Go right
		{
			vLeft = ROBOT_SPEED;
			vRight = 0.0;			
		}

		setMotors(vLeft,vRight);   
		//std::cout<<" vLeft="<<vLeft<<"  vRight="<<vRight<<std::endl;
		//std::cout<<"Left="<<leftWhitePixels<<" Right="<<rightWhitePixels<<std::endl;
		usleep(10000);
  }

}
