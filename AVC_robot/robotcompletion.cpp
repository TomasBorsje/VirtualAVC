#include <vector>
#include "robot.hpp"

std::vector<int> getLeftVec() {
	ImagePPM frame;
	std::vector<int> returnvec;
	OpenPPMFile("completionimage.ppm", frame);
	for (int i = 0; i < frame.height; i++) {
		returnvec.push_back(frame.data[1 + i * frame.width]);
	}
	returnvec = simplifyframe(returnvec);
	return returnvec;
}

std::vector<int> getRightVec() {
	ImagePPM frame;
	std::vector<int> returnvec;
	OpenPPMFile("completionimage.ppm", frame);
	for (int i = 0; i < frame.height; i++) {
		returnvec.push_back(frame.data[(1 + i) * frame.width]);
	}
	returnvec = simplifyframe(returnvec);
	return returnvec;
}

std::vector<int> getHorizVec() {
	ImagePPM frame;
	std::vector<int> returnvec;
	OpenPPMFile("completionimage.ppm", frame);
	for (int i = 0; i < frame.width; i++) {
		returnvec.push_back(frame.data[i + ((int)frame.height / 2 * frame.width)]);
	}
	returnvec = simplifyframe(returnvec);
	return returnvec;
}

std::vector<int> simplifyframe(std::vector<int> current) {
	for (int i = 0; i < current.size(); i++) {
		if (current[i] > 240) {
			current[i] = 1;
		}
		else {
			current[i] = 0;
		}
	}
	return current;
}
bool whiteLineCheck(std::vector<int> current) {
	int fieldsmid = (int)current.size() / 2;//location of the middle field in the vector
	if (current[fieldsmid] == 1) {
		return true;
	}
	return false;
}
void turnLeft() {
	vLeft = -5.0;
	vRight = 5.0;
	usleep(100);//this is to make sure the robot actually starts turning before the loop starts running otherwise the loop will register the wrong white line
	std::vector<int> myvec;
	while (whiteLineCheck(myvec) == false) {
		takePicture();
		SavePPMFile("completionimage.ppm", cameraView);
		myvec = getHorizVec();
	}
	vLeft = 0.0;
	vRight = 0.0;
}
void turnRight() {
	vLeft = 5.0;
	vRight = -5.0;
	usleep(100);//this is to make sure the robot actually starts turning before the loop starts running otherwise the loop will register the wrong white line. needs to be long enough for the robot to rotate 3 pixels
	std::vector<int> myvec;
	while (whiteLineCheck(myvec) == false) {
		takePicture();
		SavePPMFile("completionimage.ppm", cameraView);
		myvec = getHorizVec();
	}
	vLeft = 0.0;
	vRight = 0.0;
}
void moveFoward() {
	vLeft = 5.0;
	vRight = 5.0;
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
	vLeft = 0.0;
	vRight = 0.0;
}
int main()
{
	if (initClientRobot() != 0) {
		std::cout << " Error initializing robot" << std::endl;
	}
	std::vector<int> leftvec;//the three arrays that will contain lines from the image
	std::vector<int> rightvec;
	std::vector<int> horizvec;
	while (1) {
		takePicture();
		SavePPMFile("completionimage.ppm", cameraView);
		leftvec = getLeftVec();
		rightvec = getRightVec();
		horizvec = getHorizVec();
		if (whiteLineCheck(leftvec) == true) {//the next if statements are for movement directions according to a heirachy to explore every path of the maze
			turnLeft();
		}
		else if (whiteLineCheck(horizvec) == true) {
			moveFoward();
		}
		else if (whiteLineCheck(rightvec) == true) {
			turnRight();
		}
		else {
			turnLeft();
			turnLeft();
		}
	}
}



