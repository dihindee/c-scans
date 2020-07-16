#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H
#include <QGraphicsView>
#include <QImage>
#include <QWidget>
#include <opencv4/opencv2/opencv.hpp>

class ImageWidget: public QGraphicsView
{
    Q_OBJECT
    cv::Mat img; // QImage не копирует данные, необходимо сохранять источник
    QImage frame;
public:
    ImageWidget(QWidget *parent = nullptr);
    ~ImageWidget();
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void updateFrame(cv::Mat newImg);
};

#endif // IMAGEWIDGET_H
