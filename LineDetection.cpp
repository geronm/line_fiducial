/**
 * Line detection for fiducial reconstruction.
 *
 * Original code from online opencv example http://docs.opencv.org/2.4/doc/tutorials/imgproc/imgtrans/hough_lines/hough_lines.html
 * Accessed 31 Jan 2017.
 *
 */

#include "yaml-cpp/yaml.h"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <fstream>  // nice number I/O

using namespace cv;
using namespace std;

void help()
{
 cout << "\nThis program demonstrates line finding with the Hough transform.\n"
         "Usage:\n"
         "./houghlines <image_name>, Default is pic1.jpg\n" << endl;
}

int main(int argc, char** argv)
{
  if (argc < 2) {
    cout << "Usage: line_detection vid_src.mp4 config.yaml" << endl;
    return 1;
  }


  const char* rawSource = argc >= 2 ? argv[1] : "pic1.jpg";
  const char* configSource = argc >= 3 ? argv[2] : "config/defaultConfig.cfg";

  YAML::Node config = YAML::LoadFile(configSource);

  // YAML::Node shape = config["shape"];
  // YAML::Node vertices = config["vertices"];

  // int NUM_VERTICES = (vertices.as<string>()).length();
  int NUM_VERTICES = 6;

  int roi[4][2];
  for (int i=0; i<4; i++) {
    for (int j=0; j<2; j++) {
      roi[i][j] = config["roi"][i][j].as<int>();
    }
  }

  printf("Num vertices: %d", NUM_VERTICES);

  VideoCapture captureRaw;
  captureRaw.open(rawSource);

  bool visualize = true;
  if (visualize){
    namedWindow( "LineImage", cv::WINDOW_NORMAL );
    startWindowThread();
  }

  Mat RawImage;

  captureRaw >> RawImage;

  if (RawImage.empty()) {
    cout << "Error: Input images stream empty." << endl;
    return 1;
  }

  Mat src;

  while(!RawImage.empty()) {

    captureRaw >> src;

    /*  Mat src = imread(filename, 0);
    if(src.empty())
    {
      help();
      cout << "can not open " << filename << endl;
      return -1;
    } */

    Mat dst, cdst;
    Canny(src, dst, 50, 200, 3);
    cvtColor(dst, cdst, CV_GRAY2BGR);

    #if 0
      vector<Vec2f> lines;
      HoughLines(dst, lines, 1, CV_PI/180, 100, 0, 0 );

      for( size_t i = 0; i < lines.size(); i++ )
      {
        float rho = lines[i][0], theta = lines[i][1];
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = cvRound(x0 + 1000*(-b));
        pt1.y = cvRound(y0 + 1000*(a));
        pt2.x = cvRound(x0 - 1000*(-b));
        pt2.y = cvRound(y0 - 1000*(a));
        line( cdst, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
      }
    #else
      vector<Vec4i> lines;
      HoughLinesP(dst, lines, 1, CV_PI/180, 50, 50, 10 );
      for( size_t i = 0; i < lines.size(); i++ )
      {
        Vec4i l = lines[i];
        line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
      }
      line( cdst, Point(roi[0][0],roi[0][1]), Point(roi[1][0],roi[1][1]), Scalar(0,255,255), 3, CV_AA);
      line( cdst, Point(roi[1][0],roi[1][1]), Point(roi[2][0],roi[2][1]), Scalar(0,255,255), 3, CV_AA);
      line( cdst, Point(roi[2][0],roi[2][1]), Point(roi[3][0],roi[3][1]), Scalar(0,255,255), 3, CV_AA);
      line( cdst, Point(roi[3][0],roi[3][1]), Point(roi[0][0],roi[0][1]), Scalar(0,255,255), 3, CV_AA);
    #endif
    //imshow("source", src);
    //imshow("detected lines", cdst);
    cv::imshow("LineImage", cdst);

  }

  return 0;
}

