#include "imagewidget.h"

ImageWidget::ImageWidget(QWidget *parent): QGraphicsView(parent)
{
    setScene(new QGraphicsScene(this));
    setMinimumSize(640,480);
    setMaximumSize(1280,720);
}

ImageWidget::~ImageWidget()
{
}
void ImageWidget::updateFrame(cv::Mat newImg){
    img = newImg;
    frame = QImage(img.data,img.cols,img.rows,img.step,QImage::Format_BGR888);
    scene()->update();
}
void ImageWidget::drawBackground(QPainter *painter, const QRectF &rect){
    painter->resetTransform();
    painter->fillRect(0,0, width(), height(), QColor(0,255,0));
    if(!img.empty())
        painter->drawImage(QRectF(0,0,width(),height()), frame);
}
