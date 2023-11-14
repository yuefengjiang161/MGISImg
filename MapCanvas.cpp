#include"MapCanvas.h"
#include<qmessagebox.h>
#include<QtCore/qfileinfo.h>
#include<QtGui/qimage.h>
#include<QtGui/qpixmap.h>
#include<QtGui/qtransform.h>
#include <QtGui/QWheelEvent>
#include <QScrollBar>
#include <QGraphicsPixmapItem>


MapCanvas::MapCanvas(QWidget* parent)
	:QGraphicsView(parent)
{
	poDataset = NULL;
	m_scaleFactor = 1.0;
	m_showColor = true;
	imgMetaModel = new QStandardItemModel;
	imgMetaModel->setColumnCount(2);
	fileListModel = new QStandardItemModel;
	QSizePolicy policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	this->setSizePolicy(policy);
}

///<summary>
///Finalizes an instance of thr <see cref="MapCanva"/>class.
/// </summary>
MapCanvas::~MapCanvas()
{

}
///<summary>
///ͼ����Ӧ��������
/// </summary>
/// <param name="RasterDataset">դ�����ݼ�</param>

void MapCanvas::FitWindow()
{
	if (m_showColor == false)
	{
		ShowBand(poDataset->GetRasterBand(1));
	}
	else
	{
		QList<GDALRasterBand*>bandList;
		bandList.append(poDataset->GetRasterBand(1));
		bandList.append(poDataset->GetRasterBand(2));
		bandList.append(poDataset->GetRasterBand(3));
		ShowImg(&bandList);

	}
	GDALClose(poDataset);
}



///<summary>
///��ȡͼ���ļ�
/// </summary>
/// <param name="imgPath">ͼ���ļ�</param>
void MapCanvas::ReadImg(const QString imgPath)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	// std::string s=imgPath.toStdString();
	// (GDALDataset*)GDALOpen(s.c_str(), GA_ReadOnly);
	poDataset = (GDALDataset*)GDALOpen(imgPath.toLocal8Bit().toStdString().c_str(), GA_ReadOnly);
	if (poDataset == NULL)
	{
		QMessageBox::critical(this, tr("Error!"), tr("Can not open file %1").arg(imgPath));
		return;
	}
	ShowFileList(imgPath);
	ShowImgfor(imgPath);
	//���ͼ���ļ������������Σ���Ĭ��ֻ��ʾ��һ���λҶ�Ӱ��
	if (poDataset->GetRasterCount() != 3)
	{
		m_showColor = false;
		ShowBand(poDataset->GetRasterBand(1));

	}
	//���ͼ�������������Σ���Ĭ����RGB��˳����ʾ��ɫͼ
	else
	{
		m_showColor = true;
		QList<GDALRasterBand*>bandList;
		bandList.append(poDataset->GetRasterBand(1));
		bandList.append(poDataset->GetRasterBand(2));
		bandList.append(poDataset->GetRasterBand(3));
		ShowImg(&bandList);
	}
	GDALClose(poDataset);
}

///<summary>
///�رյ�ǰͼ���ļ�
/// </summary>
void MapCanvas::CloseCurrentImg()
{
	poDataset = NULL;
	imgMetaModel->clear();
	fileListModel->clear();
}

///<summary>
///��ʾ������ͼ��
/// </summary>
/// <param name="band">ͼ�񲨶�</param>
void MapCanvas::ShowBand(GDALRasterBand* band)
{
	if (band == NULL)
	{
		return;
	}
	QList<GDALRasterBand*>myBand;
	myBand.append(band);
	myBand.append(band);
	myBand.append(band);

	ShowImg(&myBand);
}

///<summary>
///��ʾͼ��
/// </summary>
/// <param name="imgBand">ͼ�񲨶�</param>
void MapCanvas::ShowImg(QList<GDALRasterBand*>* imgBand)
{
	if (imgBand->size() != 3)
	{
		return;
	}
	int imgWidth = imgBand->at(0)->GetXSize();
	int imgHeight = imgBand->at(0)->GetYSize();

	m_scaleFactor = this->width() * 1.0 / imgWidth;
	int iScaleWidth = (int)(imgWidth * m_scaleFactor - 1);
	int iScaleHeight = (int)(imgHeight * m_scaleFactor - 1);

	GDALDataType dataType = imgBand->at(0)->GetRasterDataType();

	//���ȷֱ��ȡRGB��������
	float* rBand = new float[iScaleWidth * iScaleHeight];
	float* gBand = new float[iScaleWidth * iScaleHeight];
	float* bBand = new float[iScaleWidth * iScaleHeight];

	unsigned char* rBandUC, * gBandUC, * bBandUC;

	//�����Ƿ���ʾ��ɫͼ���ж�RGB�������ε���ɷ�ʽ�����ֱ��ȡ
	if (m_showColor == true)
	{
		imgBand->at(0)->RasterIO(GF_Read, 0, 0, imgWidth, imgHeight, rBand, iScaleWidth, iScaleHeight, GDT_CFloat32, 0, 0);
		imgBand->at(1)->RasterIO(GF_Read, 0, 0, imgWidth, imgHeight, gBand, iScaleWidth, iScaleHeight, GDT_CFloat32, 0, 0);
		imgBand->at(2)->RasterIO(GF_Read, 0, 0, imgWidth, imgHeight, bBand, iScaleWidth, iScaleHeight, GDT_CFloat32, 0, 0);

		//�ֱ�����ÿ�����β���Floatת��Ϊunsigned char
		rBandUC = ImgSketch(rBand, imgBand->at(0), iScaleWidth * iScaleHeight, imgBand->at(0)->GetNoDataValue());
		gBandUC = ImgSketch(gBand, imgBand->at(1), iScaleWidth * iScaleHeight, imgBand->at(1)->GetNoDataValue());
		bBandUC = ImgSketch(bBand, imgBand->at(2), iScaleWidth * iScaleHeight, imgBand->at(2)->GetNoDataValue());
	}
	else
	{
		imgBand->at(0)->RasterIO(GF_Read, 0, 0, imgWidth, imgHeight, rBand, iScaleWidth, iScaleHeight, GDT_Float32, 0, 0);
		rBandUC = ImgSketch(rBand, imgBand->at(0), iScaleWidth * iScaleHeight, imgBand->at(0)->GetNoDataValue());
		gBandUC = rBandUC;
		bBandUC = rBandUC;

	}
	//������������Ϻ�����
	int bytePerLine = (iScaleWidth * 24 + 31) / 8;
	unsigned char* allBandUC = new unsigned char[bytePerLine * iScaleHeight * 3];
	for (int h = 0; h < iScaleHeight; h++)
	{
		for (int w = 0; w < iScaleWidth; w++)
		{
			allBandUC[h * bytePerLine + w * 3 + 0] = rBandUC[h * iScaleWidth + w];
			allBandUC[h * bytePerLine + w * 3 + 1] = gBandUC[h * iScaleWidth + w];
			allBandUC[h * bytePerLine + w * 3 + 2] = bBandUC[h * iScaleWidth + w];

		}
	}
	//����ͼ����ʾ
	QGraphicsPixmapItem* imgItem = new QGraphicsPixmapItem(QPixmap::fromImage(QImage(allBandUC, iScaleWidth, iScaleHeight, bytePerLine, QImage::Format_RGB888)));
	QGraphicsScene* myScence = new QGraphicsScene;
	myScence->addItem(imgItem);
	this->setScene(myScence);
}

///<summary>
///��ʾͼ�������Ϣ
/// </summary>
/// <param name="filename">�ļ���</param>
void MapCanvas::ShowImgfor(const QString filename)
{
	if (filename == "" || poDataset == NULL)
	{
		return;
	}
	int row = 0; //������¼����ģ�͵��к�

	// ͼ��ĸ�ʽ��Ϣ
	imgMetaModel->setItem(row, 0, new QStandardItem(tr("Description")));
	imgMetaModel->setItem(row++, 1, new QStandardItem(poDataset->GetDriver()->GetDescription()));
	imgMetaModel->setItem(row, 0, new QStandardItem(tr("Meta Infor")));
	imgMetaModel->setItem(row++, 1, new QStandardItem(poDataset->GetDriver()->GetMetadataItem(GDAL_DMD_LONGNAME)));
	imgMetaModel->setItem(row, 0, new QStandardItem(tr("Data Type")));
	imgMetaModel->setItem(row++, 1, new QStandardItem(GDALGetDataTypeName((poDataset->GetRasterBand(1)->GetRasterDataType()))));

	// ͼ��Ĵ�С�Ͳ��θ���
	imgMetaModel->setItem(row, 0, new QStandardItem(tr("X Size")));
	imgMetaModel->setItem(row++, 1, new QStandardItem(QString::number(poDataset->GetRasterXSize())));
	imgMetaModel->setItem(row, 0, new QStandardItem(tr("Y Size")));
	imgMetaModel->setItem(row++, 1, new QStandardItem(QString::number(poDataset->GetRasterYSize())));
	imgMetaModel->setItem(row, 0, new QStandardItem(tr("Band Count")));
	imgMetaModel->setItem(row++, 1, new QStandardItem(QString::number(poDataset->GetRasterCount())));

	// ͼ���ͶӰ��Ϣ
	imgMetaModel->setItem(row, 0, new QStandardItem(tr("Projection")));
	imgMetaModel->setItem(row++, 1, new QStandardItem(poDataset->GetProjectionRef()));

	// ͼ�������ͷֱ�����Ϣ
	double adfGeoTransform[6];
	QString origin = "";
	QString pixelSize = "";
	if (poDataset->GetGeoTransform(adfGeoTransform) == CE_None)
	{
		origin = QString::number(adfGeoTransform[0]) + ", " + QString::number(adfGeoTransform[3]);
		pixelSize = QString::number(adfGeoTransform[1]) + ", " + QString::number(adfGeoTransform[5]);
	}
	imgMetaModel->setItem(row, 0, new QStandardItem(tr("Origin")));
	imgMetaModel->setItem(row++, 1, new QStandardItem(origin));
	imgMetaModel->setItem(row, 0, new QStandardItem(tr("Pixel Size")));
	imgMetaModel->setItem(row++, 1, new QStandardItem(pixelSize));
	
}

/// <summary>
/// ��ʾ�ļ��ṹ��
/// </summary>
/// <param name="filename">�ļ���</param>
void MapCanvas::ShowFileList(const QString filename)
{
	if (filename == "" || poDataset == NULL)
	{
		return;
	}
	QFileInfo fileInfo(filename);
	QStandardItem* rootItem = new QStandardItem(fileInfo.fileName());
	for (int i = 0; i < poDataset->GetRasterCount(); i++)
	{
		QStandardItem* childItem = new QStandardItem(tr("Band %1").arg(i + 1));
		rootItem->setChild(i, childItem);
	}
	fileListModel->setItem(0, rootItem);
}


/// <summary>
/// ͼ����������
/// </summary>
/// <param name="buffer">ͼ�񻺴�</param>
/// <param name="currentBand">��ǰ����</param>
/// <param name="size">The size.</param>
/// <param name="noValue">ͼ���е��쳣ֵ</param>
/// <returns>���������8λͼ�񻺴�</returns>

unsigned char* MapCanvas::ImgSketch(float* buffer, GDALRasterBand* currentBand, int bandSize, double noValue)
{
	unsigned char* resBuffer = new unsigned char[bandSize];
	double max, min;
	double minmax[2];


	currentBand->ComputeRasterMinMax(1, minmax);
	min = minmax[0];
	max = minmax[1];
	if (min <= noValue && noValue <= max)
	{
		min = 0;
	}
	for (int i = 0; i < bandSize; i++)
	{
		if (buffer[i] > max)
		{
			resBuffer[i] = 255;
		}
		else if (buffer[i] <= max && buffer[i] >= min)
		{
			resBuffer[i] = static_cast<uchar>(255 - 255 * (max - buffer[i]) / (max - min));
		}
		else
		{
			resBuffer[i] = 0;
		}
	}

	return resBuffer;
}

/// <summary>
/// �ؼ���С
/// </summary>
/// <returns>QSize.</returns>
QSize MapCanvas::sizeHint() const
{
	return QSize(1024, 768);
}

/// <summary>
/// �������¼���ʵ��ͼ������
/// </summary>
/// <param name="event">�����¼�</param>
void MapCanvas::wheelEvent(QWheelEvent* event)
{
	QPoint numDegrees = event->angleDelta();
	// �������ϻ������Ŵ�ͼ��
	if (numDegrees.y() > 0)
	{
		ZoomIn();
	}
	// �������»�������Сͼ��
	if (numDegrees.y() < 0)
	{
		ZoomOut();
	}
}

/// <summary>
/// ��갴�������¼�
/// </summary>
/// <param name="event">����¼�.</param>
void MapCanvas::mousePressEvent(QMouseEvent* event)
{
	// ���ּ����£�ƽ��ͼ��
	if (event->button() == Qt::MiddleButton)
	{
		this->setDragMode(QGraphicsView::ScrollHandDrag);
		this->setInteractive(false);
		lastEventCursorPos = event->pos();
	}
}

/// <summary>
/// ����ƶ��¼�
/// </summary>
/// <param name="event">����¼�</param>
void MapCanvas::mouseMoveEvent(QMouseEvent* event)
{
	if (this->dragMode() == QGraphicsView::ScrollHandDrag)
	{
		QPoint delta = (event->pos() - lastEventCursorPos) / 10;
		this->horizontalScrollBar()->setValue(this->horizontalScrollBar()->value() + (isRightToLeft() ? delta.x() : -delta.x()));
		this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - delta.y());
		this->viewport()->setCursor(Qt::ClosedHandCursor);
	}

}

/// <summary>
/// ��갴���ͷ��¼�
/// </summary>
/// <param name="event">����¼�</param>
void MapCanvas::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::MiddleButton)
	{
		this->setDragMode(QGraphicsView::NoDrag);
	}
}