<General information on the authors of this assignment>
The name of the group is YK17. The member is Yusuke.

<The purpose of this assignment>
	I implemented four kinds of programs to measure the speed efficiency when transferring data between a producer and a consumer using different models. The four programs transmit the data with unnamed pipe, named pipe, socket and shared memory with circular buffer.

<The contents of the package>
	This package is composed of three bash file (install, run.sh and help.sh) , readme.txt and four 
source files.The source files are unnamed.c, named.c, socket.c and 
shared.c.

<How to execute the program>

1) Execute ". ./install.sh ./exec" in main konsole which enables you to unzip and compile all of 
	the files.

2) Execute "./help.sh" in main konsole to get the explanation of this program. If you don't have 
	permission, you should input "chmod +x help.sh." 

3) Execute "./run.sh" in main konsole which enables you to run this program.

<Setting the size of buffers and calculation of the transmitting time >

  From the Command console, you can move the position of the hoist's hook by controlling the 
movements of the two motors: motor_x and motor_z. Motor_x moves the hook horizontally and motor_z 
moves the hook vertically.

	>There are three commands for motor x:
		 
			a: left, d: right, x: stay
		'a' for going left, 'd' for going right and 'x' for braking and keeping the postion on x-axis.
	 
	>Motor_z is controlled in a similar manner:

			w: up, s: down, x: stay
		'w' for going up, 's' for going down and 'z' for staying the postion on z-axis. 

	Both motors can move 10 meters from the center reset position of the hoist. Inspection reports 
both theoretical positions (thx and thz) of the host and the 'real' positions which include a 
biased error that simulates what would happen in a realistic scenario.

	From the Inspection konsole, you can give two commands: Position Reset and Emergency Stop.
Position reset automatically brings back the hook to it's working environment's center position 
(thx=0,th=0). This command can be issued at any time from any starting position. Emergency Stop 
can be issued at any moment and stops motor operations completely until a special command 'u' is 
issued from the command console to 'unfreeze' motor operations on both axes. 

	If no command has been issued and/or no motor movement has occoured during a 60 seconds period, 
a watchdog process will send a signal to the Inspection console to reset the hook's position to 
its origin point (thx=0,thz=0). The hoist will keep that position until a new command is issued.

	When ctrl+c is sent on the main terminal (which also acts a a monitor to display general 
operation conditions) the master process will shut down all other processes and unlink all 
related named pipes.



