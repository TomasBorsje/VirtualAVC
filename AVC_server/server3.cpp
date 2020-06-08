#include "server3.hpp"


void drawRobot(){
   	// draw robot body outline rectangle
    robot.outline.setPosition(robot.pos);
    robot.outline.setRotation(robot.azimuth);
    globalWindow.draw(robot.outline);

   	// draw robot image
    sf::Texture texture;   
    texture.loadFromImage(robot.bkImage);
    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setTextureRect({ 0, 0, (int)robot.length, (int)robot.width });
    sprite.setOrigin(sf::Vector2f(robot.length/2.0, robot.width/2.0));
    sprite.setPosition(sf::Vector2f(robot.pos.x,robot.pos.y));
    sprite.setRotation(robot.azimuth);
  
    globalWindow.draw(sprite);
  
    //std::cout<<"    rob.x="<<robot.pos.x<<" rob.y"<<robot.pos.y;  
    //std::cout<<"  az="<<robot.azimuth<<std::endl; 
    //std::cout<<"  rob.x="<<robot.x<<" rob.y"<<robot.y<<std::endl;  

    // FOV center
    double x1= robot.pos.x+robot.cameraForward*cos(robot.azimuth*M_PI/180.0);
    double y1= robot.pos.y+robot.cameraForward*sin(robot.azimuth*M_PI/180.0) ;
    //std::cout<<"    x1="<<x1<<" y1="<<y1;

	robot.cameraFOV.setPosition(sf::Vector2f(x1, y1));
    robot.cameraFOV.setOrigin(sf::Vector2f(robot.fovLength/2.0,robot.fovWidth/2.0 ));
    robot.cameraFOV.setRotation(robot.azimuth);

    //double xFOV1 = robot.cameraFOV.getPosition().x;
    //double yFOV1 = robot.cameraFOV.getPosition().y;
    //std::cout<<"    xFOV1="<<xFOV1<<"  yFOV1="<<yFOV1<<std::endl;
    //globalWindow.draw(robot.outline);
    globalWindow.draw(robot.cameraFOV);
    
    
}


void saveCameraView(){
	    //copy pixels of the camera view window to texture
    //sf::Vector2u windowSize = cameraWindow.getSize();
    sf::Texture localTexture;
    localTexture.create(cameraWindow.getSize().x,cameraWindow.getSize().y );
    //localTexture.clear();
    
    sf::View view = cameraWindow.getDefaultView();
    //view.zoom(0.5f);
    cameraWindow.setView(view);
    
    localTexture.update(cameraWindow);
    //std::cout<<" Image dx="<<localTexture.getSize().x<<std::endl;
    //std::cout<<" Image dy="<<localTexture.getSize().y<<std::endl;
    
    // copy texture to image
    sf::Image screenShot = localTexture.copyToImage();
    screenShot.saveToFile("cam.png");
    // get pixels of the image and save them
    int count = 0;
    for ( int x = (screenShot.getSize().x-1) ; x>-1; x--){
		for (unsigned int y = 0 ; y<(screenShot.getSize().y); y++){
			//screenshot
			const sf::Color color = screenShot.getPixel(x,y);
			robot.image[count] = (unsigned char)color.r;
			//std::cout<<(int)color.r<<" ";
			count++;
			robot.image[count] = (unsigned char)color.g;
			count++;
			robot.image[count] = (unsigned char)color.b;
			count++;
			//std::cout<<" count  "<<count<<" nImage="<<robot.nImage<<std::endl;
		}
	}
	//robot.nImage = count;

}

// 
int getCameraView()
{
	if ( DEBUG_OUT ){
	   std::cout<<"Inside getCameraView()"<<std::endl;
	   std::cout<<"   robot.x="<<robot.pos.x<<" robot.y="<<robot.pos.y;
	   std::cout<<"   robot.azimuth="<<robot.azimuth<<std::endl;
	   std::cout<<"   cameraForward="<<robot.cameraForward;  //
	   std::cout<<"   fovWidth="<<robot.fovWidth;
	   std::cout<<"   fovLength="<<robot.fovLength<<std::endl;    
    }
	// calculate Field-of_View rectangle
    // set viewport
     sf::View view;
     view.setSize(sf::Vector2f(robot.fovLength,robot.fovWidth));
     //view.setRotation(30.f);
     if (DEBUG_OUT){
        std::cout<<" robot.x="<<robot.pos.x<<" robot.y="<<robot.pos.y<<std::endl;
	 }
     double x1= robot.pos.x+robot.cameraForward*cos(robot.azimuth*M_PI/180.0);
     double y1= robot.pos.y+robot.cameraForward*sin(robot.azimuth*M_PI/180.0) ;
     //std::cout<<"    x1="<<x1<<" y1="<<y1;
     view.setCenter( sf::Vector2f(x1,y1) ); 
     view.setRotation( robot.azimuth);   
     //view.setViewport(sf::FloatRect(0.25f, 0.25, 0.5f, 0.5f)); 
     if (DEBUG_OUT){ 
       std::cout<<"  view.x="<<view.getCenter().x;
       std::cout<<"  view.y="<<view.getCenter().y;
       std::cout<<"  view.rotation="<<view.getRotation()<<std::endl;
	 }
    // copy to window
    cameraWindow.clear();
    cameraWindow.setView(view);
    drawMaze();
    saveCameraView();
    //int stop;
    //std::cin>>stop;
	return 0;
}


int drawAll(){
	globalWindow.clear();
	cameraWindow.clear();
    drawMaze(); // draw without robot
	globalWindow.display();
    getCameraView(); // get camera view
	drawRobot();
	globalWindow.display(); // draw with robot
    cameraWindow.display();	
    return 1;
}

// returns 0 if robot is free to move
bool checkCollisions(){
	// check collisions with walls and images
	bool collision = 0;
	//robot bounding box
	sf::FloatRect robotRect = robot.outline.getGlobalBounds();
	/*   
    std::cout<<" Inside checkCollisions()"<<std::endl;
    std::cout<<" ROBOT"<<std::endl;  
    std::cout<<"  left="<<robotRect.left
             <<"  top="<<robotRect.top
             <<"  width="<<robotRect.width
             <<"  height="<<robotRect.height
             <<std::endl;
     */
     // check collisions with walls
	 for (unsigned int iWall = 0 ; iWall < table.walls.size(); iWall++){
		sf::RectangleShape rectangle(sf::Vector2f(180.f, 5.f));
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
  	    sf::FloatRect wallRect = rectangle.getGlobalBounds();
  	    if ( robotRect.intersects(wallRect)){
			collision = 1;
			break;
		}
  	   if(DEBUG_OUT){ 
        std::cout<<" WALL "<<iWall<<std::endl;  
        std::cout<<"  left="<<wallRect.left
                 <<"  top="<<wallRect.top
                 <<"  width="<<wallRect.width
                 <<"  height="<<wallRect.height
                 <<std::endl;
       } 
    }
    for ( unsigned int iImage = 0 ; iImage<table.localImages.size(); iImage++){
	    LocalImage lImage = table.localImages.at(iImage);
	    sf::Texture imageTexture;
	    imageTexture.loadFromImage(lImage.image);
	    sf::Sprite sprite1(imageTexture);
	    sprite1.move(lImage.p.x,lImage.p.y);  
	    sf::FloatRect imageRect = sprite1.getGlobalBounds();
	    if (DEBUG_OUT){
          std::cout<<" IMAGE "<<iImage<<std::endl;  
          std::cout<<"  left="<<imageRect.left
                 <<"  top="<<imageRect.top
                 <<"  width="<<imageRect.width
                 <<"  height="<<imageRect.height
                 <<std::endl;
		}
  	    if ( robotRect.intersects(imageRect)){
			collision = 1;
			break;
		}
	    
	    
    }
    if (DEBUG_OUT)  std::cout<<" collision="<<collision<<std::endl;
	return collision;
}


/*********NETWORK*********/
sf::TcpListener listener;
sf::TcpSocket client;

// blocks until connection established
int startNetwork(){
	 if (listener.listen(53000) != sf::Socket::Done){
       std::cout<<" Error binding the socket"<<std::endl;
       return -1;
    }
    std::cout<<" Listening"<<std::endl;
    // accept a new connection
    if (listener.accept(client) != sf::Socket::Done) { //blocks
       std::cout<<"Socket error"<<std::endl;
       return -2;
    }
    std::cout<<" Client accepted connection"<<std::endl;
    //client.setBlocking(0);
    return 0;
}

int networkExchange(){
	if (DEBUG_OUT){
	   std::cout<<" Inside networkExchange()"<<std::endl;
   }
 // poll incoming packets
   sf::Packet packet;
     if (client.receive(packet) != sf::Socket::Done){ //blocking
        //std::cout<<" Nothing received"<<std::endl;
   } else { //packet received
       sf::Uint16 command;
       std::string text;
       double value = 0.0;
       double value1 = 0.0;
       double value2 = 0.0;
       double value3 = 0.0;
       packet>>command>>text>>value>>value1>>value2>>value3;
       if (DEBUG_OUT){
          std::cout<<"  Received: com="<< command<<
                     " text="<<text<<
                      " value="<<value<<" value1="<<value1<<std::endl;
       }               
       packet.clear();
       // send back
       if ( command == 0){ // image request
		   //getCameraView(window);
		   //prepare image packet
           sf::Packet packetToSend;
           packetToSend<<robot.fovWidth;
           //std::cout<<" robot.fovWidth="<<robot.fovWidth<<std::endl;
	       //std::cout<<packetToSend<<std::endl;
           //std::cout<<" robot.fovLength"<<robot.fovLength<<std::endl;
           packetToSend<<robot.fovLength;
	       //std::cout<<packetToSend<<std::endl;
           //std::cout<<" robot.nImage="<<robot.nImage<<std::endl;
           for ( int i = 0; i < robot.nImage;i++){
			   packetToSend<<robot.image[i];
               //std::cout<<(int)robot.image[i]<<" ";
	        }
	        //std::cout<<std::endl;
	        // send the packet
            if (client.send(packetToSend) != sf::Socket::Done)  {
				std::cout<<" Error sending"<<std::endl;
            } else {
	            //std::cout<<" Send:"<<std::endl;
	            //std::cout<<packetToSend<<std::endl;
	        }
	   }
	   if (command==1) { 
		   robot.vLeft = value;
		   robot.vRight = value1;
	    } // speed change
	 
	}
	if (DEBUG_OUT){
	   std::cout<<"         vLeft="<<robot.vLeft<<"   vRight="<<robot.vRight<<std::endl;
    }
	return 0;
}

/**********CONFIG FILE READ****/
int configAVC(){
	
	initRobot();
	std::string fileName = "config.txt";
	std::ifstream inputFile;
	inputFile.open(fileName);
	if ( inputFile.good() != 1){
		std::cout<<" Error opening config file"<<fileName<<std::endl;
		return -1;
	}
	// read maze filename and robot configuration
	std::string mazeFileName;
	std::string s;
  	while (inputFile>>s) {
		 //std::cout<<s<<std::endl;
		 std::vector<std::string> v = split(s, ',');
         for (auto i : v) {
		   std::cout << i << " ";
		   if (v[0] == "mazeFile"){mazeFileName = v[1];}
		   if (v[0] == "robot.pos.x"){robot.pos.x = std::stod(v[1]);}
		   if (v[0] == "robot.pos.y"){robot.pos.y = std::stod(v[1]);}
		   if (v[0] == "robot.azimuth"){robot.azimuth = std::stod(v[1]);}
		   if (v[0] == "robot.vLeft"){robot.vLeft = std::stod(v[1]);}
		   if (v[0] == "robot.vRight"){robot.vRight = std::stod(v[1]);}
		   if (v[0] == "robot.wheelRadius"){robot.wheelRadius = std::stod(v[1]);}
		   if (v[0] == "robot.width"){robot.width = std::stod(v[1]);}
		   if (v[0] == "robot.length"){robot.length = std::stod(v[1]);}
		   if (v[0] == "robot.cameraForward"){robot.cameraForward = std::stod(v[1]);}
		   if (v[0] == "robot.fovWidth"){robot.fovWidth = std::stod(v[1]);}
		   if (v[0] == "robot.fovLength"){robot.fovLength = std::stod(v[1]);}
         }
         std::cout<<std::endl;
	}
	//std::cout<<" Maze file="<<mazeFileName<<std::endl;

	// load maze from file
	if (loadMaze(mazeFileName) != 0){
		std::cout<<" Error opening maze file"<<std::endl;
		return -2;
	}
	// create windows
    globalWindow.create( sf::VideoMode(table.mazeSpanX, table.mazeSpanY), 
                         "Global view");
    cameraWindow.create( sf::VideoMode((double)robot.fovLength, 
                                       (double)robot.fovWidth), 
                                       "Camera view",sf::Style::None);
    cameraWindow.setPosition(sf::Vector2i(table.mazeSpanX, 0));//mazeSpanY));  
    std::cout<<" cameraWindow.getSize(x).x="<<cameraWindow.getSize().x;
    std::cout<<" cameraWindow.getSize(x).7="<<cameraWindow.getSize().y<<std::endl;                                       

	return 0;
}

/***************MAIN LOOP**************/
int main(int argc, char **argv)
{
	if (configAVC()!= 0){
		std::cout<<" Error configuring AVC server"<<std::endl;
		return -1;
	}
    drawAll();
    
    if (startNetwork()!=0){	std::cout<<"Network error"<<std::endl; return -3; }
       
    sf::Clock TTimer;
    const sf::Time TTime   = sf::seconds(0.1f);
    while (globalWindow.isOpen())
    {
        // Process events
        sf::Event event;
        while (globalWindow.pollEvent(event)) {
            // Close window: exit
            if (event.type == sf::Event::Closed){
                cameraWindow.close();
                globalWindow.close();
			}
        }
        if (TTimer.getElapsedTime() > TTime) {
           TTimer.restart();
           if (DEBUG_OUT) { std::cout<<"Tick.."<<std::endl;}
           networkExchange();
           moveRobot();
           drawAll();
        }
        
    }
    return EXIT_SUCCESS;
}
