//Librerías necesarias
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

//Namespaces para no poner cv::Mat y más
  using namespace cv;
  using namespace std;

//Pasamos la imágenes con contornos dilatados y la final
void getContours(Mat imgDil, Mat img){

  //Declaramos un vector de vectores donde van los puntos del contorno y otro de la jerarquía
  vector<vector<Point>> contours;
  vector<Vec4i> hierarchy;
  
  //Función que aproxima los bordes
  findContours(imgDil, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);

  //Declaramos los vértices del contorno y el encuadramiento
  vector<vector<Point>> conPoly(contours.size());
  vector<Rect> boundRect(contours.size());
  int max;
  int area_max=0;

  //Bucle por cada contorno que detectamos
  for (int i = 0; i < contours.size(); i++)
  {
   
    //Calculamos el perimetro para hacer una aproximacion de los vértices y el encuadre
    float peri = arcLength(contours[i], true);
    approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
    boundRect[i] = boundingRect(conPoly[i]);

    int objCor = (int)conPoly[i].size();
    cout << objCor << endl;

    if(objCor>11){
          //Calculamos el área de cada contorno
          int area = contourArea(contours[i]);
          cout << area << endl;
          if(area>area_max)
          {
            area_max=area;
            max=i;
          }
    }
  }
    float peri = arcLength(contours[max], true);
    approxPolyDP(contours[max], conPoly[max], 0.02 * peri, true);
    boundRect[max] = boundingRect(conPoly[max]);
    //Dibujamos el contorno con cada vertice y luego dibujamos el rectángulo que encuadra
    rectangle(img, boundRect[max].tl(), boundRect[max].br(), Scalar(0, 255, 0), 2);
}

  int main() {

      //Declaro la direccion de la foto, la leo y declaro el resto de transformaciones
      string path = "/home/ladch/Escritorio/Universidad/Programación/TFG/ROS/catkin_ws/src/Detector_esquinas/src/Fotos/Foto1_cerca.png";
      Mat imgSmall = imread(path);
      Mat img,imgGray, imgBlur, imgCanny, imgDil;

      resize(imgSmall, img, Size(),1.5,1.5);

      //Preproceso: gris, borroso, contorno y dilatamos los bordes
      cvtColor(img, imgGray, COLOR_BGR2GRAY);
      GaussianBlur(imgGray, imgBlur, Size(3, 3), 3, 0);
      Canny(imgBlur, imgCanny, 25, 75);
      Mat kernel = getStructuringElement(MORPH_RECT, Size(2, 2));
      dilate(imgCanny, imgDil, kernel);

      //Función coge bordes, encuadra H y pone círculos esquinas
      getContours(imgDil,img);

      //Muestra las imágenes
      imshow("Image", img);
      waitKey(0);

      return 0;
  }