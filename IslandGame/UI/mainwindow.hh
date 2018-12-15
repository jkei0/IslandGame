#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include "gameboard.hh"
#include "gamestate.hh"
#include "player.hh"
#include "dialog.hh"
#include "iplayer.hh"
#include "igameboard.hh"
#include "igamestate.hh"
#include "hexgraphics.hh"
#include "pawngraphics.hh"
#include "customview.hh"
#include "transportgraphics.hh"
#include "spinwheelgraphics.hh"
#include "spinwheelarrowgraphics.hh"

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsView>


/**
 * @file
 * @brief Mainwindow luokka jonka tarkoitus on hallita pelilautaa
 */

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


    /**
     * @brief getKiekkoTiedot palauttaa kiekkotiedot
     * @return std::pair<std::string mikäEläin, std::string paljonkoLiikkuu>
     *
     */
    static std::pair<std::string, std::string> getKiekkoTiedot();
    static int checkIfSpinning();
    void lopetaPeli();


signals:

    //static std::pair<std::string, std::string> getKiekkoTiedot();

public slots:

    void uusiPeli();
    void vaihdaPeliTilaa();
    void pyoritaKiekkoa();

    void deletePawnsFromCube(Common::CubeCoordinate coord);
    void deleteTransportsFromCube(Common::CubeCoordinate coord);
    void deleteActorsFromCube(Common::CubeCoordinate coord);

private:

    void spawnObject();
    void spawnPawns();

    // Funktio alustaa pelilaudan.
    void alustaPelilauta();

    // Funktio tyhjentää pelaajat sekä nappulat laudalta
    void tyhjennaVanhaPeli();

    // Funktio piirtää pelialueelle transportolioita
    void spawnTransport();

    // Funktio hakee pistelabeelin pelaajaIdn perusteella
    void updatePoints(int playerId);

    Ui::MainWindow *ui;
    Student::CustomView *view_;

    // Spinwheelin view
    QGraphicsView *spinView_;

    int pelaajaMaara_;  // Muuttujaan tallennetaan pelaajamäärä pelissä

    static std::pair<std::string, std::string> kiekkoTiedot_;
    //static std::string kiekkoElain_;

    // vektori, jossa pointterit jokaiseen pelaajaan
    std::vector<std::shared_ptr<Common::IPlayer>> playerVector;

    // pointteri gamestate luokkaan
    std::shared_ptr< Common::IGameState > statePtr;

    // pointteri pelilautaan
    std::shared_ptr< Common::IGameBoard > boardPtr;

    // pointteri gamerunner luokkaan
    std::shared_ptr< Common::IGameRunner > runnerPtr;

    // Pointteri spinwheelin grafiikoihin
    SpinWheelGraphics *spinGraphics_;

    // Pointteri spinwheelin nuolen grafiikkaan
    SpinWheelArrowGraphics *spinArrowGraphics_;

    // vektori, jossa piirretyt hexat
    std::vector<Student::hexGraphics*> hexGraphics_;

    // vektori, jossa piirretyt pawnit
    std::vector<Student::pawnGraphics*> pawnGraphics_;

    // Labelit pelaajien määrän ylläpitoon (tämä on tehty tällä tavalla koska
    // ajatus on lisätä peliin mahdollisuus asettaa pelaajille omat nimet.
    QLabel pelaajaYksi_;
    QLabel pelaajaKaksi_;
    QLabel pelaajaKolme_;
    QLabel pelaajaNelja_;
    // Labelit pelaajien pisteiden ylläpitoon
    QLabel pYksiPisteet_;
    QLabel pKaksiPisteet_;
    QLabel pKolmePisteet_;
    QLabel pneljaPisteet_;

    QLabel *lopetaPeliLabel_;
    static int gamePhase_;

};

#endif // MAINWINDOW_HH
