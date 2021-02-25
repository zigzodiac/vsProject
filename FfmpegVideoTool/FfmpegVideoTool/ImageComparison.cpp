#include "ImageComparison.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <libavutil/pixfmt.h>


#define RATIO    0.4

using namespace std;
using namespace cv;

int PictureShow()
{
    //OpenCV版本号
    cout << "OpenCV_Version: " << CV_VERSION << endl;

    //读取图片
    Mat img = imread("D:/TestVideo/course/6BJr1EQpbKsA_20210127171442872/6BJr1EQpbKsA_2021012717144287220210127171442872.jpg");

    imshow("picture", img);
    waitKey(0);
    return 0;
}

int PicCompare(uint8_t* out_buffer, int height, int width, AVPixelFormat fmt) {

    cv::Mat* pCvMat1 = new cv::Mat();
    cv::Mat* pCvMat2 = new cv::Mat();
    pCvMat1->create(cv::Size(width, height), CV_8UC3);
    pCvMat2->create(cv::Size(width, height), CV_8UC3);
    memcpy(pCvMat1->data, out_buffer, numBytes);
    Mat box = imread("2.png");
    Mat scene = imread("数字.jpg");
    if (scene.empty()) {
        printf("could not load image...\n");
        return -1;
    }
    imshow("input image", scene);
    vector<KeyPoint> keypoints_obj, keypoints_sence;
    Mat descriptors_box, descriptors_sence;
    Ptr<ORB> detector = ORB::create();
    detector->detectAndCompute(scene, Mat(), keypoints_sence, descriptors_sence);
    detector->detectAndCompute(box, Mat(), keypoints_obj, descriptors_box);
    vector<DMatch> matches;
    // 初始化flann匹配
    // Ptr<FlannBasedMatcher> matcher = FlannBasedMatcher::create(); // default is bad, using local sensitive hash(LSH)
    Ptr<DescriptorMatcher> matcher = makePtr<FlannBasedMatcher>(makePtr<flann::LshIndexParams>(12, 20, 2));
    matcher->match(descriptors_box, descriptors_sence, matches);
    // 发现匹配
    vector<DMatch> goodMatches;
    printf("total match points : %d\n", matches.size());
    float maxdist = 0;
    for (unsigned int i = 0; i < matches.size(); ++i) {
        printf("dist : %.2f \n", matches[i].distance);
        maxdist = max(maxdist, matches[i].distance);
    }
    for (unsigned int i = 0; i < matches.size(); ++i) {
          if (matches[i].distance < maxdist * RATIO)
              goodMatches.push_back(matches[i]);
    }
    double similar = goodMatches.size() / matches.size();
    printf("sililar is %f", similar);
    Mat dst;
    drawMatches(box, keypoints_obj, scene, keypoints_sence, goodMatches, dst);
    imshow("output", dst);
    waitKey(0);
    return 0;
}
