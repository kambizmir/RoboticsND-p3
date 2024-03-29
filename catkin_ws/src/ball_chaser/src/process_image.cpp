#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the comman_robot service to drive the robot in the
// specified direction
void drive_robot(float lin_x, float ang_z){
  // TODO: Request a service and pass the velocities to it to drive the robot
  ball_chaser::DriveToTarget srv;
  srv.request.linear_x = lin_x;
  srv.request.angular_z = ang_z;
  client.call(srv);
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img){

  // TODO: Loop through each pixel in the image and check if there's a bright white one
  // Then, identify if this pixel falls in the left, mid, or right side of the image
  // Depending on the white ball position, call the drive_bot function and pass velocities to it
  // Request a stop when there's no white ball seen by the camera

  int white_count = 0;
  int total_pixel_x = 0;

  for (int i=0; i+2<img.data.size(); i+=3) {
    if (img.data[i] + img.data[i+1] + img.data[i+2]  == 3 * 255){
      int pixel_x = (i % (img.width * 3)) / 3;
      total_pixel_x += pixel_x;
      white_count ++;
    }
  }

  if(white_count > 0){
    int center_pixel_x = total_pixel_x / white_count;
    if (center_pixel_x < img.width * .33){
      drive_robot(.5, 0.5);
    }
    else if (center_pixel_x > img.width * .66){
      drive_robot(.5, -0.5);
    }
    else{
      drive_robot(.5, 0.0);
    }
  }
  else{
    drive_robot(0.0, 0.0);
  }

}

int main(int argc, char** argv){
  // Initialize the process_image node and create a handle to it
  ros::init(argc, argv, "process_image");
  ros::NodeHandle n;

  // Define a client service capable of requesting services from command_robot
  client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

  // Subscribe to /camera/rgb/image_raw topic to read the image data inside the
  // process_image_callback function
  ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

  // Handle ROS communication events
  ros::spin();

  return 0;
}