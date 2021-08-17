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

void getContours(cv::Mat imgDil, cv::Mat img){

  //Declaramos un vector de vectores donde van los puntos del contorno y otro de la jerarquía
  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> hierarchy;

  //Función que aproxima los bordes
  cv::findContours(imgDil, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

  //Declaramos los vértices del contorno y el encuadramiento
  std::vector<std::vector<cv::Point>> conPoly(contours.size());
  std::vector<cv::Rect> boundRect(contours.size());

  //Bucle por cada contorno que detectamos
  for (int i = 0; i < contours.size(); i++)
  {
    //Calculamos el perimetro para hacer una aproximacion de los vértices y el encuadre
    float peri = cv::arcLength(contours[i], true);
    cv::approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
    boundRect[i] = cv::boundingRect(conPoly[i]);

    //Contamos los vértices
    int objCor = (int)conPoly[i].size();

    //Si tienes más de 11 vértices
    if(objCor>11)
    { 
      //Dibujamos el rectángulo que encuadra
      cv::rectangle(img, boundRect[i].tl(), boundRect[i].br(), cv::Scalar(0, 255, 0), 2);
      
      corners.x=boundRect[i].tl().x;
      corners.y=boundRect[i].tl().y;
      corners.z=boundRect[i].br().x;
      corners.w=boundRect[i].br().y;
      // x1=boundRect[i].tl().x; 
      // y1=boundRect[i].tl().y;
      // x2=boundRect[i].br().x;
      // y2=boundRect[i].br().y;

    } 
  }
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

      //Función coge bordes, encuadra H
      getContours(imgDil,cv_ptr->image);

    //Rectángulo posición final
    cv::rectangle(cv_ptr->image, cv::Point(171,58),cv::Point(468,422), cv::Scalar(255,0,0),2);

    // Update GUI Window
    cv::imshow(OPENCV_WINDOW, cv_ptr->image);
    cv::waitKey(3);

    // Output modified video stream
    image_pub_.publish(cv_ptr->toImageMsg());
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