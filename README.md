# Instructions to run this project (Windows 10)

Install the latest MinGW version from https://osdn.net/projects/mingw/releases/ for Windows.\
When installing MinGW, make sure to tick the mingw32-gcc-g++ package as it is required.\
Install Geany from https://www.geany.org/ for Windows.\
First download the SFML library from https://www.sfml-dev.org/download/sfml/2.5.1/ and extract it somewhere you can remember.\
Download the version that is appropriate for you and the one that aligns with your version of MinGW.\
Example if you are downloading the "GCC 7.3.0 MinGW (SEH) - 64-bit" SFML library you also have downloaded "MinGW Builds 7.3.0 (64-bit)" as the library version you download must match the compiler version you are using.\
Next in Geany is the build tab at the top, click on set build commands and set make to " mingw32-make ".\
Next you need to download our AVC_robot folder and our AVC_server folder. Both are needed to run this project.\
Next copy the .dll files from the SFML library from the bin folder into the AVC_server and AVC_robot folder.\
Next delete server3.exe from AVC_server and robot.exe from AVC_robot.\
Next open the makefile in the AVC_server folder and change the top line to the file pathway that leads you to the SFML library and then build it using the build tab at the top of Geany.\
Next open the makefile in the AVC_robot folder and change the top line to the file pathway that leads you to the SFML library and then build it using the build tab at the top of Geany.\
Next run the server3.exe file and then the robot.exe file. The robot shoud move start to move.\
To swap to a different maze open the config.txt file in AVC_server and change the first line called mazeFile to completion.txt for the completion maze or challenge.txt for the challenge maze. To change back you can write core.txt for the core maze.\
You must save config.txt for the change in maze to work.\
To swap what code the robot.exe is running you must first delete the robot.o file and the robot.exe file and then edit the makefile in AVC_robot.\
For it to run the completion code change robot.cpp to robotcompletion.cpp in both instances and then on the line after robot: robot.o, change robot.o to robotcompletion.o and then build it.\
For it to run the challenge code make the same changes you would for completion, replacing completion with challenge and then build it.\
If the makefile doesn't work make sure the space in front of the lines beginning with g++ is a tab and not four spaces.\
Example of edited makefile that runs the challenge robot code:\
SFML = C://SFML/SFML-2.5.1  \
CFLAGS = -I ${SFML}/include \
LFLAGS = -L ${SFML}/lib -Wl,-R ${SFML}/lib  \
LIBS = -lsfml-window  -lsfml-graphics -lsfml-system -lsfml-network  \
robot: robot.o \
	g++ $(LFLAGS) -o robot robotchallenge.o ${LIBS} \
robot.o: robotchallenge.cpp robot.hpp \
	g++  -c $(CFLAGS) robotchallenge.cpp robot.hpp




# AVC Plan

# Team Name: Team 22

# Team Members & contact info:

```
● Tomas B / Tomas#9529 (Discord) / ​borsjetoma@myvuw.ac.nz
● Rose T/ WhiteRose#6406 (Discord) / ​taylorrose3@myvuw.ac.nz
● Ryan H / Rith#7410 (Discord) / ​riththurn@gmail.com​ & ​hurnenryan@myvuw.ac.nz
● Connor N / Meowth (very Important )#2752 (Discord) / nobbsconn@myvuw.ac.nz
```
# Communication tool:

Google doc, Discord group chat, Github issues & wiki, email.

# Roles:

Rose - Project Manager (organising team meetings, reporting regularly on progress)\
Connor - Software Architect (writing core code and extending functionality)\
Tomas - Software writing, testing and documentation (debugging software and committing to
git, writing test cases and documenting performance against milestones)\
Ryan - Software writing, testing and documentation (debugging software and committing to
git, writing test cases and documenting performance against milestones)\
Github: ​https://github.com/TomasBorsje/VirtualAVC

# Project Milestones

# Project & Git Setup - Due ~6th June

Familiarising ourselves with the workspace (Git, Geany, Visual Studio) and making sure
we can all work on the project properly.

## Objectives ​[Everyone]

```
● Complete project plan document
● Setup SFML on everybody’s computers
● Familiarise ourselves with Git & Github
● Create some test cases that we will test when we can
● Begin thinking about algorithms/methods our robot can use to move properly
● Move project plan document onto Github’s “readme.MD” file
```

# Core Code Design & Creation - Due ~10th June

Creating the basic functionality of the robot and getting into the flow of documenting,
coding, and committing on Github.

## Objectives

```
● [Everyone] Discuss progress, issues & testing on Github wiki
● [Software Architect] Begin creating the core code our robot will use
● [Software Tester] Write test cases the robot may encounter
● [Software Tester] Debug code created
● [Software Tester] Assist in code creation
● [Software Tester] Document our progress in comparison to milestones
● [Project Manager] Keep up to date on our team’s progress
● [Project Manager] Organise check ins or discussions (Discord)
```
# Extending Code Functionality - Due ~14th June

Finishing our code, ensuring the robot works in all cases. Making sure our documentation,
testing, and progress reports are clean. Begin work on our reports for the customer.

## Objectives

```
● [Everyone] Continue to discuss progress, issues & testing on Github wiki
● [Everyone] Work on individual reports for the customer.
● [Software Architect] Ensure the robot’s core functions work as intended
● [Software Tester] Test test cases that have become available
● [Software Tester] Document results of test cases on Github
● [Software Tester] Continue to debug code created
● [Software Tester] Continue to assist in code creation
● [Software Tester] Document our progress in comparison to milestones
● [All Coders] Ensure the robot works on Core, Completion and Challenge
● [All Coders] If it doesn’t, debug & repair code
● [Project Manager] Keep up to date on our team’s progress
● [Project Manager] Organise check ins or discussions (Discord)
● [Project Manager] Help out on any small tasks that may need completion
```
# Cleanup - Due ~16th June (Project Due Date)

Making sure everything is finished. Cleaning up any small bugs or tasks left to do.

## Objectives

```
● [Everyone] Continue to discuss progress, issues & testing on Github wiki
● [Software Architect] Ensure no code functions are longer than 80 operators (;)
● [Software Tester] Clean up any last bugs that may exist
● [Software Tester] Make sure testing has been documented
● [Software Tester] Continue to assist in code creation
● [Software Tester] Document our progress in comparison to milestones
● [Project Manager] Keep up to date on our team’s progress
● [Project Manager] Organise check ins or discussions (Discord)
```
