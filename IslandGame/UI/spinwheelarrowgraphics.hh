#ifndef SPINWHEELARROWGRAPHICS_HH
#define SPINWHEELARROWGRAPHICS_HH

#include <QGraphicsItem>
#include <QGraphicsItemAnimation>
#include <QTimeLine>
#include <string>

class SpinWheelArrowGraphics : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    SpinWheelArrowGraphics();
    ~SpinWheelArrowGraphics();
    QRectF boundingRect() const;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option, QWidget *widget);
signals:
    void pyorita();

protected:
    //void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
    QGraphicsItemAnimation *animation_;
    QTimeLine *timer_;

    unsigned int rotation_;
    std::vector<std::pair<std::string, std::string>> kiekkoVector_;
};

#endif // SPINWHEELARROWGRAPHICS_HH
