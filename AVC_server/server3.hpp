#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <vector>
#include <iostream>
#include <sstream>
#include <math.h>
#include <fstream>

#define DEBUG_OUT 0

// function prototypes
bool checkCollisions();

/***********ROBOT DATA***************/
struct Robot{
	sf::Vector2f pos;  //position
	double azimuth;    // robot angle
	double vLeft,vRight; //speed of left and right motor
	double wheelRadius;
	double width;   // distance between wheels
	double length;
	double cameraForward;  // camera tilt
	int fovWidth;     //size of camera FieldOfView
	int fovLength;
	unsigned char *image; // current camera image
	int nImage;   // size(bytes) of camera image
	
	sf::Image bkImage;
	sf::RectangleShape outline;
	sf::RectangleShape cameraFOV;
} robot;

void initRobot(){
    robot.pos.x = 100.0; robot.pos.y = 100.0; 
    robot.azimuth = 0.0;
    robot.vLeft = 10.0;
    robot.vRight = 10.0;
    robot.wheelRadius = 8.0;
    robot.width = 30.0;
    robot.length = 50.0;
    robot.cameraForward = 100.0;
	robot.fovWidth = 150.0;
	robot.fovLength = 100.0;
	robot.nImage = (int)(robot.fovLength*robot.fovWidth*3);
	robot.image = new unsigned char[robot.nImage];
	
	robot.outline.setOrigin(sf::Vector2f(robot.length/2.0, robot.width/2.0));
    robot.outline.setSize(sf::Vector2f(robot.length+2, robot.width+2));
    robot.outline.setFillColor(sf::Color::Transparent);
    robot.outline.setOutlineColor(sf::Color(255,255,255));
    robot.outline.setOutlineThickness(1.0f);
    
    robot.cameraFOV.setSize(sf::Vector2f(robot.fovLength, robot.fovWidth));
    robot.cameraFOV.setFillColor(sf::Color::Transparent);
    robot.cameraFOV.setOutlineColor(sf::Color(0,255,255));
    robot.cameraFOV.setOutlineThickness(2.0f);
    
    //sf::Image img;
    robot.bkImage.create(robot.length, robot.width);
    for (int i = 0; i < robot.length; ++i) {
     for (int j = 0; j < robot.width; ++j) {
		 if ( i<robot.length/2){
            robot.bkImage.setPixel(i, j, sf::Color( 255,0,0));
		 } else {
            robot.bkImage.setPixel(i, j, sf::Color(0,0,255));
		}
      }
    }

}


void moveRobot(){
	
	if (checkCollisions()==1){
		return;
	}
	
	float l_left;   // left wheel movement
    float l_right;  // right wheel movement
    float d_l_r;
    float s_l_r;
    float r_turn;    // radius of turn
    float df_turn;   // degree of turn
    float icc_x;
    float icc_y;
    if (DEBUG_OUT){
       std::cout<<" inside moveRobot()"<<std::endl;
       std::cout<<"     vLeft = "<< robot.vLeft<<" vRright = "<<robot.vRight<<std::endl;
       std::cout<<" before: x = "<< robot.pos.x<<" y = "<<robot.pos.y;
       std::cout<<" azimuth="<<robot.azimuth<<std::endl;
    }
    //println("  before move:  pos_x="+pos_x+" pos_y="+pos_y + " pos_teta="+pos_teta);
    // calculate wheel tip movements- lenght of the arc: 
    l_left =  robot.wheelRadius * robot.vLeft * M_PI/180.0; 
    l_right = robot.wheelRadius * robot.vRight * M_PI/180.0;
    //std::cout<<" l_left = "<< l_left<<" l_right = "<<l_right<<std::endl;
    // differences in wheel movements
    d_l_r = l_left - l_right;
    s_l_r = l_left + l_right;
    //println(" d_l_r = "+ d_l_r + " s_l_r = "+ s_l_r);
    
    if ( abs(d_l_r) <= 0.001 )
    {
      // move in straight line
      r_turn = 1000.0;
      //std::cout<<"  moving straight"<<std::endl;
      robot.pos.x = robot.pos.x + l_left*cos(robot.azimuth*M_PI/180.0);
      robot.pos.y = robot.pos.y + l_right*sin(robot.azimuth*M_PI/180.0);
    } else 
    {
       // robot is rotating
       //std::cout<<" robot turning"<<std::endl;
       r_turn = (robot.width*0.5)*s_l_r/d_l_r;
       df_turn = d_l_r / robot.width;         //radians
      // println("  r_turn ="+r_turn + " df_turn="+df_turn);   
       icc_x = robot.pos.x - r_turn*sin(robot.azimuth*M_PI/180.0);
       icc_y = robot.pos.y + r_turn*cos(robot.azimuth*M_PI/180.0);
       double pos_x_prev = robot.pos.x;
       double pos_y_prev = robot.pos.y;
       robot.pos.x = icc_x + cos(df_turn)*(pos_x_prev - icc_x) - sin(df_turn)*(pos_y_prev - icc_y);
       robot.pos.y = icc_y + sin(df_turn)*(pos_x_prev - icc_x) + cos(df_turn)*(pos_y_prev - icc_y);
       robot.azimuth = robot.azimuth + df_turn*180.0/M_PI; 
    }
    if (DEBUG_OUT){
       std::cout<<"      after: x = "<< robot.pos.x<<" y = "<<robot.pos.y;
       std::cout<<" azimuth="<<robot.azimuth<<std::endl;
    }
}



/**************MAZE DATA*************/
struct Point2D{int x; int y; int r ; int g; int b;};
struct Curve { std::vector<Point2D> points;int r ; int g; int b; };
struct Wall {Point2D p1; Point2D p2;int r ; int g; int b;};
struct LocalImage {sf::Image image; Point2D p;}; //duck
// whole bloody setup
struct Table{
  int mazeSpanX, mazeSpanY;
  std::vector<Curve> curves;
  std::vector<Wall> walls;
  //std::vector<sf::Sprite> images;
  std::vector<LocalImage> localImages;
  sf::Image backTexture;
} table;

// split string by delimiter
std::vector<std::string> split (const std::string &s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss (s);
    std::string item;
    while (getline (ss, item, delim)) {
        result.push_back (item);
    }
    return result;
}

//loads curves
int loadMaze(std::string fileName){
	//std::string fileName = "maze1.txt";
	std::ifstream inputFile;
	inputFile.open(fileName);
	//std::cout<<" Is file good? "<<inputFile.good()<<std::endl;
	if ( inputFile.good() != 1){
		std::cout<<" Error opening "<<fileName<<std::endl;
		return -1;
	}
	
    Curve currentCurve;
    Wall currentWall;
    std::string s;
  	while (inputFile>>s) {
       // parse string by comma
       std::vector<std::string> v = split(s, ',');
       for (auto i : v) {
		   std::cout << i << " ";
       }
       std::cout<<std::endl;
       if (v[0] == "#") {continue;} // comment
       if (v[0] == "size") {
		   table.mazeSpanX = std::stoi(v[1]);
		   table.mazeSpanY = std::stoi(v[2]);
	   }
	   if (v[0] == "back"){
		  std::string fileName = v[1];
	      table.backTexture.loadFromFile(fileName);
	   }
	   if (v[0] == "curve"){
		   currentCurve.r = std::stoi(v[1]);
		   currentCurve.g = std::stoi(v[2]);
		   currentCurve.b = std::stoi(v[3]);
		   
		   Point2D currentPoint;
		   for ( unsigned int j = 4;j<v.size()-1;j = j+2){
		     currentPoint.x = std::stoi(v[j]);
		     currentPoint.y = std::stoi(v[j+1]);
		     //std::cout<<"    x="<<currentPoint.x<<" y="<<currentPoint.x<<std::endl;
		     currentCurve.points.push_back(currentPoint);
	       }
   		   table.curves.push_back(currentCurve);
   		   currentCurve.points.clear();
	   }
	   if (v[0] == "wall"){
		   currentWall.r = std::stoi(v[1]);
		   currentWall.g = std::stoi(v[2]);
		   currentWall.b = std::stoi(v[3]);
		   currentWall.p1.x = std::stoi(v[4]);
		   currentWall.p1.y = std::stoi(v[5]);
		   currentWall.p2.x = std::stoi(v[6]);
		   currentWall.p2.y = std::stoi(v[7]);
		   table.walls.push_back(currentWall);
	   }
	   
	   if ( v[0] == "image"){
		   int xc = std::stoi(v[1]);
		   int yc = std::stoi(v[2]);
		   std::string fileName = v[3];
	
	       LocalImage li;
	       //sf::Image image;
	       li.image.loadFromFile(fileName);
	       li.p.x = xc;
	       li.p.y = yc;
  	       table.localImages.push_back(li);
	   }
    }    
	return 0;
}



/**************GRAPHICAL DATA************/
// too slow to make functional
sf::RenderWindow globalWindow;
//sf::RenderTexture globalTexture;
sf::RenderWindow cameraWindow;


/*********DRAWING THINGS**************/
// draws maze into both windows(global and camera)
void drawMaze(){
	
	// maze section
	//sf::RectangleShape rectangle(sf::Vector2f(180.f, 5.f));
	// crete maze background texture
    //sf::Image img;
    //img.create(20, 20);
    //for (auto i = 0; i < 20; ++i) {
    // for (auto j = 0; j < 20; ++j) {
    //    img.setPixel(i, j, sf::Color(0,130,0));
    //  }
    //}
    sf::Texture texture;
    texture.loadFromImage(table.backTexture);
    texture.setRepeated(true);
    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setTextureRect({ 0, 0,table.mazeSpanX, table.mazeSpanY });
	globalWindow.draw(sprite);
	cameraWindow.draw(sprite);
	
	
    sf::RectangleShape rectangle(sf::Vector2f(180.f, 5.f));
    
    for (unsigned int iCurve = 0 ; iCurve < table.curves.size(); iCurve++){
		int r = table.curves.at(iCurve).r;
		int g = table.curves.at(iCurve).g;
		int b = table.curves.at(iCurve).b;
		rectangle.setFillColor(sf::Color(r,g,b));
		for (unsigned int iPoint = 0 ; iPoint < table.curves.at(iCurve).points.size()-1;iPoint++){
			double x1 = (double)table.curves.at(iCurve).points.at(iPoint).x;
			double y1 = (double)table.curves.at(iCurve).points.at(iPoint).y;
			double x2 = (double)table.curves.at(iCurve).points.at(iPoint+1).x;
			double y2 = (double)table.curves.at(iCurve).points.at(iPoint+1).y;
			//std::cout<<" x1="<<x1<<"  y1="<<y1;
			//std::cout<<" x2="<<x2<<"  y2="<<y2;
            //rectangle.setPosition((x1+x2)*0.5, (y1+y2)*0.5);
            rectangle.setPosition(x1, y1);
            double dx = x2-x1; double dy = y2-y1;
            double length = sqrt(dx*dx+dy*dy);
            double angle = atan2(dy,dx)*180.0/M_PI;
            //std::cout<<" length="<<length<<" ang="<<angle<<std::endl;
            rectangle.setSize(sf::Vector2f(length, 4.f));
            rectangle.setRotation(angle); 
            cameraWindow.draw( rectangle );
            globalWindow.draw(rectangle);
		}
	}
	for (unsigned int iWall = 0 ; iWall < table.walls.size(); iWall++){
		int r = table.walls.at(iWall).r;
		int g = table.walls.at(iWall).g;
		int b = table.walls.at(iWall).b;
		rectangle.setFillColor(sf::Color(r,g,b));
		double x1 = (double)table.walls.at(iWall).p1.x;
		double y1 = (double)table.walls.at(iWall).p1.y;
		double x2 = (double)table.walls.at(iWall).p2.x;
		double y2 = (double)table.walls.at(iWall).p2.y;
		
        rectangle.setPosition(x1, y1);
        double dx = x2-x1; double dy = y2-y1;
        double length = sqrt(dx*dx+dy*dy);
        double angle = atan2(dy,dx)*180.0/M_PI;
        //std::cout<<" length="<<length<<" ang="<<angle<<std::endl;
        rectangle.setSize(sf::Vector2f(length, 14.f));
        rectangle.setRotation(angle); 
        cameraWindow.draw(rectangle );
        globalWindow.draw(rectangle);
    }
    for ( unsigned int iImage = 0 ; iImage<table.localImages.size(); iImage++){
 	   //sf::Sprite imSprite1 = table.images.at(iImage);
 	   //(imageTexture);
	    LocalImage lImage = table.localImages.at(iImage);
	    sf::Texture imageTexture;
	    imageTexture.loadFromImage(lImage.image);
	    sf::Sprite sprite1(imageTexture);
	    sprite1.move(lImage.p.x,lImage.p.y);  
    	globalWindow.draw(sprite1);
    	cameraWindow.draw(sprite1);
	}	
	 //sf::Texture imageTexture = table.textures.at(iImage);
     //if (!imageTexture.loadFromFile("duck.png") ){
	//		    std::cout<<" Can not load image "<<std::endl;
	 // };
	               
//	  globalWindow.draw(sprite1);  
	//  cameraWindow.draw(sprite1);  
	                 
}

