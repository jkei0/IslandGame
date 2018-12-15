#ifndef TRANSPORTGRAPHICS_HH
#define TRANSPORTGRAPHICS_HH

#include "transport.hh"
#include "coordinates.hh"
#include <memory>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>


namespace Student {


class TransportGraphics : public QGraphicsPixmapItem
{
public:

    TransportGraphics(std::shared_ptr<Common::Transport> transport,
                      Common::CubeCoordinate coord,
                      QGraphicsPixmapItem* parent = nullptr);
    ~TransportGraphics();

    QRectF boundingRect() const;

    void updatePos(Common::CubeCoordinate cubeCoord);
    int getTransportId();

    static std::vector<TransportGraphics*> getTransportVector();
    static void ClearTransportVector();

    std::shared_ptr<Common::Transport> getTransportPtr();

private:
    Common::CubeCoordinate coord_;
    std::shared_ptr<Common::Transport> transport_;

    // Vektori johon tallennetaan pointterit kaikkiin transportin graphicseihin.
    static std::vector<TransportGraphics*> transportGraphVector_;
};
}

#endif // TRANSPORTGRAPHICS_HH
