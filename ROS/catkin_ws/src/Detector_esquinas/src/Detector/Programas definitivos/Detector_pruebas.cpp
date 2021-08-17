#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "geometry_msgs/Quaternion.h"

static const std::string OPENCV_WINDOW = "Image window";

//float x1,y1,x2,y2;
geometry_msgs::Quaternion corners;

class ImageConverter
{
  ros::NodeHandle nh_;
  image_transport::ImageTransport it_;
  image_transport::Subscriber image_sub_;
  image_transport::Publisher image_pub_;
  ros::Publisher pub;

  // ros::NodeHandle n;
  // ros::Publisher pub = n.advertise<geometry_msgs::Quaternion>("send_corners",1000);
  // ros::Rate loop_rate(10);
  // int count=0;
  // while (ros::ok())
  // {
  //   std::cout<<corners.x << corners.y<<std::endl;
  //   pub.publish(corners);
  //   ros::spinOnce;
  //   loop_rate.sleep();
  //   ++count;
  // }

public:
  ImageConverter()
    : it_(nh_)
  {
    // Subscrive to input video feed and publish output video feed
    image_sub_ = it_.subscribe("/iris/usb_cam/image_raw", 1,
      &ImageConverter::imageCb, this);
    image_pub_ = it_.advertise("/image_converter/output_video", 1);
    pub = nh_.advertise<geometry_msgs::Quaternion>("Corners_ibvs",1000);

    cv::namedWindow(OPENCV_WINDOW);
  }

  ~ImageConverter()
  {
    cv::destroyWindow(OPENCV_WINDOW);
  }

  void imageCb(const sensor_msgs::ImageConstPtr& msg)
  {
    cv_bridge::CvImagePtr cv_ptr;
    try
    {
      cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    }
    catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("cv_bridge exception: %s", e.what());
      return;
    }

      cv::Mat img = cv_ptr->image;
      cv::Mat imgGray, imgBlur, imgCanny, imgDil;

      //Preproceso: gris, borroso, contorno y dilatamos los bordes
      cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);
      GaussianBlur(imgGray, imgBlur, cv::Size(3, 3), 3, 0);
      Canny(imgBlur, imgCanny, 25, 75);
      cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
      dilate(imgCanny, imgDil, kernel);

    //Rectángulo posición final
    cv::rectangle(cv_ptr->image, cv::Point(171,422),cv::Point(468,58), cv::Scalar(255,0,0),2);

    // Update GUI Window
    cv::imshow(OPENCV_WINDOW, cv_ptr->image);
    cv::waitKey(3);

    // Output modified video stream
    image_pub_.publish(cv_ptr->toImageMsg());

      corners.x=120;
      corners.y=0;
      corners.z=20;
      corners.w=0;

    //Publico esquinas
    pub.publish(corners);
  }
};

int main(int argc, char** argv)
{
  ros::init(argc, argv, "image_converter");
  ImageConverter ic;
  ros::spin();
  return 0;
}