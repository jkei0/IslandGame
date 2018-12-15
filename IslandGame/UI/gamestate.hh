#ifndef GAMESTATE_HH
#define GAMESTATE_HH

#include "iplayer.hh"
#include "igamestate.hh"

#include <vector>

namespace Student {


class GameState : public Common::IGameState
{
public:
    GameState();
    ~GameState();

    virtual Common::GamePhase currentGamePhase() const;
    virtual int currentPlayer() const;
    virtual void changeGamePhase(Common::GamePhase nextPhase);
    virtual void changePlayerTurn(int nextPlayer);

    int getPawnPerPlayer();
    void addPointsToPlayer(int playerId, int points);
    void deletePawn(int pawnId);
    int playerPawnsLeft(int playerId);
    void addPawn(int playerId,int pawnId);
    void initializePlayers(std::vector<std::shared_ptr<Common::IPlayer>>
                           playerVector);
    int getPoints(int playerId);

private:
    Common::GamePhase gamePhaseId_;

    int playerInTurn_;
    int pawnPerPlayer_;

    // Vectori mihin tallennetaan pelaajaId sekä pelaajan pisteet
    std::vector<std::pair<int,int>> playerPointVector_;
    // Vectori johon tallennetaan pelaajaId sekä pawnId
    std::vector<std::pair<int,int>> playerPawnVector_;
};
}

#endif // GAMESTATE_HH
