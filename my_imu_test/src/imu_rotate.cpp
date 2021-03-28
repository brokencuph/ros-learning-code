#include <cstdio>
#include <thread>
#include <cmath>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Quaternion.h>
#include <nav_msgs/Odometry.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>


double accum = 0.0;
double target;

bool first_call = false;
bool target_reached = false;

void odom_handler(const nav_msgs::Odometry::ConstPtr& msg) {
    static double last_y = -INFINITY;
    if (target_reached) {
        return;
    }
    const auto& qua_msg = msg->pose.pose.orientation;
    tf2::Quaternion tf_qua;
    tf2::fromMsg(qua_msg, tf_qua);
    tf2::Matrix3x3 qua_mat(tf_qua);
    double r, p, y;
    qua_mat.getRPY(r, p, y);
    ROS_INFO("New Orientation: %f, %f, %f", r, p, y);
    if (first_call) {
        accum = -y;
        first_call = false;
    }
    if (y < last_y) {
        accum += 6.28;
    }
    last_y = y;
    if (y + accum >= target) {
        ROS_INFO("target reached");
        target_reached = true;
        return;
    }
}

void send_rotate(ros::NodeHandle* nh) {
    ros::Publisher pub = nh->advertise<geometry_msgs::Twist>("/cmd_vel", 1000);
    ros::Rate rate(1000);
    geometry_msgs::Twist msg;
    memset(&msg, 0, sizeof(msg));
    msg.angular.z = 1.0;
    while (ros::ok() && !target_reached) {
        pub.publish(msg);
        ros::spinOnce();
        rate.sleep();
    }
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "my_imu_rotater");
    if (argc < 2) {
        printf("Error: target radian needed\n");
        return 1;
    }
    double tar_rad;
    if (sscanf(argv[1], "%lf", &tar_rad) != 1) {
        printf("Error: target radian needed\n");
        return 1;
    }
    ros::NodeHandle n;
    target = tar_rad;
    target_reached = false;
    first_call = true;
    ROS_INFO("going to rad %f", target);
    ros::Subscriber sub = n.subscribe("/odom", 1000, odom_handler);
    std::thread rot_thr(send_rotate, &n);
    rot_thr.join();
    return 0;
}