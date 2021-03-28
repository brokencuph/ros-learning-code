#include <ros/ros.h>
#include <geometry_msgs/Quaternion.h>
#include <sensor_msgs/Imu.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>

void imu_callback(const sensor_msgs::Imu::ConstPtr& msg) {
    tf2::Quaternion tf_qua;
    tf2::fromMsg(msg->orientation, tf_qua);
    tf2::Matrix3x3 qua_mat(tf_qua);
    double r, p, y;
    qua_mat.getRPY(r, p, y);
    ROS_INFO("Interpreted: r=%f, p=%f, y=%f", r, p, y);
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "my_imu_listener");
    ros::NodeHandle n;
    ros::Subscriber sub = n.subscribe("/e100_imu", 1000, imu_callback);
    ros::spin();
    return 0;
}