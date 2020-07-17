#include "mainwindow.h"
#include <opencv4/opencv2/opencv.hpp>
#include <QFileDialog>
#include <QGroupBox>
#include <QSlider>
#include <QMenuBar>
#include <QCoreApplication>
#include <QVBoxLayout>
#include <string>
#include <QCheckBox>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(1280,720);
    auto controlPanel = new QVBoxLayout(this);
    controlPanel->setSpacing(1);
    auto centralWidget = new QWidget(this);

    imgViewer = new ImageWidget(this);
    detector = new CVDetector();
    auto layout = new QHBoxLayout(this);

    auto origButton = new QCheckBox("show original",this);
    origButton->setChecked(true);
    controlPanel->addWidget(origButton);
    connect(origButton, &QCheckBox::toggled, this, &MainWindow::toggleOriginalDraw);

    //переключение канала изображения
    channelButton = new QPushButton("grayscale",this);
    controlPanel->addWidget(new QLabel("image channel",this));
    controlPanel->addWidget(channelButton);
    connect(channelButton, &QPushButton::clicked, this, &MainWindow::toggleChannel);


    // переключатели метода бинаризации
    auto typeGroup = new QGroupBox("threshold type");
    meanType = new QRadioButton("Mean");
    gaussianType = new QRadioButton("Gaussian");
    gaussianType->setChecked(true);
    otsuType = new QRadioButton("Otsu");
    auto vbox = new QVBoxLayout(this);
    vbox->addWidget(meanType);
    vbox->addWidget(gaussianType);
    vbox->addWidget(otsuType);
    typeGroup->setLayout(vbox);
    controlPanel->addWidget(typeGroup);
    connect(meanType,&QRadioButton::toggled, this, &MainWindow::typeToggled);
    connect(gaussianType,&QRadioButton::toggled, this, &MainWindow::typeToggled);
    connect(otsuType,&QRadioButton::toggled, this, &MainWindow::typeToggled);

    adaptiveParamsBlock = new QWidget(this);
    auto adaptiveParamsLayout = new QVBoxLayout(this);

    // слайдер размера блока
    auto blockSizeSlider = new QSlider(this);
    blockSizeSlider->setRange(3,51);
    connect(blockSizeSlider, &QSlider::valueChanged, this, &MainWindow::blockSizeSliderMoved);
    blockSizeLabel = new QLabel("block size "+ QString::number(detector->getAdaptiveBlockSize()));
    adaptiveParamsLayout->addWidget(blockSizeLabel);
    adaptiveParamsLayout->addWidget(blockSizeSlider);

    // слайдер параметра c
    auto cSlider = new QSlider(this);
    cSlider->setRange(-50,100);
    cLabel = new QLabel("c " + QString::number(detector->getAdaptiveC()));
    connect(cSlider, &QSlider::valueChanged, this, &MainWindow::cSliderMoved);
    adaptiveParamsLayout->addWidget(cLabel);
    adaptiveParamsLayout->addWidget(cSlider);

    adaptiveParamsBlock->setLayout(adaptiveParamsLayout);
    controlPanel->addWidget(adaptiveParamsBlock);

    // слайдер силы размытия
    auto blurSlider = new QSlider(this);
    blurSlider->setRange(0,100);
    blurLabel = new QLabel("blur "  + QString::number(detector->getBlurSize()));
    connect(blurSlider, &QSlider::valueChanged, this, &MainWindow::blurSliderMoved);
    controlPanel->addWidget(blurLabel);
    controlPanel->addWidget(blurSlider);

    // кнопки загрузки/сохранения изображений
    layout->addItem(controlPanel);
    layout->addWidget(imgViewer);
    centralWidget->setLayout(layout);
    auto menuBar = new QMenuBar(this);
    auto openAction = new QAction("open image", this);
    openAction->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+O", nullptr));
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
    menuBar->addAction(openAction);

    auto saveAction = new QAction("save image", this);
    saveAction->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+S", nullptr));
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);
    menuBar->addAction(saveAction);

    setCentralWidget(centralWidget);
    setMenuBar(menuBar);
}

MainWindow::~MainWindow()
{
    delete detector;
}

void MainWindow::blockSizeSliderMoved(int value)
{
    // принимается только нечетное значение
    if(value%2 == 0)
        value++;
    detector->setAdaptiveBlockSize(value);
    blockSizeLabel->setText("block size "+ QString::number(detector->getAdaptiveBlockSize()));
    updateWidget();
}

void MainWindow::cSliderMoved(int value)
{
    detector->setAdaptiveC(value);
    cLabel->setText("c " + QString::number(detector->getAdaptiveC()));
    updateWidget();
}

void MainWindow::blurSliderMoved(int value)
{
    // принимается только нечетное значение
    if(value%2 == 0)
      value++;
    detector->setBlurSize(value);
    blurLabel->setText("blur "  + QString::number(detector->getBlurSize()));
    updateWidget();
}

void MainWindow::typeToggled(bool checked)
{
    if(!checked)
        return;
    adaptiveParamsBlock->setVisible(true);
    if(meanType->isChecked())
        detector->setMeanType();
    if(gaussianType->isChecked())
        detector->setGaussianType();
    if(otsuType->isChecked()){
        detector->setOtsuType();
        adaptiveParamsBlock->setVisible(true); // скрытие ненужных слайдеров
    }
    updateWidget();
}

void MainWindow::toggleChannel()
{
    detector->setNextChannel();
    const char* text[] = {"grayscale","blue","green","red","hue","saturation","value"};
    channelButton->setText(text[detector->getChannel()]);
    updateWidget();
}

void MainWindow::toggleOriginalDraw()
{
    detector->toggleOriginal();
    updateWidget();
}

void MainWindow::openFile()
{
    QString path = QFileDialog::getOpenFileName(this, "Select image", ".", "Image files (*.jpg, *.jpeg, "
                                                                           "*.jpe, *.jfif, "
                                                                           "*.png) | *.jpg; *.jpeg; "
                                                                           "*.jpe; *.jfif; *.png" );
    if(!path.isEmpty()){
        std::string str = path.toStdString();
        detector->setImage(str);
        updateWidget();
    }
}


void MainWindow::saveFile()
{
    QString path = QFileDialog::getSaveFileName(this, "Save image", ".", "Image files (*.jpg, *.jpeg, "
                                                                           "*.jpe, *.jfif, "
                                                                           "*.png) | *.jpg; *.jpeg; "
                                                                           "*.jpe; *.jfif; *.png" );
    if(!path.isEmpty()){
        std::string str = path.toStdString();
        detector->saveImage(str);
    }
}

void MainWindow::updateWidget()
{
    imgViewer->updateFrame(detector->processImage());
}

