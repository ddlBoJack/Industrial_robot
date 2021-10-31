//
//  main.cpp
//  Industrial_robot
//
//  Created by BoJack on 2021/10/31.
//

#include<opencv2/opencv.hpp>
#include<iostream>
#include<cmath>
#include<ctime>
#include<vector>
#include<algorithm>

using namespace cv;
using namespace std;

static double angle(Point pt1, Point pt2, Point pt0)
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}


//检测矩形
//第一个参数是传入的原始图像，第二是输出的图像
void findSquares(const Mat& image,Mat &out)
{
    int thresh = 50, N = 5;
    vector<vector<Point> > squares;
    squares.clear();

    Mat src,dst, gray_one, gray;

    src = image.clone();
    out = image.clone();
    gray_one = Mat(src.size(), CV_8U);
    //滤波增强边缘检测
    medianBlur(src, dst, 9);
    //bilateralFilter(src, dst, 25, 25 * 2, 35);

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    //在图像的每个颜色通道中查找矩形
    for (int c = 0; c < image.channels(); c++)
    {
        int ch[] = { c, 0 };

        //通道分离
        mixChannels(&dst, 1, &gray_one, 1, ch, 1);

        // 尝试几个阈值
        for (int l = 0; l < N; l++)
        {
            // 用canny()提取边缘
            if (l == 0)
            {
                //检测边缘
                Canny(gray_one, gray, 5, thresh, 5);
                //膨脹
                dilate(gray, gray, Mat(), Point(-1, -1));
                imshow("dilate", gray);
            }
            else
            {
                gray = gray_one >= (l + 1) * 255 / N;
            }

            // 轮廓查找
            //findContours(gray, contours, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
            findContours(gray, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);

            vector<Point> approx;
            
            // 检测所找到的轮廓
            for (size_t i = 0; i < contours.size(); i++)
            {
                //使用图像轮廓点进行多边形拟合
                approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

                //计算轮廓面积后，得到矩形4个顶点
                if (approx.size() == 4 &&fabs(contourArea(Mat(approx))) > 1000 &&isContourConvex(Mat(approx)))
                {
                    double maxCosine = 0;

                    for (int j = 2; j < 5; j++)
                    {
                        // 求轮廓边缘之间角度的最大余弦
                        double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
                        maxCosine = MAX(maxCosine, cosine);
                    }

                    if (maxCosine < 0.3)
                    {
                        squares.push_back(approx);
                    }
                }
            }
        }
    }

    
    for (size_t i = 0; i < squares.size(); i++)
    {
        const Point* p = &squares[i][0];

        int n = (int)squares[i].size();
        if (p->x > 3 && p->y > 3)
        {
            polylines(out, &p, &n, 1, true, Scalar(0, 255, 0), 3, LINE_AA);
        }
    }
//    imshow("out",out);
//    waitKey(0);
    imwrite("out.jpg",out);
}

int main(int argc, const char * argv[]) {
    Mat src = imread("scene2.jpg", 1);
    Mat out = src.clone();
    findSquares(src, out);
    return 0;
}
