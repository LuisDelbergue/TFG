#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;
void getContours(Mat mask, Mat img){

  //Declaramos un vector de vectores donde van los puntos del contorno y otro de la jerarquía
  vector<vector<Point>> contours;
  vector<Vec4i> hierarchy;

findContours(mask, contours, hierarchy, RETR_LIST , CHAIN_APPROX_SIMPLE);

  //Declaramos los vértices del contorno y el encuadramiento
  vector<vector<Point>> conPoly(contours.size());
  vector<Rect> boundRect(contours.size());

  if (contours.size()>0){

    int min=0;
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
      //Calculamos el área de cada contorno
      int area = cv::contourArea(contours[i]);

      //Si tienes más de 11 vértices
      if((objCor>11)&&(area>1000))
      {
        if(bandera==0)
        {
          bandera=1;
          area_min=area;
          min=i;
        }
        if(area<area_min)
        {
          area_min=area;
          min=i;
        }  
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
    }
  }
}


int main(){

string path = "/home/ladch/Escritorio/Universidad/Programación/TFG/ROS/catkin_ws/src/Detector_esquinas/src/Detector/Programa independiente/Fotos/Foto2_lejos.png";
Mat imgSmall = imread(path);
Mat img, imgHSV, mask,imgDil;

resize(imgSmall, img, Size(),2,2);

cvtColor(img, imgHSV, COLOR_BGR2HSV);

Scalar lower(0,0,0);   //(0,0,0)    (0,0,81)  (0,0,53-90)    (0,0,0)  
Scalar upper(0,0,45); //(0,0,45-55) (0,0,255) (0,0,152-160)   (0,0224)
inRange(imgHSV, lower, upper, mask);

Mat kernel = getStructuringElement(MORPH_RECT, Size(8, 8));
dilate(mask, imgDil, kernel);

getContours(imgDil,img);

imshow("Image", img);
imshow("Image DIL", imgDil);
//imshow("Image Mask", mask);
waitKey(0);

return 0;
}