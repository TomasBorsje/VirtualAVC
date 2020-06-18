#include "robot.hpp"
#include <algorithm>
#include <functional>
#include <cmath>
#include <iostream>       // std::cout, std::endl
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <unistd.h>


struct RGB  // Define RGB structure
{  
    int r; // R, G, and B values are 0-255
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

RGB getPixelRGB(ImagePPM currentFrame, int row, int column)
{
	RGB RGBToReturn;
	RGBToReturn.r = get_pixel(currentFrame, row, column, 0); 
	RGBToReturn.g = get_pixel(currentFrame, row, column, 1); 
	RGBToReturn.b = get_pixel(currentFrame, row, column, 2); 
	
	return RGBToReturn;
}

bool isPixelRed(HSV pixel)
{
	if(pixel.H > 340 || pixel.H < 20) // Pixel's hue is red
	{
		if(pixel.S > 70) // Pixel is saturated enough
		{
			if(pixel.V > 75) // Pixel is bright enough
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	
	else
	{
		return false;
	}
	
	
}

bool isLeftSideRed(ImagePPM currentFrame)
{
	for(int i = 0; i < 40; i++) // Width
	{
		for(int row = 0; row < currentFrame.height; row++) // Height
		{
			RGB pixel = getPixelRGB(currentFrame, row, i);
			if(isPixelRed(returnHSV(pixel.r,pixel.g,pixel.b)))
			{
				return true;
			}
		}
	}
	return false;
}

bool isRightSideRed(ImagePPM currentFrame)
{
	for(int i = currentFrame.width - 40; i < currentFrame.width; i++) // Width
	{
		for(int row = 0; row < currentFrame.height; row++) // Height
		{
			RGB pixel = getPixelRGB(currentFrame, row, i);
			if(isPixelRed(returnHSV(pixel.r,pixel.g,pixel.b)))
			{
				return true;
			}
		}
	}
	return false;
}

bool isFrontSideRed(ImagePPM currentFrame)
{
	for(int i = 0; i < currentFrame.width; i++) // Width
	{
		for(int row = 0; row < 40; row++) // Height
		{
			RGB pixel = getPixelRGB(currentFrame, row, i);
			if(isPixelRed(returnHSV(pixel.r,pixel.g,pixel.b)))
			{
				return true;
			}
		}
	}
	return false;
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
    bool turningLeft;
    int leftRedPixels;
    int rightRedPixels;
    int middleRedPixels;

    // Variable to store current frame in
    ImagePPM currentFrame; 

    while(1){
		
	    // Take picture
		takePicture(); 
	    // Save it as view.ppm
		SavePPMFile("view.ppm",cameraView); 
		// Open view.ppm, and save the contents in currentFrame variable
		OpenPPMFile("view.ppm", currentFrame); 
		
		leftRedPixels = 0;
		rightRedPixels = 0;
		middleRedPixels = 0;
		
		int totalWhiteXValue = 0;
		int totalWhitePixels = 0;
		double averageWhiteXValue = 0;		

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
		
		// Check there are actually pixels, otherwise the robot does not do anything relating to the white line
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
	    else // There is no white line, we do this instead of white line logic
	    {
	    
			// Robot Algorithm:
			// If there is something infront, turn around
			// Then, if there is a wall to the left:
			// 		If there is a wall infront, turn right
			//		If there isn't, move forward
			// Then, if there is no wall to the left,
			// Begin turning left
			
			
			// Now we must check if there is red right infront of the robot
			// If there is, we REVERSE before doing anything else
			
			RGB center1 = getPixelRGB(currentFrame, 99, 70); // 70 and 80 are 5 pixels away from the center width of the image
			RGB center2 = getPixelRGB(currentFrame, 99, 80);
			
			if(isPixelRed(returnHSV(center1.r,center1.g,center1.b)) || isPixelRed(returnHSV(center2.r,center2.g,center2.b)))
			{
				setMotors(-ROBOT_SPEED, -ROBOT_SPEED); // Either one of the center pixels was red, there's a barricade RIGHT infront of us
			}
			else if(isLeftSideRed(currentFrame))
			{
				// Left side IS red
				if(isFrontSideRed(currentFrame)) // Front side red, barricade infront of us so we turn right
				{
					setMotors(ROBOT_SPEED, -ROBOT_SPEED); // Turn right on the spot
				}
				else
				{
					setMotors(ROBOT_SPEED, ROBOT_SPEED); // Continue forward
				}
			}
			else // There is no wall to the left of us
			{
				setMotors(-ROBOT_SPEED, ROBOT_SPEED); // Turn left on the spot
			}
		}

		//std::cout<<" vLeft="<<vLeft<<"  vRight="<<vRight<<std::endl;
		//std::cout<<"Left="<<leftWhitePixels<<" Right="<<rightWhitePixels<<std::endl;
		usleep(7500);
	}

}

