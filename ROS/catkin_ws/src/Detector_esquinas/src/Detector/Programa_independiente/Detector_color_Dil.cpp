#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(){

string path = "/home/ladch/Escritorio/Universidad/Programación/TFG/ROS/catkin_ws/src/Detector_esquinas/src/Fotos/Foto4_perspectiva_lejos.png";
Mat img = imread(path);
Mat imgHSV, mask,imgDil,Kernel;
int hmin = 0, smin = 110, vmin = 153;
int hmax = 19, smax = 240, vmax = 255;

cvtColor(img, imgHSV, COLOR_BGR2HSV);

namedWindow("Trackbars", (640, 200));
createTrackbar("Hue Min", "Trackbars", &hmin, 179);
createTrackbar("Hue Max", "Trackbars", &hmax, 179);
createTrackbar("Sat Min", "Trackbars", &smin, 255);
createTrackbar("Sat Max", "Trackbars", &smax, 255);
createTrackbar("Val Min", "Trackbars", &vmin, 255);
createTrackbar("Val Max", "Trackbars", &vmax, 255);

while (true) {

Scalar lower(hmin, smin, vmin);
Scalar upper(hmax, smax, vmax);
inRange(imgHSV, lower, upper, mask);

Mat kernel = getStructuringElement(MORPH_RECT, Size(4, 4));
dilate(mask, imgDil, kernel);

imshow("Image", img);
imshow("Image HSV", imgHSV);
imshow("Image Mask", mask);
imshow("Image DIL", imgDil);
waitKey(1);
}
return 0;
}