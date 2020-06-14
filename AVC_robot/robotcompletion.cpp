#include <vector>

#include "robot.hpp"



std::vector<int> getLeftVec() {//fills the left vector with 0's and 1's. 1 meaning that pixel had luminosity <240 0 meaning less than

	ImagePPM frame;
	std::vector<int> returnvec;
	OpenPPMFile("completionimage.ppm", frame);

	for (int i = 0; i < frame.height; i++) {
		if (get_pixel(frame,i,frame.width,3)> 240){
			returnvec.push_back(1);
		}
		else{
			returnvec.push_back(0);
		}
	}

	return returnvec;

}



std::vector<int> getRightVec() {//fills the right vector with 0's and 1's. 1 meaning that pixel had luminosity <240 0 meaning less than

	ImagePPM frame;

	std::vector<int> returnvec;

	OpenPPMFile("completionimage.ppm", frame);

	for (int i = 0; i < frame.height; i++) {
		if (get_pixel(frame,i,0,3)> 240){
			returnvec.push_back(1);
		}
		else{
			returnvec.push_back(0);
		}
	}

	return returnvec;

}



std::vector<int> getHorizVec() {//fills the horizontal vector with 0's and 1's. 1 meaning that pixel had luminosity <240 0 meaning less than
	ImagePPM frame;
	std::vector<int> returnvec;
	OpenPPMFile("completionimage.ppm", frame);

	for (int i = 0; i < frame.width; i++) {
		if (get_pixel(frame,frame.height/2,i,3)> 240){
			returnvec.push_back(1);
		}
		else{
			returnvec.push_back(0);
		}
	}
	return returnvec;
}

bool whiteLineCheck(std::vector<int> current) {//checks if the middle fields of an array contain 1's

	int fieldsmid = (int)current.size() / 2;//location of the middle field in the vector

	if (current[fieldsmid] == 1) {

		return true;

	}

	return false;

}

void turnLeft() {//makes the robot turn to the left until it detects a line in the middle of a frame (90 degree turn)

	setMotors(-5.0,5.0);

	usleep(100);//this is to make sure the robot actually starts turning before the loop starts running otherwise the loop will register the wrong white line

	std::vector<int> myvec;

	while (whiteLineCheck(myvec) == false) {

		takePicture();

		SavePPMFile("completionimage.ppm", cameraView);

		myvec = getHorizVec();

	}
	setMotors(0.0,0.0);

}

void turnRight() {//makes the robot turn to the right until it detects a line in the middle of a frame (90 degree turn)
	
	setMotors(5.0,-5.0);

	usleep(100);//this is to make sure the robot actually starts turning before the loop starts running otherwise the loop will register the wrong white line. needs to be long enough for the robot to rotate 3 pixels

	std::vector<int> myvec;

	while (whiteLineCheck(myvec) == false) {

		takePicture();

		SavePPMFile("completionimage.ppm", cameraView);

		myvec = getHorizVec();

	}
	setMotors(0.0,0.0);
}

void move75Px(){//self explanatory 
	setMotors(5.0,5.0);
	usleep(100);//needs to be tested to get the correct distance
	setMotors(0.0,0.0);
}

void moveFoward() {//makes the robot move foward until it detects a corner of a dead end

	setMotors(5.0,5.0);

	std::vector<int> leftvec;

	std::vector<int> rightvec;

	std::vector<int> horizvec;

	while (whiteLineCheck(horizvec) == true && whiteLineCheck(leftvec) == false && whiteLineCheck(rightvec) == false) {

		takePicture();

		SavePPMFile("completionimage.ppm", cameraView);

		horizvec = getHorizVec();

		rightvec = getRightVec();

		leftvec = getLeftVec();

	}

	setMotors(0.0,0.0);

}

int main()

{

	if (initClientRobot() != 0) {

		std::cout << " Error initializing robot" << std::endl;

	}
	//the three arrays that will contain lines from the image
	std::vector<int> leftvec;//will contain data from the first column of the image

	std::vector<int> rightvec;//will contain data from the last column of the image

	std::vector<int> horizvec;//will contain data from the middle row of the image

	while (1) {

		takePicture();

		SavePPMFile("completionimage.ppm", cameraView);

		leftvec = getLeftVec();

		rightvec = getRightVec();

		horizvec = getHorizVec();

		if (whiteLineCheck(leftvec) == true) {//the next if statements are for movement directions according to a heirachy to explore every path of the maze
			move75Px();
			turnLeft();

		}

		else if (whiteLineCheck(horizvec) == true) {

			moveFoward();

		}

		else if (whiteLineCheck(rightvec) == true) {
			move75Px();
			turnRight();

		}

		else {
			move75Px();
			turnLeft();//as this method stops turning when it sees a white line it will do 180 degree turns as well as left turns

		}

	}

}

