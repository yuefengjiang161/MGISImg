#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MGISImg.h"


class MapCanvas;

class MGISImg : public QMainWindow
{
    Q_OBJECT

public:
    MGISImg(QWidget *parent = nullptr);
    ~MGISImg();

public slots:
    void PickOpenFile();
    void Exit();
    void ShowFileListWindow();
    void ShowInforWindow();
    void Fit_Window();

private:
    Ui::MGISImgClass ui;
    MapCanvas* myMap;
};
