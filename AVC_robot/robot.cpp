#include "robot.hpp"
#include <algorithm>
#include <functional>

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
		cout<<"Error in finding cMax comparison";
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

int main(){
	if (initClientRobot() !=0){
		std::cout<<" Error initializing robot"<<std::endl;
	}
    double vLeft = 40.0;
    double vRight = 30.0;
    takePicture();
    SavePPMFile("i0.ppm",cameraView);
    while(1){
      setMotors(vLeft,vRight);   
      std::cout<<" vLeft="<<vLeft<<"  vRight="<<vRight<<std::endl;
       usleep(10000);
  } //while

} // main
