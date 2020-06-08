#include <SFML/Network.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <unistd.h>

/**FUNCTIONS****/
int connectNetwork();

/*****IMAGE - same as Project2*****/
struct ImagePPM{
	int width, height;
	int n_bytes;
	char* data;
} cameraView;

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



int initClientRobot(){
	std::string fileName = "config.txt";
	std::ifstream inputFile;
	inputFile.open(fileName);
	if ( inputFile.good() != 1){
		std::cout<<" Error opening config"<<fileName<<std::endl;
		return -1;
	}
	// read maze filename and robot configuration
	std::string s;
  	while (inputFile>>s) {
		 //std::cout<<s<<std::endl;
		 std::vector<std::string> v = split(s, ',');
         for (auto i : v) {
		   std::cout << i << " ";
		   /*
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
		   */ 
		   if (v[0] == "robot.fovWidth"){cameraView.width = std::stod(v[1]);}
		   if (v[0] == "robot.fovLength"){cameraView.height = std::stod(v[1]);}
         }
         std::cout<<std::endl;
	}
	//std::cout<<" Maze file="<<mazeFileName<<std::endl;
    cameraView.data = new char[cameraView.width * cameraView.height*3];
    cameraView.n_bytes = cameraView.width * cameraView.height*3;
     if (connectNetwork()!=0){
		 std::cout<<" Can not connect to server"<<std::endl;
		return -2;
	}
	
	return 0;
}

// returns color component (color==0 -red,color==1-green,color==2-blue
// color == 3 - luminocity
// for pixel located at (row,column)
unsigned char get_pixel( ImagePPM image,int row,int col, int color)
{
    // calculate address in 1D array of pixels
    int address = image.width*row*3 + col*3;  // standard ppm
    //int address = image.width*row*4 + col*4;
    if ((row < 0 ) || (row > image.height) )  {
        printf("row is out of range\n");
        return -1;
    }
    if ( (col< 0) || (col > image.width))    {
        printf("column is out of range\n");
        return -1;
    }
    unsigned char r = image.data[address];
    unsigned char g = image.data[address+1];
    unsigned char b = image.data[address+2];
//std::cout<<"GP "<<(int)r<<" "<<(int)g<<" "<<(int)b<<std::endl;		          
    if (color==0) { return r;}
    if (color==1) { return g;}
    if (color==2) { return b;}
    if (color==3)    {
		int yi = r+g+b;
        unsigned char y = yi/3;
//std::cout<<"GP y="<<(int)y<<std::endl;		          
        
        return y;
    }
    printf("Color encoding wrong: 0-red, 1-green,2-blue,3 - luminosity\n");
    return -2; //error
}

int set_pixel(ImagePPM& image, int row, int col,
              unsigned char red,unsigned char green,unsigned char blue)
{
    //int address = image.width*row*4 + col*4; // RPI
    int address = image.width*row*3 + col*3;  // standard ppm
    if ((address < 0) || (address>image.width*image.height*3))
    {
        printf("SetPixel(): wrong x,y coordinates\n");
        printf(" row= %d col=%d address=%d\n",row,col,address);
        printf(" width= %d height=%d\n",image.width,image.height);
        return -1;
    }
    image.data[address] = red;
    image.data[address+1] = green;
    image.data[address+2] = blue;
    return 0;
}

int SavePPMFile(std::string filename, ImagePPM& image){
	std::ofstream fout;
	fout.open(filename,std::ofstream::binary);
	if (!fout.is_open()){ std::cout<<"no file"; return -1;}
	fout<<"P6"<<std::endl;
	fout<<(int)image.width<<" "<<(int)image.height<<" 255"<<std::endl;
	
	fout.write(image.data,image.width*image.height*3);
	fout.close();
	return 0;
}

int OpenPPMFile(std::string filename,ImagePPM& image){
	std::cout<<"Opening single ppm file"<<std::endl;
	
	std::ifstream fin;
	fin.open(filename);
	if (!fin.is_open()){ std::cout<<"no file"; return -1;}
    // get first line - contains P6
    std::string fd1;
    getline(fin,fd1);
    std::cout<<fd1<<std::endl; 
    // get secondline
    getline(fin,fd1);
    std::cout<<fd1<<std::endl; 
    int depth;
    sscanf( fd1.c_str(),"%d %d %d\n",&image.width ,&image.height,&depth );
    // size of the image
    // parse string
    std::cout<<"image.height = "<<image.height;
    std::cout<<" image.width = "<<image.width<<std::endl;
    
    image.n_bytes =  image.width*image.height*3;
    image.data = new char[image.n_bytes];
    fin.read(image.data, image.n_bytes);
    fin.close();
    return 0;
}


/**********NETWORK****/

sf::TcpSocket socket;
sf::Packet packet;

int connectNetwork(){
   
   sf::Socket::Status status = socket.connect("127.0.0.1", 53000);
   if (status != sf::Socket::Done)  {
       std::cout<<"Error socket"<<std::endl;
       return -1;
   }
   //socket.setBlocking(0);
   return 0;
}

int takePicture(){
	sf::Packet packet;
    sf::Uint16 command =0;
    std::string text = "hello";
    double value = 0.0;
    double value1 = 0.0;
    double value2 = 0.0;
    double value3 = 0.0;
    // fill the packet
	packet<<command<<text<<value<<value1<<value2<<value3;
    if (socket.send(packet) != sf::Socket::Done)  {
          std::cout<<" Error sending"<<std::endl;
    } else {
	  // std::cout<<" Send OK"<<std::endl;
	}
	
	sf::Packet receivePacket;
    if (socket.receive(receivePacket) != sf::Socket::Done){ // non blocking
           std::cout<<" Nothing received"<<std::endl;
          return -1;
    } else {
	 // std::cout<<" Received"<<std::endl;
	  int imageWidth;
	  receivePacket>>imageWidth;
	  int imageHeight;
	  receivePacket>>imageHeight;
	  std::cout<<" width="<<imageWidth<<std::endl;
	  std::cout<<" height="<<imageHeight<<std::endl;
	  for ( int row = 0 ; row < imageHeight; row++){
		for ( int col = 0 ; col < imageWidth; col++){
		  unsigned char r;
		  receivePacket>>r;
		  unsigned char g;
		  receivePacket>>g;
		  unsigned char b;
		  receivePacket>>b;
		  if ( set_pixel(cameraView,row,col,r,g,b) != 0 ){
				  return -1;
		  };	
		  //std::cout<<"TP: "<<row<<" "<<col<<" "<<(int)r<<" "<<(int)g<<" "<<(int)b<<std::endl;
		 }
	  }
	  //std::cout<<" end"<<std::endl;
	  //SavePPMFile("i0.ppm",cameraView);
	} //else
    return 0;
}

int setMotors(double vLeft, double vRight){
	sf::Packet packet;
    sf::Uint16 command = 1;
    std::string text = "hello";
    double value = vLeft;
    double value1 = vRight;
    double value2 = 0.0;
    double value3 = 0.0;
    // fill the packet
    packet.clear();
	packet<<command<<text<<value<<value1<<value2<<value3;
    if (socket.send(packet) != sf::Socket::Done)  {
         std::cout<<" Error sending"<<std::endl;
    } 

	return 0;
}

