#pragma once

#include <QtWidgets/QWidget>
#include <qpixmap.h>
#include "ui_virapp.h"
#include <vector>
#include "ui_add_collection.h"
#include "AddCollectionDialog.h"
#include "Retriever.h"
#include <chrono>

class Meta {
public:
    std::string path;
    std::string type;
    int size;
    Meta() {}
    Meta(std::string path, std::string type, int size) : path(path), type(type), size(size) {}
};

class VIRApp : public QWidget
{
    Q_OBJECT

public:
    VIRApp(QWidget *parent = nullptr);
    ~VIRApp();

private:
    Ui::VIRAppClass ui;
    Ui::Dialog add_collect;
    cv::Mat currentImg;
    int currentCollection = -1;
    Retriever* retriever;
    std::vector<Meta> meta;
    std::vector<cv::Mat> retrieved;
    std::vector<Meta> readMetadata(const std::string& filename);
    double computeAP(const std::vector<cv::Mat>& retrieved, const std::vector<cv::Mat>& groundTruth);
private slots:
    void loadImages(const std::vector<cv::Mat>& images);
    void selectImage();
    void retrieve();
    void addCollection();
    void onCollectionClicked(QListWidgetItem* item);
    void measuremAP();
};
