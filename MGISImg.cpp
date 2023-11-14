#include "MGISImg.h"
#include<QFileDialog>
#include <QMessageBox>
#include "MapCanvas.h"
#include <QHBoxLayout>



MGISImg::MGISImg(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    this->showMaximized();

    myMap = new MapCanvas(this);
    myMap->setMinimumSize(1024, 768);
    this->setCentralWidget(myMap);

    ui.tableView->setModel(myMap->ImgMetaModel());// ��ʼ��ͼ��Ԫ����ģ��
    ui.tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.fileTreeView->setModel(myMap->FileListModel());// ��ʼ���ļ��б�
    ui.fileTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // �����źš��� 
    connect(ui.actionFit_Window, SIGNAL(triggered()), this, SLOT(Fit_Window()));
    connect(ui.actionAbout, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui.actionimage, SIGNAL(triggered()), this, SLOT(PickOpenFile()));
    connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(Exit()));
    connect(ui.actionFile_List, SIGNAL(triggered()), this, SLOT(ShowFileListWindow()));
    connect(ui.actionInformation, SIGNAL(triggered()), this, SLOT(ShowInforWindow()));
    connect(ui.actionZoom_In, SIGNAL(triggered()), this->myMap, SLOT(ZoomIn()));
    connect(ui.actionZoom_Out, SIGNAL(triggered()), this->myMap, SLOT(ZoomOut()));
}

MGISImg::~MGISImg()
{}

/// <summary>
/// �˳�
/// </summary>
void MGISImg::Exit()
{
    this->close();
}

/// <summary>
/// ѡ���ļ�
/// </summary>
void MGISImg::PickOpenFile()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Pick a image file to open..."),
        QDir::currentPath(),
        tr("jpg(*.jpg);;tiff(*.tif);;img(*.img);;All files(*.*)"));
    if (!fileName.isNull())
    {
        myMap->ReadImg(fileName);
    }
    ui.fileTreeView->expandAll();
}

/// <summary>
/// Shows the file list window
/// </summary>
void MGISImg::ShowFileListWindow()
{
    ui.FileListDockWidget->toggleViewAction();
}

/// <summary>
/// Shows the information window.
/// </summary>
void MGISImg::ShowInforWindow()
{
    ui.inforDockWidget->toggleViewAction();
}

///<summary>
/// Fit the window
/// </summary>
void MGISImg::Fit_Window()
{
    myMap->FitWindow();
}

