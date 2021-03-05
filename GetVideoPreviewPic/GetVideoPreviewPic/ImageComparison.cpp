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
    //OpenCV�汾��
    cout << "OpenCV_Version: " << CV_VERSION << endl;

    //��ȡͼƬ
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

    //-- ��һ��:��� Oriented FAST �ǵ�λ��
    detector->detect(*img_1, keypoints_1);
    detector->detect(*img_2, keypoints_2);

    //-- �ڶ���:���ݽǵ�λ�ü��� BRIEF ������
    descriptor->compute(*img_1, keypoints_1, descriptors_1);
    descriptor->compute(*img_2, keypoints_2, descriptors_2);

    Mat outimg1;
    drawKeypoints(*img_1, keypoints_1, outimg1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
    imshow("ORB������", outimg1);

    //-- ������:������ͼ���е�BRIEF�����ӽ���ƥ�䣬ʹ�� Hamming ����
    //BFMatcher matcher(NORM_HAMMING);
    vector<DMatch> matches;
    matcher->match(descriptors_1, descriptors_2, matches);

    //-- ���Ĳ�:ƥ����ɸѡ
    double min_dist = 10000, max_dist = 0;
    //�ҳ�����ƥ��֮�����С�����������, ���������Ƶĺ�����Ƶ������֮��ľ���
    for (int i = 0; i < descriptors_1.rows; i++)
    {
        double dist = matches[i].distance;
        if (dist < min_dist) min_dist = dist;
        if (dist > max_dist) max_dist = dist;
    }

    // �������ֵ�д��
    //min_dist = min_element(matches.begin(), matches.end(), [](const DMatch& m1, const DMatch& m2) {return m1.distance < m2.distance; })->distance;
    //max_dist = max_element(matches.begin(), matches.end(), [](const DMatch& m1, const DMatch& m2) {return m1.distance < m2.distance; })->distance;

    printf("-- Max dist : %f \n", max_dist);
    printf("-- Min dist : %f \n", min_dist);

    //��������֮��ľ��������������С����ʱ,����Ϊƥ������.����ʱ����С�����ǳ�С,����һ������ֵ30��Ϊ����.
    std::vector< DMatch > good_matches;
    for (int i = 0; i < descriptors_1.rows; i++)
    {
        if (matches[i].distance <= max(2 * min_dist, 30.0))
        {
            good_matches.push_back(matches[i]);
        }
    }

    //-- ���岽:����ƥ����
    Mat img_match;
    Mat img_goodmatch;
    drawMatches(*img_1, keypoints_1, *img_2, keypoints_2, matches, img_match);
    drawMatches(*img_1, keypoints_1, *img_2, keypoints_2, good_matches, img_goodmatch);
    imshow("����ƥ����", img_match);
    imshow("�Ż���ƥ����", img_goodmatch);
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

    //HSV��ɫ����ģ��(ɫ��H,���Ͷ�S������V)
    cvtColor(*img_1, *img_1, COLOR_BGR2HSV);
    cvtColor(*img_2, *img_2, COLOR_BGR2HSV);
    //ֱ��ͼ�ߴ�����
    //һ���Ҷ�ֵ�����趨һ��bins��256���Ҷ�ֵ�Ϳ����趨256��bins
    //��ӦHSV��ʽ,������άֱ��ͼ
    //ÿ��ά�ȵ�ֱ��ͼ�Ҷ�ֵ����Ϊ256�����ͳ�ƣ�Ҳ����ʹ������ֵ
    int hBins = 256, sBins = 256;
    int histSize[] = { hBins,sBins };
    //H:0~180, S:0~255,V:0~255
    //Hɫ��ȡֵ��Χ
    float hRanges[] = { 0,180 };
    //S���Ͷ�ȡֵ��Χ
    float sRanges[] = { 0,255 };
    const float* ranges[] = { hRanges,sRanges };
    int channels[] = { 0,1 };//��άֱ��ͼ
    MatND hist1, hist2;
    calcHist(img_1, 1, channels, Mat(), hist1, 2, histSize, ranges, true, false);
    normalize(hist1, hist1, 0, 1, NORM_MINMAX, -1, Mat());
    calcHist(img_2, 1, channels, Mat(), hist2, 2, histSize, ranges, true, false);
    normalize(hist2, hist2, 0, 1, NORM_MINMAX, -1, Mat());
    double similarityValue = compareHist(hist1, hist2, CV_COMP_CORREL);
    cout << "���ƶȣ�" << similarityValue << endl;
    if (similarityValue >= 0.75)
    {
        return true;
    }
    return false;
}
bool compareFacesByHist��()
{
    Mat tmpImg;
    cv::Mat src1 = cv::imread("D:\TestVideo\picture\picture-��.jpg", CV_LOAD_IMAGE_COLOR);
    cv::Mat src2 = cv::imread("D:\TestVideo\picture\picture-1.jpg", CV_LOAD_IMAGE_COLOR);
    //resize(*img_1, tmpImg, Size(orgImg.cols, orgImg.rows));

    //HSV��ɫ����ģ��(ɫ��H,���Ͷ�S������V)
    cvtColor(src1, src1, COLOR_BGR2HSV);
    cvtColor(src2, src2, COLOR_BGR2HSV);
    //ֱ��ͼ�ߴ�����
    //һ���Ҷ�ֵ�����趨һ��bins��256���Ҷ�ֵ�Ϳ����趨256��bins
    //��ӦHSV��ʽ,������άֱ��ͼ
    //ÿ��ά�ȵ�ֱ��ͼ�Ҷ�ֵ����Ϊ256�����ͳ�ƣ�Ҳ����ʹ������ֵ
    int hBins = 256, sBins = 256;
    int histSize[] = { hBins,sBins };
    //H:0~180, S:0~255,V:0~255
    //Hɫ��ȡֵ��Χ
    float hRanges[] = { 0,180 };
    //S���Ͷ�ȡֵ��Χ
    float sRanges[] = { 0,255 };
    const float* ranges[] = { hRanges,sRanges };
    int channels[] = { 0,1 };//��άֱ��ͼ
    MatND hist1, hist2;
    calcHist(&src1, 1, channels, Mat(), hist1, 2, histSize, ranges, true, false);
    normalize(hist1, hist1, 0, 1, NORM_MINMAX, -1, Mat());
    calcHist(&src2, 1, channels, Mat(), hist2, 2, histSize, ranges, true, false);
    normalize(hist2, hist2, 0, 1, NORM_MINMAX, -1, Mat());
    double similarityValue = compareHist(hist1, hist2, CV_COMP_CORREL);
    cout << "���ƶȣ�" << similarityValue << endl;
    if (similarityValue >= 0.85)
    {
        return true;
    }
    return false;
}


//pHash�㷨
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

    /* ��һ�������ųߴ�*/
    resize(img, img, Size(32, 32));
    resize(img2, img2, Size(32, 32));

    /* �ڶ�������ɢ���ұ任��DCTϵ����ȡ*/
    dct(img, dst);
    dct(img2, dst2);

    /* ����������ȡDCTϵ����ֵ�����Ͻ�8*8�����DCTϵ����*/
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

    /* ���Ĳ��������ϣֵ��*/
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
    
    Mat hist(1, 256, CV_32F, histogram);   //��һ��һά����ʵ����Mat���飺hist����ʼ��

    double minVal = 0;
    double maxVal = 0;
    minMaxLoc(hist, &minVal, &maxVal, 0, 0);  //Ѱ��ȫ����С�����������Ŀ

    //����ֱ��ͼ
    Mat histImage(255, 255, CV_8UC3, Scalar(255, 255, 255));
    int hpt = static_cast<uchar>(0.9 * hist.cols); //ֱ��ͼ���߶�
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

    int image_count = 1;  //�������ֱ��ͼ
    int channels[1] = { 0 }; //��ͨ��
    Mat out; //�����ά����
    int dims = 1; //ά��Ϊ1
    int histSize[1] = { 256 };  //�Ҷ�ֵSize��256��
    float hrange[2] = { 0,255 }; //�Ҷȷ�Χ[0-255]
    const float* ranges[1] = { hrange }; //�����Ҷȷ�Χ[0-255]

    int histogram[256] = { 0 };
    int tmp;

    //ͳ�ƻҶȸ���
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

    //2��ʹ��calcHist�����ά�Ҷ�ͳ������
    //calcHist(&dst, image_count, channels, Mat(), out, dims, histSize, ranges);
    //Mat histImage1 = getHistImage(out);   //�ڶ��ַ���

    //imshow("calcHist_histogram", histImage1);
    //cv::waitKey();
    result =  (float)darkPixelPoints / (float)sumPixelPoints;
    return 0;
}