#include <ros/ros.h>
#include <geometry_msgs/Twist.h>

int main(int argc, char** argv) {
    ros::init(argc, argv, "my_motion");
    ros::NodeHandle n;
    ros::Publisher twist_pub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 1000);
    ros::Rate loop_rate(0.5);
    int state = 0; // 0 for straight-through, 1 for right-turning
    while (ros::ok())
    {
        geometry_msgs::Twist msg;
        memset(&msg, 0, sizeof(geometry_msgs::Twist));
        switch (state) {
        case 0:
            msg.linear.x = 1.0;
            // state = 1;
            ROS_INFO("will go straight...");
            break;
        case 1:
            msg.angular.z = 1.0;
            // state = 0;
            ROS_INFO("will turn right...");
            break;
        }
        twist_pub.publish(msg);
        ros::spinOnce();
        loop_rate.sleep();
    }
    return 0;
}