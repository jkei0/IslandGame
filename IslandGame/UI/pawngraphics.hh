#ifndef PAWNGRAPHICS_HH
#define PAWNGRAPHICS_HH

#include "coordinates.hh"
#include "pawn.hh"
#include "hex.hh"

#include <vector>
#include <QGraphicsItem>
#include <unordered_map>
#include <QGraphicsItemAnimation>
#include <QTimeLine>

namespace Student {

/*
 * Luokka vastaa pelaajien nappuloiden piirtämisestä pelilaudalle, jokainen nappula on
 * oma pawnGraphics olio
*/
class pawnGraphics : public QObject, public QGraphicsItem
{
public:
    pawnGraphics(std::shared_ptr<Common::Pawn> pawn,
                QGraphicsItem* parent = nullptr);
    ~pawnGraphics();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr);
    QRectF boundingRect() const;

    void updatePos(int pawns);
    int getId();
    static std::vector<pawnGraphics*> getPawnVector();
    static void clearPawnVector();
    void animatePosition(int pawns);
    std::shared_ptr<Common::Pawn> getPawnPointer();

    // Malin testaa
    void removePawnGraphics();

private:

    std::shared_ptr<Common::Pawn> pawn_;
    static std::vector<pawnGraphics*> pawnGraphicsVector_;

    QGraphicsItemAnimation *animation_;
    QTimeLine *timer_;
};
}

#endif // PAWNGRAPHICS_HH
