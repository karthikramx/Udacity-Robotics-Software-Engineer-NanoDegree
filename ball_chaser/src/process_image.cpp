#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ROS_INFO_STREAM("Moving the robot");

    // Request centered joint angles [1.57, 1.57]
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    // Call the safe_move service and pass the requested joint angles
    if (!client.call(srv))
        ROS_ERROR("Failed to call service safe_move");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
    int left = 0;
    int right = 0;
    int centre = 0;

    //ball detection
    for (int i = 0; i < img.height * img.step; i += 3) 
	{
         int row = int (i/img.height);
         int col = (i + img.width) % img.width;
        if(img.data[i] == 255 && img.data[i+1] == 255 && img.data[i+2] == 255){
	  if(col<img.width/3)
		{left++;}

          else if(col>2*img.width/3)
		{right++;}

	  else if(col>img.width/3 && col<2*img.width/3)
		{centre++;}
	}
    	}
    
    if(left > right && left > centre)
	{drive_robot(0.0, 0.25);}

    else if(right > left && right > centre)
	{drive_robot(0.0,-0.25);}

    else if(centre > right && centre > left)
	{drive_robot(0.25,0.0);}

    else if(centre == 0  && right == 0 && left == 0)
	{drive_robot(0.0,0.0);}
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
