#include "transportgraphics.hh"
#include "coordinates.hh"
#include <QPainter>

namespace Student {

std::vector<TransportGraphics*> TransportGraphics::transportGraphVector_;


TransportGraphics::TransportGraphics(std::shared_ptr<Common::Transport> transport,
                                     Common::CubeCoordinate coord,
                                     QGraphicsPixmapItem *parent) :
    QGraphicsPixmapItem(parent),
    coord_(coord),
    transport_(transport)
{
    transportGraphVector_.push_back(this);
    if(transport_->getTransportType() == "boat") {
        setPixmap(QPixmap(":/images/boat.png"));
        setScale(0.02);
    }
    if(transport_->getTransportType() == "dolphin") {
        setPixmap(QPixmap(":/images/dolphin.png"));
        setScale(0.1);
    }
}

TransportGraphics::~TransportGraphics()
{
}

QRectF TransportGraphics::boundingRect() const
{
    return QRect(0, 0, 2500, 2500);
}

void TransportGraphics::updatePos(Common::CubeCoordinate cubeCoord)
{
    QPointF coordinates = hex_to_pixel(getLayout(), cubeCoord);
    setPos(coordinates - QPointF(4.5,4.5) - QPointF(10, 0));
}

int TransportGraphics::getTransportId()
{
    return transport_->getId();
}

std::vector<TransportGraphics *> TransportGraphics::getTransportVector()
{
    return transportGraphVector_;
}

void TransportGraphics::ClearTransportVector()
{
    transportGraphVector_.clear();
}

std::shared_ptr<Common::Transport> TransportGraphics::getTransportPtr()
{
    return transport_;
}

}
