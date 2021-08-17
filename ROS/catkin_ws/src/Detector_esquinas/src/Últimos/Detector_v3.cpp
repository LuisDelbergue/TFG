#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "std_msgs/Float32MultiArray.h"
#include "sensor_msgs/CameraInfo.h"

static const std::string OPENCV_WINDOW = "Image window";

std_msgs::Float32MultiArray corners;

class ImageConverter
{
  ros::NodeHandle nh_;
  image_transport::ImageTransport it_;
  image_transport::Subscriber image_sub_;
  image_transport::Publisher image_pub_;
  ros::Publisher pub;
  ros::Subscriber sub;
  float ancho,alto,Cx,Cy,Fx,Fy;

public:
  ImageConverter()
    : it_(nh_)
  {
    // Subscrive to input video feed and publish output video feed
    image_sub_ = it_.subscribe("/iris/usb_cam/image_raw", 1,&ImageConverter::imageCb, this);
    image_pub_ = it_.advertise("/image_converter/output_video", 1);
    pub = nh_.advertise<std_msgs::Float32MultiArray>("Corners_ibvs",1000);
    sub = nh_.subscribe("/iris/usb_cam/camera_info", 1000,&ImageConverter::CameraInfo, this);

    //sensor_msgs::CameraInfo characteristic;
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

  if (contours.size()>0){

    int min;
    int area_min;
    bool bandera=0;

    //Bucle por cada contorno que detectamos
    for (int i = 0; i < contours.size(); i++)
    {
      //Calculamos el perimetro para hacer una aproximacion de los vértices y el encuadre
      float peri = cv::arcLength(contours[i], true);
      cv::approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
      boundRect[i] = cv::boundingRect(conPoly[i]);
      
      int objCor = (int)conPoly[i].size();
      
      int area = cv::contourArea(contours[i]);

      //Si tienes más de 11 vértices
      if((objCor>11)&&(area>350))
      {
        if(bandera==0)
        {
          bandera=1;
          area_min=area;
        }
        else if(area<area_min)
        {
          area_min=area;
        }  
        min=i;
      } 
    }
    if (bandera)
    {
      //Calculamos el perimetro para hacer una aproximacion de los vértices y el encuadre
      float peri = cv::arcLength(contours[min], true);
      cv::approxPolyDP(contours[min], conPoly[min], 0.02 * peri, true);
      boundRect[min] = cv::boundingRect(conPoly[min]);
      //Dibujamos el contorno con cada vertice y luego dibujamos el rectángulo que encuadra
      cv::rectangle(img, boundRect[min].tl(), boundRect[min].br(), cv::Scalar(0, 255, 0), 2);
      
      corners.data.clear();
      corners.data.push_back(boundRect[min].tl().x/ancho);
      corners.data.push_back(boundRect[min].tl().y/alto);
      corners.data.push_back(boundRect[min].br().x/ancho);
      corners.data.push_back(boundRect[min].br().y/alto);
      corners.data.push_back(Cx);
      corners.data.push_back(Cy);
      corners.data.push_back(Fx);
      corners.data.push_back(Fy);

      std::cout<<boundRect[min].tl().x<<std::endl;
      std::cout<<ancho<<std::endl;
    }
  }
}

  void CameraInfo (const sensor_msgs::CameraInfo& cam){
    ancho=cam.height;
    alto=cam.width;
    Cx = cam.K[2];
    Cy = cam.K[5];
    Fx = cam.K[0];
    Fy = cam.K[4];
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
    cv::Mat imgHSV, mask,imgDil;

    cvtColor(img, imgHSV, cv::COLOR_BGR2HSV);

    cv::Scalar lower(0,0,0);   //(0,0,0)    (0,0,81)  (0,0,53-90)    (0,0,0)  
    cv::Scalar upper(0,0,45); //(0,0,45-55) (0,0,255) (0,0,152-160)   (0,0224)
    cv::inRange(imgHSV, lower, upper, mask);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(8, 8));
    dilate(mask, imgDil, kernel);

    getContours(imgDil,cv_ptr->image);

    //Rectángulo posición final
    cv::rectangle(cv_ptr->image, cv::Point(171,58),cv::Point(468,422), cv::Scalar(255,0,0),2);

    // Update GUI Window
    imshow(OPENCV_WINDOW, cv_ptr->image);
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