#ifndef GAMEBOARD_HH
#define GAMEBOARD_HH

#include "hex.hh"
#include "igameboard.hh"
#include "igamerunner.hh"
#include "coordinates.hh"
#include "mainwindow.hh"
#include "pawn.hh"

#include <map>
#include <unordered_map>
#include <memory>

namespace Student {


class GameBoard : public Common::IGameBoard
{
public:
    GameBoard();
    ~GameBoard();
    virtual int checkTileOccupation(Common::CubeCoordinate tileCoord) const;
    virtual bool isWaterTile(Common::CubeCoordinate tileCoord) const;
    virtual std::shared_ptr<Common::Hex> getHex(Common::CubeCoordinate hexCoord) const;
    virtual void addPawn(int playerId, int pawnId);
    virtual void addPawn(int playerId, int pawnId, Common::CubeCoordinate coord);
    virtual void movePawn(int pawnId, Common::CubeCoordinate pawnCoord);
    virtual void removePawn(int pawnId);
    virtual void addActor(std::shared_ptr<Common::Actor> actor,
                          Common::CubeCoordinate actorCoord);
    virtual void moveActor(int actorId, Common::CubeCoordinate actorCoord);
    virtual void removeActor(int actorId);
    virtual void addHex(std::shared_ptr<Common::Hex> newHex);
    virtual void addTransport(std::shared_ptr<Common::Transport> transport,
                              Common::CubeCoordinate coord);
    virtual void moveTransport(int id, Common::CubeCoordinate coord);
    virtual void removeTransport(int id);
    bool checkPawn(int pawnId);

private:
    // mappi, josta löytyy hexat
    std::map<Common::CubeCoordinate, std::shared_ptr<Common::Hex>> hexMap_;

    // mappi, jossa on pelaajien nappulat, int -> pawnID
    std::map<int, std::shared_ptr<Common::Pawn>> pawnMap_;

    // Mappi, jossa actorit
    std::map<int, Common::CubeCoordinate> actorMap_;

    // Mappi, jossa on transportit
    std::map<int, Common::CubeCoordinate> transportMap_;

};
}
#endif // GAMEBOARD_HH
