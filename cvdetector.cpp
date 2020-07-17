#include "cvdetector.h"
#include <vector>
#include <cmath>
#include <memory>
using namespace cv;
CVDetector::CVDetector(){}

void CVDetector::setImage(std::string &path){
    img = imread(path);
}

void CVDetector::saveImage(std::string &path){
    Mat pic = processImage();
    imwrite(path,pic);
}

void CVDetector::setMeanType()
{
    isAdaptive = true;
    adaptiveType = ADAPTIVE_THRESH_MEAN_C;
}

void CVDetector::setGaussianType()
{
    isAdaptive = true;
    adaptiveType = ADAPTIVE_THRESH_GAUSSIAN_C;
}

void CVDetector::setOtsuType()
{
    isAdaptive = false;
}



void CVDetector::setAdaptiveBlockSize(int value)
{
    adaptiveBlockSize = value;
}

void CVDetector::setAdaptiveC(int value)
{
    adaptiveC = value;
}

void CVDetector::setBlurSize(int value)
{
    blurSize = value;
}

void CVDetector::setNextChannel()
{
    switch (processingChannel) {
    case GRAYSCALE:
        processingChannel = BLUE;
        break;
    case BLUE:
        processingChannel = GREEN;
        break;
    case GREEN:
        processingChannel = RED;
        break;
    case RED:
        processingChannel = HUE;
        break;
    case HUE:
        processingChannel = SATURATION;
        break;
    case SATURATION:
        processingChannel = VALUE;
        break;
    case VALUE:
        processingChannel = GRAYSCALE;
    }
}

CVDetector::channel CVDetector::getChannel()
{
    return processingChannel;
}

int CVDetector::getAdaptiveBlockSize()
{
    return adaptiveBlockSize;
}

int CVDetector::getAdaptiveC()
{
    return adaptiveC;
}

int CVDetector::getBlurSize()
{
    return blurSize;
}

bool CVDetector::intersects(Rect r1 , Rect r2){
    return !((r1.x > r2.x + r2.width || r2.x > r1.x + r1.width) || (
            r1.y > r2.y + r2.height || r2.y > r1.y + r1.height));
}

std::vector<Rect>* CVDetector::getBounds(){
    auto unitedRects = new std::vector<Rect>;
    if(img.empty())
        return unitedRects;
    Mat imgray, gauss, thresh;
    //конвертация в серый
    if( processingChannel == GRAYSCALE){
        cvtColor(img,imgray,COLOR_BGR2GRAY);
    }
    if( processingChannel >= BLUE && processingChannel <= RED){
        Mat channels[3];
        split(img, channels);
        //нужный канал
        channels[processingChannel-BLUE].copyTo(imgray);
    }
    if( processingChannel >= HUE && processingChannel <= VALUE){
        //конвертируем в hsv
        cvtColor(img,imgray, COLOR_BGR2HSV);
        Mat channels[3];
        split(imgray, channels);
        //нужный канал
        channels[processingChannel-HUE].copyTo(imgray);
    }
    // здесь должна быть фильтрация для лучшего выделения

    // размытие, настраиваемый
    GaussianBlur(imgray,gauss,Size(blurSize,blurSize),0);
    if(isAdaptive){
        // адаптивная бинаризация
        adaptiveThreshold(gauss,thresh,255,adaptiveType, THRESH_BINARY, adaptiveBlockSize, adaptiveC);
    }
    else{
        // статическая бинаризация Оцу
        threshold(gauss,thresh,0,255,THRESH_BINARY+THRESH_OTSU);
    }
    std::vector<std::vector<Point>> contours;
    std::vector<Vec4i> hierarchy;
    // множество точек контуров аномалий с аппроксимацией
    findContours(thresh,contours,hierarchy,RETR_TREE, CHAIN_APPROX_SIMPLE);
    for(auto c : contours){
        Rect rect = boundingRect(c);
        if(rect.width >= minimalBoundSize && rect.width < img.cols/2
                && rect.height >= minimalBoundSize && rect.height < img.rows/2){
            bool overlaps = false;
            for(int i = 0; i < unitedRects->size(); i++){
                // если ограничивающие прямоугольники двух контуров пересекаются, объединяем их в один
                if(intersects(unitedRects->at(i), rect)){
                    int xmin = min(unitedRects->at(i).x,rect.x);
                    int ymin = min(unitedRects->at(i).y, rect.y);
                    (*unitedRects)[i] = Rect(xmin,ymin
                                             ,max(rect.x+rect.width,unitedRects->at(i).x + unitedRects->at(i).width) - xmin
                                             ,max(rect.y+rect.height,unitedRects->at(i).y + unitedRects->at(i).height) - ymin);
                    overlaps = true;
                    break;
                }
            }
            if(!overlaps){
                unitedRects->push_back(rect);
            }
        }
    }
    // объединяем прямоугольники до тех пор, пока это возможно
    int prevSize;
    do{
        prevSize = unitedRects->size();
        for(int i = 0; i < unitedRects->size(); i++){
            for(int j = unitedRects->size()-1; j > i; j--){
                Rect r1 = unitedRects->at(i), r2 = unitedRects->at(j);
                if(intersects(r1,r2)){
                    int xmin = min(r1.x, r2.x);
                    int ymin = min(r1.y, r2.y);
                    (*unitedRects)[i] = Rect(xmin,ymin
                                             ,max(r1.x+r1.width,r2.x + r2.width) - xmin
                                             ,max(r1.y+r1.height,r2.y + r2.height) - ymin);
                    unitedRects->erase(unitedRects->begin() + j);
                }
            }
        }
    }while(prevSize != unitedRects->size());
    return unitedRects;
}

cv::Mat CVDetector::processImage(){
    Mat result;
    if(img.empty())
        return result;
    if(showOriginal){
        img.copyTo(result);
    }
    else{
        if( processingChannel == GRAYSCALE){
            cvtColor(img,result,COLOR_BGR2GRAY);
        }
        if( processingChannel >= BLUE && processingChannel <= RED){
            Mat channels[3];
            split(img, channels);
            //нужный канал
            channels[processingChannel-BLUE].copyTo(result);
        }
        if( processingChannel >= HUE && processingChannel <= VALUE){
            //конвертируем в hsv
            cvtColor(img,result, COLOR_BGR2HSV);
            Mat channels[3];
            split(result, channels);
            //нужный канал
            channels[processingChannel-HUE].copyTo(result);
        }
        cvtColor(result,result,COLOR_GRAY2BGR);
    }
    auto rects = getBounds();
    // рисуем прямоугольники и освобождаем выделенную под них память
    for(auto r : *rects){
        rectangle(result,r,Scalar(0,0,255),2);
    }
    delete rects;
    return result;
}

void CVDetector::toggleOriginal()
{
    showOriginal = !showOriginal;
}

