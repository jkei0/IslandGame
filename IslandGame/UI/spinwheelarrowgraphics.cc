#include "spinwheelarrowgraphics.hh"
#include "mainwindow.hh"
#include <math.h>
#include <QPainter>

#include <iostream>

SpinWheelArrowGraphics::SpinWheelArrowGraphics()
{
    setPos(50,50);

    animation_ = new QGraphicsItemAnimation;
    timer_ = new QTimeLine(3000);
    rotation_ = 0;

    kiekkoVector_ =  {std::make_pair("shark","D"),
                       std::make_pair("shark","3"),
                       std::make_pair("shark","2"),
                       std::make_pair("shark","1"),
                       std::make_pair("dolphin","1"),
                       std::make_pair("dolphin","2"),
                       std::make_pair("dolphin","3"),
                       std::make_pair("dolphin","D"),
                       std::make_pair("kraken","D"),
                       std::make_pair("kraken","3"),
                       std::make_pair("kraken","2"),
                       std::make_pair("kraken","1"),
                       std::make_pair("seamunster","1"),
                       std::make_pair("seamunster","2"),
                       std::make_pair("seamunster","3"),
                       std::make_pair("seamunster","D")
                     };
}

SpinWheelArrowGraphics::~SpinWheelArrowGraphics()
{
    delete animation_;
    delete timer_;
}

QRectF SpinWheelArrowGraphics::boundingRect() const
{
    return QRect(-70, -70, 140, 140);
}

void SpinWheelArrowGraphics::paint(QPainter *painter,
                                   const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    QPolygonF poly;
    poly.push_back(QPointF(-70,3));
    poly.push_back(QPointF(60,3));
    poly.push_back(QPointF(70,0));
    poly.push_back(QPointF(60,-3));
    poly.push_back(QPointF(-70,-3));
    poly.push_back(QPointF(-70,3));

    painter->setBrush(Qt::black);
    painter->drawPolygon(poly);
}

void SpinWheelArrowGraphics::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{

    Q_UNUSED(event);
    // Lähetetään signaali mainwindowiin
    emit pyorita();

    if ( MainWindow::checkIfSpinning() == 1) {

        // haetaan kiekkotiedot
        auto kiekkoTiedot = MainWindow::getKiekkoTiedot();

        // Lasketaan kulma mihin kiekko päätyy
        int kulma = 0;
        for (int i = 1; i <= 16; i++) {
            if(kiekkoVector_.at(i-1).first == kiekkoTiedot.first &&
                    kiekkoVector_.at(i-1).second == kiekkoTiedot.second) {
                kulma = i * 22.5;
            }
        }


        animation_->setItem(this);
        animation_->setTimeLine(timer_);
        timer_->setUpdateInterval(20);
        setRotation(rotation_);
        animation_->setRotationAt(1, 360*3 + kulma-10 + 360-rotation_);
        timer_->start();
        rotation_ = kulma-10;

    }

}
