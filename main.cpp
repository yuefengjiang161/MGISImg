#include "MGISImg.h"
#include <QtWidgets/QApplication>
#include<QtCore/QStringConverter>
#include<QtCore5Compat/QTextCodec>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec* codec = QTextCodec::codecForName("GBK");
    if (codec == NULL)
        codec = QTextCodec::codecForLocale();

    QTextCodec::setCodecForLocale(codec);
  
    MGISImg w;
    w.show();
    return a.exec();
}
