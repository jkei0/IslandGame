#include "spinwheelgraphics.hh"
#include<math.h>
#include<QPainter>
#include <QPolygonF>

SpinWheelGraphics::SpinWheelGraphics()
{
    setPos(50,50);
}

QRectF SpinWheelGraphics::boundingRect() const
{
    return QRect(0,0,200,200);
}

void SpinWheelGraphics::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    int ISO_SADE = 200;
    int PIENI_SADE = 145;

    // Iso ympyrä
    painter->setBrush(Qt::green);
    painter->drawEllipse(-100,-100,ISO_SADE,ISO_SADE);

    // Pieni ympyrö
    painter->setBrush(Qt::red);
    painter->drawEllipse(-100/1.38,-100/1.38,PIENI_SADE,PIENI_SADE);

    // Jaetaan neljään osaan
    painter->drawLine(0,-100,0,100);
    painter->drawLine(-100,0,100,0);

    // Jaetaan ympyrä 16 osaan
    for ( int i = 1; i <= 16; i++ ) {
        if ( i != 4 && i != 8 && i != 12 && i != 16) {
            painter->drawLine(cos(i*0.39)*(100/1.38), sin(i*0.39)*(100/1.38),
                              cos(i*0.39)*100, sin(i*0.39)*100);
        }
    }

    // Kirjoitetaan actoreiden nimet
    QFont font;
    font.setPointSize(11);
    painter->setFont(font);
    painter->drawText(10,10,45,25,0,QString("Shark"));
    painter->drawText(-60,10,50,25,0,QString("Dolphin"));
    painter->drawText(10,-45,60,50,0,QString("Sea-\nmunster"));
    painter->drawText(-50,-35,45,25,0,QString("Kraken"));

    // Kirjoitetaan liikkeiden määrä
    // Shark
    painter->drawText(15, 75, 20, 20,0, QString("1"));
    painter->drawText(45, 63, 20, 20,0, QString("2"));
    painter->drawText(70, 40, 20, 20,0, QString("3"));
    painter->drawText(80, 7, 20, 20,0, QString("D"));

    // Kraken
    painter->drawText(-22, -92, 20, 20,0, QString("1"));
    painter->drawText(-52, -80, 20, 20,0, QString("2"));
    painter->drawText(-75, -55, 20, 20,0, QString("3"));
    painter->drawText(-90, -23, 20, 20,0, QString("D"));

    // Seamunster
    painter->drawText(15, -92, 20, 20,0, QString("1"));
    painter->drawText(45, -80, 20, 20,0, QString("2"));
    painter->drawText(70, -55, 20, 20,0, QString("3"));
    painter->drawText(80, -23, 20, 20,0, QString("D"));

    // Dolphin
    painter->drawText(-22, 75, 20, 20,0, QString("1"));
    painter->drawText(-52, 63, 20, 20,0, QString("2"));
    painter->drawText(-75, 40, 20, 20,0, QString("3"));
    painter->drawText(-90, 7, 20, 20,0, QString("D"));

}
