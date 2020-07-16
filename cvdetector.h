#ifndef CVDETECTOR_H
#define CVDETECTOR_H
#include <QImage>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>

class CVDetector
{
    bool isAdaptive = true;
    int adaptiveType  = cv::ADAPTIVE_THRESH_GAUSSIAN_C;
    int adaptiveBlockSize = 11;
    int adaptiveC = 20;
    int minimalBoundSize = 5;
    int blurSize = 5;
    cv::Mat img;

public:
    CVDetector();
    void setImage(std::string &path); //чтение из файла
    void saveImage(std::string &path); //запись в файл
    cv::Mat processImage(); // отображение областей на изображании

    void setMeanType();
    void setGaussianType();
    void setOtsuType();
    void setAdaptiveBlockSize(int value);
    void setAdaptiveC(int value);
    void setBlurSize(int value);

    int getAdaptiveBlockSize();
    int getAdaptiveC();
    int getBlurSize();
private:
    std::vector<cv::Rect> *getBounds(); // возвращает прямоугольники областей
    bool intersects(cv::Rect r1, cv::Rect r2);
};

#endif // CVDETECTOR_H
