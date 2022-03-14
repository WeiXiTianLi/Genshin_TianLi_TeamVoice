#include "Genshin_TianLi_TeamVoice.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Genshin_TianLi_TeamVoice w;
    w.show();
    return a.exec();
}
