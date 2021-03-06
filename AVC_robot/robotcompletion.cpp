#include "robot.hpp"
#include <algorithm>
#include <functional>
#include <cmath>

// Define RGB structure
struct RGB 
{  
    // R, G, and B values are 0-255
    int r;
    int g;
    int b;
};

// Define HSV structure - Example: https://upload.wikimedia.org/wikipedia/commons/f/f2/HSV_color_solid_cone.png
struct HSV 
{  
    int H;  // Hue, 0 - 360 (what base colour this actually is (sees in the dark!))
    float S;  // Saturation, 0-1 
    float V; // Value, 0-1
};    

// RGB to HSV function I created by adapting the math's algorithm to c++
HSV returnHSV(int r, int g, int b)
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

	// Hue calculation
	if(cMax==rRange)
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
	
	// Saturation calculation
	if(cMax==0.0)
	{
		S=0;
	}
	else
	{
		S=delta/cMax;
	}
	
	// Value calculation
	V=cMax;

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
    const static double TURN_MULTIPLIER = 0.25;
	
    if (initClientRobot() !=0){
	std::cout<<" Error initializing robot"<<std::endl;
    }
    double vLeft = ROBOT_SPEED;
    double vRight = ROBOT_SPEED;

    // Variable to store current frame in
    ImagePPM currentFrame; 

    while(1){
		int totalWhiteXValue = 0;
		int totalWhitePixels = 0;
		double averageWhiteXValue = 0;
	    // Take picture
		takePicture(); 
	    // Save it as view.ppm
		SavePPMFile("view.ppm",cameraView); 
		// Open view.ppm, and save the contents in currentFrame variable
		OpenPPMFile("view.ppm", currentFrame); 
		
		int middleWhitePixels = 0;
	    
	    // Check the entire frame for white pixels
		for(int currentRow = 0; currentRow < currentFrame.height; currentRow++) 
		{ 
			int pixelGrayscale;
			
			for(int currentColumn = 0; currentColumn < currentFrame.width; currentColumn++) 
			{
				pixelGrayscale = get_pixel(currentFrame, currentRow, currentColumn, 3); 
				if(isPixelWhite(pixelGrayscale)==1)
				{
					// Store the average white pixel x value and the total count
					totalWhiteXValue += currentColumn;
					totalWhitePixels++;
				} 
			}
		}
		
		// Calculate average X location of white pixels
		averageWhiteXValue = (double)totalWhiteXValue / (double)totalWhitePixels ;
		
		// Check there are actually pixels, otherwise the robot turns around
		if(totalWhitePixels > 0)
		{
			std::cout<<"Average white "<<averageWhiteXValue;
			// If the pixels are mostly centered, move forward
			if(abs(averageWhiteXValue - currentFrame.width / 2) < 20)
			{
				vLeft = ROBOT_SPEED;
				vRight = ROBOT_SPEED;
			}
			// If the pixels are to the left, turn left
			else if (averageWhiteXValue > currentFrame.width/2)
			{
				vLeft = -((currentFrame.width / 2) - averageWhiteXValue) * TURN_MULTIPLIER;
				vRight =  -(averageWhiteXValue - (currentFrame.width / 2)) * TURN_MULTIPLIER + ROBOT_SPEED/2;
				setMotors(vLeft,vRight);			
			}
			// If the pixels are to the right, turn right
			else
			{
				vLeft = -((currentFrame.width / 2) - averageWhiteXValue) * TURN_MULTIPLIER + ROBOT_SPEED/2;
				vRight =  -(averageWhiteXValue - (currentFrame.width / 2)) * TURN_MULTIPLIER;
				setMotors(vLeft,vRight);					
			}
   
	    }
	    else
	    {
			vLeft = 10;
			vRight = 0;
			setMotors(vLeft,vRight);
	    }
		//std::cout<<" vLeft="<<vLeft<<"  vRight="<<vRight<<std::endl;
		//std::cout<<"Left="<<leftWhitePixels<<" Right="<<rightWhitePixels<<std::endl;
		usleep(12500);
  }

}
