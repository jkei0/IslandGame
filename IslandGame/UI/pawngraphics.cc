#include "pawngraphics.hh"
#include "coordinates.hh"
#include <QPainter>

namespace Student {


std::vector<pawnGraphics*> pawnGraphics::pawnGraphicsVector_;

pawnGraphics::pawnGraphics(
        std::shared_ptr<Common::Pawn> pawn, QGraphicsItem *parent) :
    QGraphicsItem(parent),
    pawn_(pawn)
{
    pawnGraphicsVector_.push_back(this);

    animation_ = new QGraphicsItemAnimation;
    timer_ = new QTimeLine(1000);

}

pawnGraphics::~pawnGraphics()
{
    delete animation_;
    delete timer_;
}


void pawnGraphics::paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (pawn_->getPlayerId() == 1) {
        painter->setPen(Qt::red);
        painter->setBrush(Qt::red);
    }

    else if (pawn_->getPlayerId() == 2) {
        painter->setPen(Qt::black);
        painter->setBrush(Qt::black);
    }

    else if (pawn_->getPlayerId() == 3) {
        painter->setPen(Qt::darkCyan);
        painter->setBrush(Qt::darkCyan);
    }
    else if (pawn_->getPlayerId() == 4) {
        painter->setPen(Qt::magenta);
        painter->setBrush(Qt::magenta);
    }

    painter->drawEllipse(0, 0, 8, 8);
}

QRectF pawnGraphics::boundingRect() const
{
    return QRect(0, 0, 8, 8);
}

void pawnGraphics::updatePos(int pawns)
{

    QPointF coords = hex_to_pixel(getLayout(), pawn_->getCoordinates());
    if ( pawns == 1 ) {
        setPos(coords - QPointF(4.5,4.5));
    }
    else if (pawns == 2) {
        setPos(coords - QPointF(4.5,4.5) - QPointF(0, 10));
    }
    else if (pawns==3) {
        setPos(coords - QPointF(4.5,4.5) + QPointF(0, 10));
    }
}

int pawnGraphics::getId()
{
    return pawn_->getId();
}

std::vector<pawnGraphics*> pawnGraphics::getPawnVector()
{
    return pawnGraphicsVector_;
}

void pawnGraphics::clearPawnVector()
{
    pawnGraphicsVector_.clear();
}

void pawnGraphics::animatePosition(int pawns)
{
    timer_->setUpdateInterval(40);
    animation_->setItem(this);
    animation_->setTimeLine(timer_);

    QPointF nextPos = hex_to_pixel(getLayout(), pawn_->getCoordinates());

    if ( pawns == 1 ) {
        nextPos = nextPos - QPointF(4.5,4.5);
    }
    else if (pawns == 2) {
        nextPos = nextPos - QPointF(4.5,4.5) - QPointF(0, 10);
    }
    else if (pawns==3) {
        nextPos = nextPos - QPointF(4.5,4.5) + QPointF(0, 10);
    }

    animation_->setPosAt(1, nextPos);
    timer_->start();
}

std::shared_ptr<Common::Pawn> pawnGraphics::getPawnPointer()
{
    return pawn_;
}

void pawnGraphics::removePawnGraphics()
{
    delete this;
}
}

