/**     2017-12-16
 *      micheal chen    michealchen17@163.com
 * chmod 444 /dev/input/event4
 * change the privilege of keyboard event to 444.
 * this means users can read the keyboard input events
 * or it will report permission denied error.
 * use cat /proc/bus/input/devices to find the input events name
 * https://www.kernel.org/doc/Documentation/input/event-codes.txt
 **/
/**This program can generate velocity command.
 * press arrow_up means linear speed increase in x direction
 * press arrow_down means linear speed decrease in x direction
 * press arrow_left means angular speed increase in z dirction
 * press arrow_right means angular speed decrease in z dirction
 *
 * topic publish
 * --kb_teleop  sensor_msgs::joy
 *
 **/
#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <linux/input.h>
//#include <iostream>             //just for debug.


#define KEY_ARROW_UP        103
#define KEY_ARROW_DOWN      108
#define KEY_ARROW_LEFT      105
#define KEY_ARROW_RIGHT     106


int main(int argc,char **argv)
{
    ros::init(argc, argv, "kb_teleop");
    ros::NodeHandle nh;
    ros::Publisher pub = nh.advertise<sensor_msgs::Joy>("/kb_teleop", 1);
    ros::Rate loop_rate(100);
    FILE *fp;
    struct input_event kbEvn;
    fp = fopen("/dev/input/event4", "r");
    if(fp==NULL)
    {
        ROS_DEBUG("keyboard device read denied!");
        exit(1);
    }
    int kb_btn[4]={0,0,0,0};
    unsigned int msg_seq = 0;
    while(ros::ok())
    {
        sensor_msgs::Joy msg;
        fread((void *)&kbEvn, sizeof(kbEvn), 1, fp);
        if(ferror(fp))
        {
            ROS_DEBUG("keyboard events read failed!");
            exit(1);
        }
        if(kbEvn.type==1 && (kbEvn.value==0 || kbEvn.value==1))
        {
            switch (kbEvn.code) {
            case KEY_ARROW_UP:
                kb_btn[0] = kbEvn.value;
                break;
            case KEY_ARROW_DOWN:
                kb_btn[1]=kbEvn.value;
                break;
            case KEY_ARROW_LEFT:
                kb_btn[2]=kbEvn.value;
                break;
            case KEY_ARROW_RIGHT:
                kb_btn[3]=kbEvn.value;
                break;
            default:
                break;
            }
        }
        msg.header.seq = msg_seq;
        msg.header.stamp = ros::Time::now();
        msg.buttons.push_back(kb_btn[0]);
        msg.buttons.push_back(kb_btn[1]);
        msg.buttons.push_back(kb_btn[2]);
        msg.buttons.push_back(kb_btn[3]);
        pub.publish(msg);
        ros::spinOnce();
        loop_rate.sleep();
        msg_seq++;
    }
    return 0;
}


