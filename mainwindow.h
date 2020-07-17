#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QRadioButton>
#include <QPushButton>

#include "imagewidget.h"
#include "cvdetector.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    ImageWidget *imgViewer;
    CVDetector *detector;
    QWidget *adaptiveParamsBlock;
    QLabel *blockSizeLabel, *cLabel, *blurLabel;
    QRadioButton *meanType, *gaussianType, *otsuType;
    QPushButton *channelButton;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void blockSizeSliderMoved(int value);
    void cSliderMoved(int value);
    void blurSliderMoved(int value);
    void typeToggled(bool checked);
    void toggleChannel();
    void toggleOriginalDraw();
    void openFile();
    void saveFile();
private:
    void updateWidget();

};
#endif // MAINWINDOW_H
