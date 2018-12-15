#ifndef HEXGRAPHICS_HH
#define HEXGRAPHICS_HH

#include "hex.hh"
#include "igamerunner.hh"
#include "igameboard.hh"
#include "pawngraphics.hh"
#include "transportgraphics.hh"
#include "actorgraphics.hh"
#include "iplayer.hh"
#include "customview.hh"
#include "gamestate.hh"

#include <vector>
#include <QGraphicsItem>
#include <unordered_map>


namespace Student {

/*
 * Luokka vastaa hexagonien piirtämisestä pelilaudalle, jokainen hexagon on
 * hexGraphics olio
*/
class hexGraphics : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    hexGraphics(std::shared_ptr<Common::Hex> hex,
                std::shared_ptr<Common::IGameRunner> runner,
                std::vector<std::shared_ptr<Common::IPlayer>> playerVector,
                CustomView *view,
                std::shared_ptr<Common::IGameState> statePtr,
                std::shared_ptr<Common::IGameBoard> boardPtr,
                QGraphicsItem* parent = nullptr);

    ~hexGraphics();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr);
    QRectF boundingRect() const;
    std::shared_ptr<Common::Hex> getHex();
    pawnGraphics* getPawnGraphics(int pawnId);
    std::shared_ptr<Common::Pawn> getPawn();
    std::shared_ptr<Common::IPlayer> getPlayer();
    ActorGraphics* getActorGraphics(int actorId);
    TransportGraphics* getTransportGraphics(int transportId);
    std::string getFlipTileActor();
    bool checkGoalHex();
    static void nollaaTila();

signals:
    void vaihdaPeliTilaa();
    void updateGraphics();
    void deletePawnsFromCube(Common::CubeCoordinate);
    void deleteTransportsFromCube(Common::CubeCoordinate);
    void deleteActorsFromCube(Common::CubeCoordinate);

protected:

    // Funktio liikuttaa pelaajien nappulat, aktorit, huolehtii fliptile:stä
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:

    // Funktio liikuttaa pelaajien nappulat kun GamePhase::MOVEMENT
    void movePlayerPawns();

    // Funktio liikuttaa transport + pawnit jos Game::Phase::MOVEMENT ja
    // on tuplaklikattu jo hexiä.
    void moveTransWActors();

    // Funktio huolehtii toiminnallisuuden GamePhase::SINKING
    void sinkingFlipTile();

    // Seuraavat funktiot liikuttaa actorit tai transportit riippuen
    // kiekkopyörityksestä GamePhase::SPINNING
    void moveActorSpinningPhase(std::pair<std::string, std::string> kiekkoTiedot);
    void moveDolphinSpinningPhase(std::pair<std::string, std::string> kiekkoTiedot);

    void doActionCleanHex(std::string actor);

    // Funktio palauttaa actorptr jos hexalta löytyy sama actori kuin parametri
    std::shared_ptr<Common::Actor> getActorPtr(std::shared_ptr<Common::Hex> hexPtr, std::string kiekkoActor);
    // Funktio palauttaa transportptr jos hexalta löytyy sama transport kuin parametri
    std::shared_ptr<Common::Transport> getTransPtr(std::shared_ptr<Common::Hex> hexPtr, std::string kiekkoTrans);

    std::shared_ptr<Common::Hex> hex_;

    // Pelilogiikan pointteri
    std::shared_ptr<Common::IGameRunner> runner_;

    // pointteri pelilautaan
    std::shared_ptr< Common::IGameBoard > boardPtr_;

    // pelaajavektori
    std::vector<std::shared_ptr<Common::IPlayer>> playerVector_;

    // pointteri gamestateen
    std::shared_ptr<Common::IGameState> statePtr_;

    std::string actor_;  // Tallennetaan flipTilestä tuleva actor stringiin
    static int phase_;
    static hexGraphics *activeHex_;
    static bool isRightClicked_;

    CustomView *view_;
};
}

#endif // HEXGRAPHICS_HH
