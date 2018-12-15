#ifndef SPINWHEELGRAPHICS_HH
#define SPINWHEELGRAPHICS_HH

#include <QGraphicsItem>


class SpinWheelGraphics : public QGraphicsItem
{
public:
    SpinWheelGraphics();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

};

#endif // SPINWHEELGRAPHICS_HH
