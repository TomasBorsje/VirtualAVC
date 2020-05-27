#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <vector>
#include <iostream>
#include <math.h>
#include <fstream>

/***********ROBOT CODE***************/
struct Robot{
	double x,y;        // position
	double azimuth;    // robot angle
	double vLeft,vRight; //spped of left and right motor
	double wheelRadius;
	double width;   // distance between wheels
	double cameraForward;  //
} robot;

void initRobot(Robot& robot){
    robot.x = 0.0; robot.y = 0.0; robot.azimuth = 45.0;
    robot.vLeft = 16.0; robot.vRight = 19.0;
    robot.wheelRadius = 20.0;
    robot.width = 30.0;
    robot.cameraForward = 40.0;
}

void drawRobot(sf::RenderWindow& window){
	// draw robot body rectangle
	sf::RectangleShape outline;
	//outline.setPosition(0.0f, 0.0f);
    outline.setSize(sf::Vector2f(30.f, 20.f));
    outline.setFillColor(sf::Color(255,0,0));
	outline.setPosition(robot.x, robot.y);
    outline.setRotation(robot.azimuth);
    window.draw(outline);
    // draw camera Field-of-View
	sf::RectangleShape FOV;
	
	FOV.setPosition(robot.x+ robot.cameraForward*cos(robot.azimuth*M_PI/180.0), 
	                robot.y + robot.cameraForward*sin(robot.azimuth*M_PI/180.0) );
    FOV.setRotation(robot.azimuth);
    FOV.setSize(sf::Vector2f(40.f, 50.f));
    FOV.setFillColor(sf::Color::Transparent);
    FOV.setOutlineColor(sf::Color(255,255,255));
    FOV.setOutlineThickness(3.0f);
    window.draw(FOV);
}

void moveRobot(Robot& rob){
	float l_left;   // left wheel movement
    float l_right;  // right wheel movement
    float d_l_r;
    float s_l_r;
    float r_turn;    // radius of turn
    float df_turn;   // degree of turn
    float icc_x;
    float icc_y;
    //std::cout<<" vLeft = "<< rob.vLeft<<" vRright = "<<rob.vRight<<std::endl;

    //println("  before move:  pos_x="+pos_x+" pos_y="+pos_y + " pos_teta="+pos_teta);
    // calculate wheel tip movements- lenght of the arc: 
    l_left =  rob.wheelRadius * rob.vLeft * M_PI/180.0; 
    l_right = rob.wheelRadius * rob.vRight * M_PI/180.0;
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
      rob.x = rob.x + l_left*cos(rob.azimuth*M_PI/180.0);
      rob.y = rob.y + l_right*sin(rob.azimuth*M_PI/180.0);
    } else 
    {
       // robot is rotating
       //std::cout<<" robot turning"<<std::endl;
       r_turn = (rob.width*0.5)*s_l_r/d_l_r;
       df_turn = d_l_r / rob.width;                              //radians
      // println("  r_turn ="+r_turn + " df_turn="+df_turn);   
       icc_x = rob.x - r_turn*sin(rob.azimuth*M_PI/180.0);
       icc_y = rob.y + r_turn*cos(rob.azimuth*M_PI/180.0);
       double pos_x_prev = rob.x;
       double pos_y_prev = rob.y;
       rob.x = icc_x + cos(df_turn)*(pos_x_prev - icc_x) - sin(df_turn)*(pos_y_prev - icc_y);
       rob.y = icc_y + sin(df_turn)*(pos_x_prev - icc_x) + cos(df_turn)*(pos_y_prev - icc_y);
       rob.azimuth = rob.azimuth + df_turn*180.0/M_PI; 
    }
   
}

/**************MAZECODE*************/
struct Point2D{int x; int y;};
struct Curve { std::vector<Point2D> points;};
std::vector<Curve> maze;

int loadMaze(std::string fileName){
	//std::string fileName = "maze1.txt";
	std::ifstream inputFile;
	inputFile.open(fileName);
	std::cout<<" Is good? "<<inputFile.good()<<std::endl;
	if ( inputFile.good() != 1){
		std::cout<<" Error opening "<<fileName<<std::endl;
	}
    std::string s;
    //inputFile>>s;
    Curve currentCurve;
  	while (inputFile>>s) {
       std::cout << s<<std::endl;
       if (s=="***"){ // new curve
		   maze.push_back(currentCurve);
		   currentCurve.points.clear();
	   } else{ //point
		   int x; int y; int col;
		   sscanf (s.c_str(),"%d,%d,%d",&x,&y,&col);
		   std::cout<<"   x="<<x<<" y="<<y<<std::endl;
		   Point2D currentPoint;
		   currentPoint.x = x; currentPoint.y = y;
		   currentCurve.points.push_back(currentPoint);
	   }
    }
	return 0;
}


int drawMaze(sf::RenderWindow& window){
	sf::RectangleShape rectangle(sf::Vector2f(180.f, 5.f));
 
    rectangle.setFillColor(sf::Color(255,255,255));
    for (unsigned int iCurve = 0 ; iCurve < maze.size(); iCurve++){
		for (unsigned int iPoint = 0 ; iPoint < maze.at(iCurve).points.size()-1;iPoint++){
			double x1 = (double)maze.at(iCurve).points.at(iPoint).x;
			double y1 = (double)maze.at(iCurve).points.at(iPoint).y;
			double x2 = (double)maze.at(iCurve).points.at(iPoint+1).x;
			double y2 = (double)maze.at(iCurve).points.at(iPoint+1).y;
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
            window.draw( rectangle );
		}
		//std::cout<<"*****"<<std::endl;
	    //int stop;
        //std::cin>>stop;
	}
    return 1;
}


/*********NETWORK*********/


/***************MAIN LOOP**************/
int main()
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");
	loadMaze("maze1.txt");
    initRobot(robot);

     /*
    //networking stuff
    sf::TcpListener listener;
    // bind the listener to a port
    if (listener.listen(53000) != sf::Socket::Done){
       std::cout<<" Error binding the socket"<<std::endl;
    }
    std::cout<<" Listening"<<std::endl;
    // accept a new connection
    sf::TcpSocket client;
    if (listener.accept(client) != sf::Socket::Done) { //blocks
       std::cout<<"Socket error"<<std::endl;
    }
    std::cout<<" Client accepted connection"<<std::endl;
    client.setBlocking(0);
    std::cout<<" Socket is blocking: "<<client.isBlocking()<<std::endl;
    */
    
    sf::Clock TTimer;
    const sf::Time TTime   = sf::seconds(1.0f);
    
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event)) {
            // Close window: exit
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        if (TTimer.getElapsedTime() > TTime) {
           TTimer.restart();
           std::cout<<"Tick.."<<std::endl;
           /*
           // poll incoming packets
           sf::Packet packet;
           if (client.receive(packet) != sf::Socket::Done){ //blocking
             std::cout<<" Nothing received"<<std::endl;
           } else {
             sf::Uint16 x;
             std::string s;
             double d;
             packet>>x>>s>>d;
             std::cout<<"  Received: x="<< x<<" s="<<s<<" d="<<d<<std::endl;
             packet.clear();
             // send back
             if ( x == 0){
                 sf::Packet packetToSend;
                 std::string s1 = "reply";
                 packetToSend<<s1;
                 if (client.send(packetToSend) != sf::Socket::Done)  {
                   std::cout<<" Error sending"<<std::endl;
                 } else {
		             std::cout<<" Send:"<<s1<<std::endl;
		         }
			 }
			 if (x==1) { robot.vLeft--;}
			 if (x==2) { robot.vLeft++;}
			 if (x==3) { robot.vRight--;}
			 if (x==4) { robot.vRight++;}
			 
	       }
           */
           moveRobot(robot);
           //std::cout<<" x="<<robot.x<<" y="<<robot.y<<" Angle="<<robot.azimuth<<std::endl;
        }
        window.clear();
        drawMaze(window);
        drawRobot(window);
        window.display();
    }
    return EXIT_SUCCESS;
}
