//LIBRERIAS Y NAMESPACES
#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "std_msgs/Float32MultiArray.h"
#include "sensor_msgs/CameraInfo.h"

//VARIABLES DE LA CÁMARA Y EL VECTOR QUE SE MANDA (ESUINAS Y CARACTERISTICAS IMAGEN)
static const std::string OPENCV_WINDOW = "Image window";
std_msgs::Float32MultiArray corners;

//CLASE CON LAS VARIABLES Y FUNCIONES PARA PASAR AL MAIN
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
    //SUBSCRIBERS Y PUBLISHERS DEL VÍDEO
    image_sub_ = it_.subscribe("/iris/usb_cam/image_raw", 1,&ImageConverter::imageCb, this);
    image_pub_ = it_.advertise("/image_converter/output_video", 1);

    //SUBSCRIBERS DE LAS CARACTERÍSTICAS IMAGEN Y PUBLISHERS DEL VECTOR
    sub = nh_.subscribe("/iris/usb_cam/camera_info", 1000,&ImageConverter::CameraInfo, this);
    pub = nh_.advertise<std_msgs::Float32MultiArray>("Corners_ibvs",1000);

    //CONSTRUCTOR DEL DISPLAY
    cv::namedWindow(OPENCV_WINDOW);
  }

  //DESTRUCTOR DEL DISPLAY
  ~ImageConverter()
  {
    cv::destroyWindow(OPENCV_WINDOW);
  }


void getContours(cv::Mat imgDil, cv::Mat img){

  //DECLARAR VECTOR DE VECTORES CON LOS PUNTOS DEL CONTORNO Y LA JERARQUÍA
  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> hierarchy;

  //FUNCION QUE APROXIMA LOS BORDES
  cv::findContours(imgDil, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

  //DECLARAR LOS VERTICES DEL CONTORNO Y EL ENCUADRAMIENTO
  std::vector<std::vector<cv::Point>> conPoly(contours.size());
  std::vector<cv::Rect> boundRect(contours.size());

  if (contours.size()>0) //SI HAY ALGÚN CONTORNO
  {
    int min; //INDICE CON EL AREA MINIMA
    int area_min; //MENOR AREA
    bool bandera=0; //BANDERA PARA INICIALIZAR EL MINIMO

    //BUCLE POR CADA CONTORNO QUE DETECTAMOS
    for (int i = 0; i < contours.size(); i++)
    {
      //CALCULAR EL PERIMETRO PARA HACER UNA APROXIMACION DE LOS VERTICES Y EL ENCUADRE
      float peri = cv::arcLength(contours[i], true);
      cv::approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
      boundRect[i] = cv::boundingRect(conPoly[i]);
      
      //NUMERO DE VERTICES Y AREA
      int objCor = (int)conPoly[i].size();      
      int area = cv::contourArea(contours[i]);

      if((objCor>11)&&(area>350)) //SI TIENE MAS DE 11 VERTICES Y SU AREA ES MAYOR DE 350
      {
        if(bandera==0)  //SI ENTRA POR PRIMERA VEZ
        {
          bandera=1;
          area_min=area;
        }
        else if(area<area_min) //PONER EL MENOR AREA
        {
          area_min=area;
        }  
        min=i;
      } 
    }
    if (bandera) //SI HA ENTRADO ALGUNA VEZ
    {
      //CALCULAR EL PERIMETRO PARA HACER UNA APROXIMACION DE LOS VERTICES Y EL ENCUADRE
      float peri = cv::arcLength(contours[min], true);
      cv::approxPolyDP(contours[min], conPoly[min], 0.02 * peri, true);
      boundRect[min] = cv::boundingRect(conPoly[min]);

      //DIBUJAR EL RECTANGULO QUE ENCUADRA
      cv::rectangle(img, boundRect[min].tl(), boundRect[min].br(), cv::Scalar(0, 255, 0), 2);
      
      //LIMPIAR CADA VEZ QUE LLENA EL VECTOR
      corners.data.clear();
      //VALOR ESQUINAS TOPLEFT Y BOTTONRIGHT EN PIXELES
      corners.data.push_back(boundRect[min].tl().x);
      corners.data.push_back(boundRect[min].tl().y);
      corners.data.push_back(boundRect[min].br().x);
      corners.data.push_back(boundRect[min].br().y);
      //CARACTERÍSTICA IMAGEN
      corners.data.push_back(Cx);
      corners.data.push_back(Cy);
      corners.data.push_back(Fx);
      corners.data.push_back(Fy);
      corners.data.push_back(ancho);
      corners.data.push_back(alto);

      //std::cout<<boundRect[min].tl().x<<std::endl;

    }
  }
}

  //SUBSCRIBER: LEER LA CARACTERÍSTICAS DE LA IMAGEN
  void CameraInfo (const sensor_msgs::CameraInfo& cam){
    alto=cam.height;
    ancho=cam.width;
    Cx = cam.K[2];
    Cy = cam.K[5];
    Fx = cam.K[0];
    Fy = cam.K[4];
  }

  //SUBSCRIBER A LA CÁMARA
  void imageCb(const sensor_msgs::ImageConstPtr& msg)
  {
    //PUENTE ROS-OPENCV
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

    //VARIABLES DE LA IMAGEN
    cv::Mat img = cv_ptr->image;
    cv::Mat imgHSV, mask,imgDil;

    //FILTRO COLOR
    cvtColor(img, imgHSV, cv::COLOR_BGR2HSV);
    cv::Scalar lower(0,0,0);   //(0,0,0)    (0,0,81)  (0,0,53-90)    (0,0,0)  
    cv::Scalar upper(0,0,45); //(0,0,45-55) (0,0,255) (0,0,152-160)   (0,0224)
    cv::inRange(imgHSV, lower, upper, mask);

    //DILATA BORDES
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(8, 8));
    dilate(mask, imgDil, kernel);

    //OBTIENE DE LA IMAGEN DILATADA Y DIBUJA CONTORNO DE LA H EN EL DISPLAY
    getContours(imgDil,cv_ptr->image);

    //RECTANGULO DE LA POSICIÓN FINAL
    cv::rectangle(cv_ptr->image, cv::Point(171,58),cv::Point(468,422), cv::Scalar(255,0,0),2);

    //ACTUALIZA EL DISPLAY DE LA CÁMARA
    imshow(OPENCV_WINDOW, cv_ptr->image);
    cv::waitKey(3);

    //PUBLICO EL VÍDEO MODIFICADO
    image_pub_.publish(cv_ptr->toImageMsg());

    //PUBLICO VECTOR
    pub.publish(corners);
  }
};

int main(int argc, char** argv)
{
  //INICIALIZA EL NODO
  ros::init(argc, argv, "image_converter");
  ImageConverter ic;

  ros::spin();
  return 0;
}