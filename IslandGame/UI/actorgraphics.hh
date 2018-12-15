#ifndef ACTORGRAPHICS_HH
#define ACTORGRAPHICS_HH

#include "actor.hh"
#include "coordinates.hh"
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>


namespace Student {

class ActorGraphics : public QGraphicsPixmapItem
{
public:

    ActorGraphics(std::shared_ptr<Common::Actor> actor,
                  Common::CubeCoordinate coord,
                  QGraphicsPixmapItem *parent = 0);
    virtual ~ActorGraphics() = default;

    virtual QRectF boundingRect() const;

    void updatePos(Common::CubeCoordinate cubeCoord);
    int getActorId();
    void clearActorGraphVec();
    std::shared_ptr<Common::Actor> getActorPtr();

    static std::vector<ActorGraphics*> getActorGraphVector();
    static void clearActorGrahpVector();

private:
    Common::CubeCoordinate coord_;
    std::shared_ptr<Common::Actor> actor_;

    // Vektori johon tallennetaan pointerit jokaiseen actorgraphicolioihin.
    static std::vector<ActorGraphics*> actorGraphicsVector_;

};

}
#endif // ACTORGRAPHICS_HH
