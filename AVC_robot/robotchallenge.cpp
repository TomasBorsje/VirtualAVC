#include "robot.hpp"
#include <algorithm>
#include <functional>
#include <cmath>

void checkInitialize(){
	if (initClientRobot() !=0){
	std::cout<<" Error initializing robot"<<std::endl;
    }
}

int getNumPix(int coords[]){//looks through the parts of cameraView mentioned in the coords array. then increments the return value everytime the pixel has lots of red
	int COLOR_THRESHOLD = 240;
	int returnint;
	for(int i = coords[0];i<coords[1];i++){
		for(int j = coords[2];j<coords[3];j++){
			if(get_pixel(cameraView,j,i,0)>COLOR_THRESHOLD){
				returnint++;
			}
		}
	}
	return returnint;
}
int getAvgX(){//gets avg x value for group of pixels in the array
	int COLOR_THRESHOLD = 240;
	int totalx = 0;
	int counter = 0;
	int coords[] = {/*startcol*/0,/*endcol*/149,/*startrow*/0,/*endrow*/29}; //by default the first 30 rows
	for(int i = coords[0];i<coords[1];i++){
		for(int j = coords[2];j<coords[3];j++){
			if(get_pixel(cameraView,j,i,0)>COLOR_THRESHOLD){
				counter++;
				totalx+=i;
			}
		}
	}
	totalx = totalx/counter; //totalx is now the average
	return totalx;
}

double getTurningRatio(int numleftpix,int numrightpix){
	//negative ratio means turn right
	//positive ratio means turn left
	//ratio of zero means no pixels found
	//ratio of one means go in a straight line
	//the closer the ratio is to 0 (positive or negative) the sharper you should be turning
	int top = numleftpix-numrightpix;
	int bottom = numleftpix+numrightpix ;
	top = top/bottom;
	return top;
}

void decideMovement(int numleftpix,int numrightpix,int numhorizpix,int avgx){
	double turningratio = getTurningRatio(numleftpix,numrightpix);
}

int main(){
checkInitialize();
setMotors(20.0,20.0);
usleep(10000000);//leaves the robot running in a straight line for long enough to properly get inside the maze
setMotors(0.0,0.0);
//image 150 columns and 100 rows
//what parts of the image the arrays will be using, allows easy changing
int leftcoords[] = {/*startcol*/0,/*endcol*/29,/*startrow*/0,/*endrow*/99}; //by default the first 30 columns
int rightcoords[] = {/*startcol*/120,/*endcol*/149,/*startrow*/0,/*endrow*/99}; //by default the last 30 columns
int horizcoords[] = {/*startcol*/30,/*endcol*/119,/*startrow*/0,/*endrow*/29}; //the first 30 rows but not anything that is within the left and right parts (no overlap)
int numleftpix = 0;
int numrightpix = 0;
int numhorizpix = 0;
int avgxcoord = 0;
	while(1){
		takePicture();//no need to save as currentFrame. cameraView can be used directly
		numleftpix = getNumPix(leftcoords);
		numrightpix = getNumPix(rightcoords);
		numhorizpix = getNumPix(horizcoords);
		avgxcoord = getAvgX();
		decideMovement(numleftpix,numrightpix,numhorizpix,avgxcoord);
	}









}

