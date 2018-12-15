#include "actorgraphics.hh"
#include "coordinates.hh"

#include <QPainter>

namespace Student {

std::vector<ActorGraphics*> ActorGraphics::actorGraphicsVector_;

ActorGraphics::ActorGraphics(std::shared_ptr<Common::Actor> actor,
                             Common::CubeCoordinate coord,
                             QGraphicsPixmapItem *parent) :
    QGraphicsPixmapItem(parent),
    coord_(coord),
    actor_(actor)
{

    actorGraphicsVector_.push_back(this);
    if(actor_->getActorType() == "shark") {
        setPixmap(QPixmap(":/images/Shark1.png"));
        setScale(0.3);
    }
    else if(actor_->getActorType() == "seamunster") {
        setPixmap(QPixmap(":/images/Monster.png"));
        setScale(0.02);
    }
    else if(actor_->getActorType() == "kraken") {
        setPixmap(QPixmap(":/images/EvilOctopus.png"));
        setScale(0.01);
    }
    else {
    }

}


QRectF ActorGraphics::boundingRect() const
{
    int leveys = 2500;
    int korkeus = 2500;
    return QRect(0,0,leveys,korkeus);
}

void ActorGraphics::updatePos(Common::CubeCoordinate cubeCoord)
{
    QPointF coordinates = hex_to_pixel(getLayout(), cubeCoord);
    int leveys = 18;
    int korkeus = 15;
    setPos(coordinates - QPointF(leveys,korkeus));
    coord_ = cubeCoord;

}

int ActorGraphics::getActorId()
{
    return actor_->getId();
}

void ActorGraphics::clearActorGraphVec()
{

    actorGraphicsVector_.clear();
}

std::shared_ptr<Common::Actor> ActorGraphics::getActorPtr()
{
    return actor_;
}

std::vector<ActorGraphics *> ActorGraphics::getActorGraphVector()
{
    return actorGraphicsVector_;
}

void ActorGraphics::clearActorGrahpVector()
{
    actorGraphicsVector_.clear();
}
}
