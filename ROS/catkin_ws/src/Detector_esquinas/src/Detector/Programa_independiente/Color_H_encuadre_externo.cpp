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

findContours(mask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

  //Declaramos los vértices del contorno y el encuadramiento
  vector<vector<Point>> conPoly(contours.size());
  vector<Rect> boundRect(contours.size());

  //Bucle por cada contorno que detectamos
  for (int i = 0; i < contours.size(); i++)
  {
    //Calculamos el área de cada contorno
    int area = contourArea(contours[i]);
      cout << area << endl;

      //Calculamos el perimetro para hacer una aproximacion de los vértices y el encuadre
      float peri = arcLength(contours[i], true);
      approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
      boundRect[i] = boundingRect(conPoly[i]);

      //Dibujamos el contorno con cada vertice y luego dibujamos el rectángulo que encuadra
      drawContours(img, conPoly, i, Scalar(255, 0, 255), 2);
      rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 2);
  }
}


int main(){

string path = "/home/ladch/Escritorio/Universidad/Programación/TFG/ROS/catkin_ws/src/Detector_esquinas/src/Fotos/Foto2_lejos.png";
Mat imgSmall = imread(path);
Mat img, imgHSV, mask;

resize(imgSmall, img, Size(),2.5,2.5);

cvtColor(img, imgHSV, COLOR_BGR2HSV);

Scalar lower(0,0,0);   //(0,0,0)  (0,0,81)  (0,0,90)  triangulo test (8,96,140)
Scalar upper(0,0,45); //(0,0,45) (0,0,255) (0,0,160) triangulo test (62,255,255)
inRange(imgHSV, lower, upper, mask);

getContours(mask,img);

imshow("Image", img);
imshow("Image HSV", imgHSV);
imshow("Image Mask", mask);
waitKey(0);

return 0;
}