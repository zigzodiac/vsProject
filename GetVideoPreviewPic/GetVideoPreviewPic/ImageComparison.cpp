#include "ImageComparison.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgcodecs/legacy/constants_c.h>


#define RATIO    0.4

using namespace std;
using namespace cv;
static cv::Mat* img_1;
static cv::Mat* img_2;
int InitMat(int width, int height, AVPixelFormat fmt) {
    img_1 = new cv::Mat();
    img_2 = new cv::Mat();
    img_1->create(cv::Size(width, height), CV_8UC3);
    img_2->create(cv::Size(width, height), CV_8UC3);
    return 0;
}
int SetMatData(uint8_t* out_buffer, int height, int width, AVPixelFormat fmt, int size, int flag) {
    if (flag == 1) {
        memcpy(img_1->data, out_buffer, size);

        //imshow("RGB", *img_1);
    }
    else {
        memcpy(img_2->data, out_buffer, size);
        //imshow("RGB", *img_2);
    }
    return 0;
}

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

double PicCompare() {

    std::vector<KeyPoint> keypoints_1, keypoints_2;
    Mat descriptors_1, descriptors_2;
    Ptr<FeatureDetector> detector = ORB::create();
    Ptr<DescriptorExtractor> descriptor = ORB::create();
    // Ptr<FeatureDetector> detector = FeatureDetector::create(detector_name);
    // Ptr<DescriptorExtractor> descriptor = DescriptorExtractor::create(descriptor_name);
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce-Hamming");

    //-- 第一步:检测 Oriented FAST 角点位置
    detector->detect(*img_1, keypoints_1);
    detector->detect(*img_2, keypoints_2);

    //-- 第二步:根据角点位置计算 BRIEF 描述子
    descriptor->compute(*img_1, keypoints_1, descriptors_1);
    descriptor->compute(*img_2, keypoints_2, descriptors_2);

    Mat outimg1;
    drawKeypoints(*img_1, keypoints_1, outimg1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
    imshow("ORB特征点", outimg1);

    //-- 第三步:对两幅图像中的BRIEF描述子进行匹配，使用 Hamming 距离
    //BFMatcher matcher(NORM_HAMMING);
    vector<DMatch> matches;
    matcher->match(descriptors_1, descriptors_2, matches);

    //-- 第四步:匹配点对筛选
    double min_dist = 10000, max_dist = 0;
    //找出所有匹配之间的最小距离和最大距离, 即是最相似的和最不相似的两组点之间的距离
    for (int i = 0; i < descriptors_1.rows; i++)
    {
        double dist = matches[i].distance;
        if (dist < min_dist) min_dist = dist;
        if (dist > max_dist) max_dist = dist;
    }

    // 仅供娱乐的写法
    //min_dist = min_element(matches.begin(), matches.end(), [](const DMatch& m1, const DMatch& m2) {return m1.distance < m2.distance; })->distance;
    //max_dist = max_element(matches.begin(), matches.end(), [](const DMatch& m1, const DMatch& m2) {return m1.distance < m2.distance; })->distance;

    printf("-- Max dist : %f \n", max_dist);
    printf("-- Min dist : %f \n", min_dist);

    //当描述子之间的距离大于两倍的最小距离时,即认为匹配有误.但有时候最小距离会非常小,设置一个经验值30作为下限.
    std::vector< DMatch > good_matches;
    for (int i = 0; i < descriptors_1.rows; i++)
    {
        if (matches[i].distance <= max(2 * min_dist, 30.0))
        {
            good_matches.push_back(matches[i]);
        }
    }

    //-- 第五步:绘制匹配结果
    Mat img_match;
    Mat img_goodmatch;
    drawMatches(*img_1, keypoints_1, *img_2, keypoints_2, matches, img_match);
    drawMatches(*img_1, keypoints_1, *img_2, keypoints_2, good_matches, img_goodmatch);
    imshow("所有匹配点对", img_match);
    imshow("优化后匹配点对", img_goodmatch);
    waitKey(0);
    double sim = good_matches.size() / matches.size();
    printf("-- simliar : %f \n", sim);
    return 0;
}
int PerceptualHash()
{
    cv::Mat matDst1, matDst2;

    cv::resize(*img_1, matDst1, cv::Size(8, 8), 0, 0, cv::INTER_CUBIC);
    cv::resize(*img_2, matDst2, cv::Size(8, 8), 0, 0, cv::INTER_CUBIC);
    //update 20181206 for the bug cvtColor
    cv::Mat temp1 = matDst1;
    cv::Mat temp2 = matDst2;
    cv::cvtColor(temp1, matDst1, CV_BGR2GRAY);
    cv::cvtColor(temp2, matDst2, CV_BGR2GRAY);

    int iAvg1 = 0, iAvg2 = 0;
    int arr1[64], arr2[64];

    for (int i = 0; i < 8; i++)
    {
        uchar* data1 = matDst1.ptr<uchar>(i);
        uchar* data2 = matDst2.ptr<uchar>(i);

        int tmp = i * 8;

        for (int j = 0; j < 8; j++)
        {
            int tmp1 = tmp + j;

            arr1[tmp1] = data1[j] / 4 * 4;
            arr2[tmp1] = data2[j] / 4 * 4;

            iAvg1 += arr1[tmp1];
            iAvg2 += arr2[tmp1];
        }
    }

    iAvg1 /= 64;
    iAvg2 /= 64;

    for (int i = 0; i < 64; i++)
    {
        arr1[i] = (arr1[i] >= iAvg1) ? 1 : 0;
        arr2[i] = (arr2[i] >= iAvg2) ? 1 : 0;
    }

    int iDiffNum = 0;

    for (int i = 0; i < 64; i++)
        if (arr1[i] != arr2[i])
            ++iDiffNum;

    cout << "iDiffNum = " << iDiffNum << endl;

    if (iDiffNum <= 5)
        cout << "two images are very similar!" << endl;
    else if (iDiffNum > 10)
        cout << "they are two different images!" << endl;
    else
        cout << "two image are somewhat similar!" << endl;
    cout << "diff is !" << iDiffNum << endl;
    return iDiffNum;
}

bool compareFacesByHist()
{
    Mat tmpImg;
    //resize(*img_1, tmpImg, Size(orgImg.cols, orgImg.rows));

    //HSV颜色特征模型(色调H,饱和度S，亮度V)
    cvtColor(*img_1, *img_1, COLOR_BGR2HSV);
    cvtColor(*img_2, *img_2, COLOR_BGR2HSV);
    //直方图尺寸设置
    //一个灰度值可以设定一个bins，256个灰度值就可以设定256个bins
    //对应HSV格式,构建二维直方图
    //每个维度的直方图灰度值划分为256块进行统计，也可以使用其他值
    int hBins = 256, sBins = 256;
    int histSize[] = { hBins,sBins };
    //H:0~180, S:0~255,V:0~255
    //H色调取值范围
    float hRanges[] = { 0,180 };
    //S饱和度取值范围
    float sRanges[] = { 0,255 };
    const float* ranges[] = { hRanges,sRanges };
    int channels[] = { 0,1 };//二维直方图
    MatND hist1, hist2;
    calcHist(img_1, 1, channels, Mat(), hist1, 2, histSize, ranges, true, false);
    normalize(hist1, hist1, 0, 1, NORM_MINMAX, -1, Mat());
    calcHist(img_2, 1, channels, Mat(), hist2, 2, histSize, ranges, true, false);
    normalize(hist2, hist2, 0, 1, NORM_MINMAX, -1, Mat());
    double similarityValue = compareHist(hist1, hist2, CV_COMP_CORREL);
    cout << "相似度：" << similarityValue << endl;
    if (similarityValue >= 0.75)
    {
        return true;
    }
    return false;
}
bool compareFacesByHist１()
{
    Mat tmpImg;
    cv::Mat src1 = cv::imread("D:\TestVideo\picture\picture-０.jpg", CV_LOAD_IMAGE_COLOR);
    cv::Mat src2 = cv::imread("D:\TestVideo\picture\picture-1.jpg", CV_LOAD_IMAGE_COLOR);
    //resize(*img_1, tmpImg, Size(orgImg.cols, orgImg.rows));

    //HSV颜色特征模型(色调H,饱和度S，亮度V)
    cvtColor(src1, src1, COLOR_BGR2HSV);
    cvtColor(src2, src2, COLOR_BGR2HSV);
    //直方图尺寸设置
    //一个灰度值可以设定一个bins，256个灰度值就可以设定256个bins
    //对应HSV格式,构建二维直方图
    //每个维度的直方图灰度值划分为256块进行统计，也可以使用其他值
    int hBins = 256, sBins = 256;
    int histSize[] = { hBins,sBins };
    //H:0~180, S:0~255,V:0~255
    //H色调取值范围
    float hRanges[] = { 0,180 };
    //S饱和度取值范围
    float sRanges[] = { 0,255 };
    const float* ranges[] = { hRanges,sRanges };
    int channels[] = { 0,1 };//二维直方图
    MatND hist1, hist2;
    calcHist(&src1, 1, channels, Mat(), hist1, 2, histSize, ranges, true, false);
    normalize(hist1, hist1, 0, 1, NORM_MINMAX, -1, Mat());
    calcHist(&src2, 1, channels, Mat(), hist2, 2, histSize, ranges, true, false);
    normalize(hist2, hist2, 0, 1, NORM_MINMAX, -1, Mat());
    double similarityValue = compareHist(hist1, hist2, CV_COMP_CORREL);
    cout << "相似度：" << similarityValue << endl;
    if (similarityValue >= 0.85)
    {
        return true;
    }
    return false;
}


//pHash算法
int pHashValueCompare()
{
    cv::Mat matDst1, matDst2;
    int diffNum = 0;
    Mat img, dst, img2, dst2;
    string rst(64, '\0');
    string rst2(64, '\0');
    double dIdex[64];
    double dIdex2[64];
    double mean = 0.0;
    double mean2 = 0.0;
    int k = 0, k2 = 0;
    if (img_1->channels() == 3)
    {
        cvtColor(*img_1, matDst1, CV_BGR2GRAY);
        img = Mat_<double>(matDst1);
    }
    else
    {
        img = Mat_<double>(*img_1);
    }
    if (img_2->channels() == 3)
    {
        cvtColor(*img_2, matDst2, CV_BGR2GRAY);
        img2 = Mat_<double>(matDst2);
    }
    else
    {
        img2 = Mat_<double>(*img_2);
    }

    /* 第一步，缩放尺寸*/
    resize(img, img, Size(32, 32));
    resize(img2, img2, Size(32, 32));

    /* 第二步，离散余弦变换，DCT系数求取*/
    dct(img, dst);
    dct(img2, dst2);

    /* 第三步，求取DCT系数均值（左上角8*8区块的DCT系数）*/
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j)
        {
            dIdex[k] = dst.at<double>(i, j);
            mean += dst.at<double>(i, j) / 64;
            ++k;
        }
    }
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j)
        {
            dIdex2[k2] = dst2.at<double>(i, j);
            mean2 += dst2.at<double>(i, j) / 64;
            ++k2;
        }
    }

    /* 第四步，计算哈希值。*/
    for (int i = 0; i < 64; ++i)
    {
        if (dIdex[i] >= mean)
        {
            rst[i] = '1';
        }
        else
        {
            rst[i] = '0';
        }
    }
    for (int i = 0; i < 64; ++i)
    {
        if (dIdex2[i] >= mean2)
        {
            rst2[i] = '1';
        }
        else
        {
            rst2[i] = '0';
        }
    }

    for (int i = 0; i < 64; i++) {
        if (rst[i] != rst2[i])
            diffNum++;
    }
    //cout << "two  picture diffNum is " << diffNum << endl;
    return diffNum;
}
Mat getHistImage(int* histogram) {
    
    Mat hist(1, 256, CV_32F, histogram);   //用一个一维数组实例化Mat数组：hist并初始化

    double minVal = 0;
    double maxVal = 0;
    minMaxLoc(hist, &minVal, &maxVal, 0, 0);  //寻找全局最小、最大像素数目

    //绘制直方图
    Mat histImage(255, 255, CV_8UC3, Scalar(255, 255, 255));
    int hpt = static_cast<uchar>(0.9 * hist.cols); //直方图最大高度
    for (int i = 0; i < 255; i++) {
        float binVal = hist.at<float>(i);
        int intensity = static_cast<int>(binVal * hpt / maxVal);
        line(histImage, Point(i, hist.cols), Point(i, hist.cols - intensity), Scalar(255, 0, 0));
    }
    return histImage;
}

float CheckBlankScreen(int flag, float &result) {
    Mat dst;
    if (flag == 1) {
        cvtColor(*img_1, dst, CV_BGR2GRAY);
    }
    else {
        cvtColor(*img_2, dst, CV_BGR2GRAY);
    }
    //Mat dst = imread("D:/XunLeiDownload/pul2uLXIWGEA/3249250.jpg", IMREAD_GRAYSCALE);
    if (dst.data == NULL) {
        std::cout << "read image failed" << std::endl;
        return -20;
    }
    //cvtColor(src, dst, CV_BGR2GRAY);

    int image_count = 1;  //输出单个直方图
    int channels[1] = { 0 }; //单通道
    Mat out; //输出二维数组
    int dims = 1; //维度为1
    int histSize[1] = { 256 };  //灰度值Size：256个
    float hrange[2] = { 0,255 }; //灰度范围[0-255]
    const float* ranges[1] = { hrange }; //单个灰度范围[0-255]

    int histogram[256] = { 0 };
    int tmp;

    //统计灰度个数
    for (int i = 0; i < dst.rows; i++) {
        for (int j = 0; j < dst.cols; j++) {
            tmp = dst.at<uchar>(i, j);
            histogram[tmp]++;
        }
    }
    int index = 1;
    int darkPixelPoints = 0;
    int sumPixelPoints = 0;
    for (int val : histogram) {
        if (index < 20) {
            darkPixelPoints += val;
        }
        sumPixelPoints += val;
        //std::cout << "index:" << index << "   pixel value : " << val << endl;
        index++;
    }
    
    /*Mat histImage = getHistImage(histogram);
    imshow("srcImage", dst);
    imshow("count_histogram", histImage);*/

    //2、使用calcHist输出二维灰度统计数组
    //calcHist(&dst, image_count, channels, Mat(), out, dims, histSize, ranges);
    //Mat histImage1 = getHistImage(out);   //第二种方法

    //imshow("calcHist_histogram", histImage1);
    //cv::waitKey();
    result =  (float)darkPixelPoints / (float)sumPixelPoints;
    return 0;
}