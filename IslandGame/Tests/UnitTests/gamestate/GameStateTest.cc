#include <QString>
#include <QtTest>
#include "gamestate.hh"
#include "player.hh"

class GamestateTest : public QObject
{
    Q_OBJECT

public:
    GamestateTest();

private Q_SLOTS:
    void testChangeGamePhase();
    void testChangePlayerTurn();
    void testAddPointsToPlayer();
    void testPlayerPawnsLeft();
    void testDeletePawn();
};

GamestateTest::GamestateTest()
{
}

void GamestateTest::testChangeGamePhase()
{
    Student::GameState game;

    game.changeGamePhase(Common::GamePhase::MOVEMENT);
    QCOMPARE(game.currentGamePhase(), Common::GamePhase::MOVEMENT);

    game.changeGamePhase(Common::GamePhase::SINKING);
    QCOMPARE(game.currentGamePhase(), Common::GamePhase::SINKING);

    game.changeGamePhase(Common::GamePhase::SPINNING);
    QCOMPARE(game.currentGamePhase(), Common::GamePhase::SPINNING);
}

void GamestateTest::testChangePlayerTurn()
{
    Student::GameState game;

    game.changePlayerTurn(1);
    QCOMPARE(game.currentPlayer(), 1);

    game.changePlayerTurn(2);
    QCOMPARE(game.currentPlayer(), 2);

    game.changePlayerTurn(3);
    QCOMPARE(game.currentPlayer(), 3);

    game.changePlayerTurn(4);
    QCOMPARE(game.currentPlayer(), 4);
}

void GamestateTest::testAddPointsToPlayer()
{
    Student::GameState game;
    std::vector<std::shared_ptr<Common::IPlayer>> playerVector;
    std::shared_ptr<Common::IPlayer> player = std::make_shared<Student::Player>(1,6);
    playerVector.push_back(player);
    game.initializePlayers(playerVector);

    game.addPointsToPlayer(1, 100);
    QCOMPARE(game.getPoints(1), 100);

    game.addPointsToPlayer(1, 100);
    QCOMPARE(game.getPoints(1), 200);
}

void GamestateTest::testPlayerPawnsLeft()
{
    Student::GameState game;

    game.addPawn(1,1);
    QCOMPARE(game.playerPawnsLeft(1), 1);

    game.addPawn(1,2);
    QCOMPARE(game.playerPawnsLeft(1), 2);

    game.addPawn(1,3);
    QCOMPARE(game.playerPawnsLeft(1), 3);

    game.addPawn(1,4);
    QCOMPARE(game.playerPawnsLeft(1), 4);

    game.deletePawn(1);
    QCOMPARE(game.playerPawnsLeft(1), 3);

}

void GamestateTest::testDeletePawn()
{
    Student::GameState game;

    game.addPawn(1,1);
    game.addPawn(1,2);
    game.addPawn(1,3);
    game.addPawn(1,4);
    game.addPawn(1,5);
    game.addPawn(2,6);
    game.addPawn(2,7);
    game.addPawn(2,8);
    game.addPawn(2,9);
    game.addPawn(2,10);

    QCOMPARE(game.playerPawnsLeft(1), 5);
    QCOMPARE(game.playerPawnsLeft(2), 5);

    game.deletePawn(1);
    QCOMPARE(game.playerPawnsLeft(1), 4);
    QCOMPARE(game.playerPawnsLeft(2), 5);

    game.deletePawn(2);
    QCOMPARE(game.playerPawnsLeft(1), 3);
    QCOMPARE(game.playerPawnsLeft(2), 5);

    game.deletePawn(6);
    QCOMPARE(game.playerPawnsLeft(1), 3);
    QCOMPARE(game.playerPawnsLeft(2), 4);

    game.deletePawn(3);
    game.deletePawn(4);
    game.deletePawn(5);
    QCOMPARE(game.playerPawnsLeft(1), 0);
    QCOMPARE(game.playerPawnsLeft(2), 4);
}



QTEST_APPLESS_MAIN(GamestateTest)

#include "GameStateTest.moc"
