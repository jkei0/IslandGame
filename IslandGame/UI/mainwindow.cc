#include "mainwindow.hh"
#include "ui_mainwindow.h"
#include "gameboard.hh"
#include "gamestate.hh"
#include "player.hh"
#include "hexgraphics.hh"
#include "coordinates.hh"
#include "dialog.hh"
#include "pawngraphics.hh"
#include "transportgraphics.hh"
#include "igamerunner.hh"
#include "initialize.hh"
#include "igameboard.hh"
#include "iplayer.hh"
#include "coordinates.hh"


#include <QWidget>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QGraphicsView>
#include <QGraphicsPolygonItem>
#include <QPainter>
#include <QSpinBox>
#include <QString>
#include <QColor>
#include <memory>



unsigned const PELAAJIENNAPPULAMAARA = 7;
QString const VAIHDE1TEKSTI = "Siirtele nappuloita";
QString const VAIHDE2TEKSTI = "Upota saaripala";
QString const VAIHDE3TEKSTI = "Pyöritä kiekkoa ja siirtele eläimiä";


std::pair<std::string, std::string> MainWindow::kiekkoTiedot_ =
        std::pair<std::string, std::string>("","");
int MainWindow::gamePhase_ = 1;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    pelaajaYksi_("Pelaaja 1",this),  // Pelissä on vähintään kaksi pelaajaa


    pelaajaKaksi_("Pelaaja 2",this),
    pelaajaKolme_("",this),
    pelaajaNelja_("",this),
    pYksiPisteet_("0",this),        // Asetetaan kahdelle ekalle pelaajille
    pKaksiPisteet_("0",this),       // alkupisteiksi 0.
    pKolmePisteet_("",this),
    pneljaPisteet_("",this)

{
    ui->setupUi(this);
    alustaPelilauta();
    uusiPeli();


    // Piirretään spinwheel laudalle
    spinGraphics_ = new SpinWheelGraphics();
    spinView_->scene()->addItem(spinGraphics_);

    // Piirretään nuoli laudalle
    spinArrowGraphics_ = new SpinWheelArrowGraphics();
    spinView_->scene()->addItem(spinArrowGraphics_);

    // connectataan spinwheel
    connect(spinArrowGraphics_, SIGNAL(pyorita()),this,SLOT(pyoritaKiekkoa()));
}


MainWindow::~MainWindow()
{
    tyhjennaVanhaPeli();
    delete ui;
}


std::pair<std::string, std::string> MainWindow::getKiekkoTiedot()
{
    return kiekkoTiedot_;
}

int MainWindow::checkIfSpinning()

{
    int spinningPhase = 3;
    if(gamePhase_ == spinningPhase) {
        return 1;
    }
    return 0;
}

void MainWindow::lopetaPeli()
{
    // Pointteri itse tehtyyn gamestate luokkaan
    std::shared_ptr<Student::GameState> state =
            std::dynamic_pointer_cast<Student::GameState>(statePtr);

    int p1pisteet = state->getPoints(1);
    int p2pisteet = state->getPoints(2);
    int p3pisteet = state->getPoints(3);
    int p4pisteet = state->getPoints(4);

    lopetaPeliLabel_ = new QLabel();

    if (pelaajaMaara_ == 2) {
        lopetaPeliLabel_->setText(QString("PELI PÄÄTTYI \n"
                                   "PISTETILANNE: \n"
                                   "Pelaaja 1: ") + QString::number(p1pisteet) +
                                   QString("\nPelaaja 2: ") + QString::number(p2pisteet)
                                   );
    }
    else if (pelaajaMaara_ == 3) {
        lopetaPeliLabel_->setText(QString("PELI PÄÄTTYI \n"
                                   "PISTETILANNE: \n"
                                   "Pelaaja 1: ") + QString::number(p1pisteet) +
                                   QString("\nPelaaja 2: ") + QString::number(p2pisteet) +
                                    QString("\nPelaaja 3: ") + QString::number(p3pisteet)
                                   );
    }
    else if (pelaajaMaara_ == 4) {
        lopetaPeliLabel_->setText(QString("PELI PÄÄTTYI \n"
                                   "PISTETILANNE: \n"
                                   "Pelaaja 1: ") + QString::number(p1pisteet) +
                                   QString("\nPelaaja 2: ") + QString::number(p2pisteet) +
                                   QString("\nPelaaja 3: ") + QString::number(p3pisteet) +
                                   QString("\nPelaaja 4: ") + QString::number(p4pisteet)
                                   );
    }
    QFont font = lopetaPeliLabel_->font();
    font.setPointSize(40);
    font.setBold(true);
    lopetaPeliLabel_->setFont(font);
    lopetaPeliLabel_->setGeometry(QRect(-75,-110,650,700));
    view_->scene()->addWidget(lopetaPeliLabel_);


}


void MainWindow::uusiPeli()
{
    Student::hexGraphics::nollaaTila();
    // Tyhennetään mahdollisen edellisen pelin tiedot
    tyhjennaVanhaPeli();
    // Aloitusdialogi joka kysyy käyttäjältä pelaajamäärän.
    Dialog dialog_window;
    // aloitus ja lopetusnapit
    connect(&dialog_window,SIGNAL(accepted()),this,SLOT(show()));
    connect(&dialog_window,SIGNAL(rejected()),this,SLOT(close()));
    dialog_window.exec();
    // Otetaan uusi pelaajamäärä talteen...
    pelaajaMaara_ = dialog_window.getPlayerAmount();
    // ... ja luodaan sen mukaan pelaajia
    for (int pelaaja = 1; pelaaja <= pelaajaMaara_; pelaaja++) {
        playerVector.push_back(std::make_shared<Student::Player>
                               (pelaaja,PELAAJIENNAPPULAMAARA));
    }

    // Asetetaan aluksi kiekkotiedot tyhjiksi stringeiksi
    kiekkoTiedot_.first = "";
    kiekkoTiedot_.second = "";

    // Merkitaan pelilaudalle oikea info
    ui->vuoroLabel->setText("Pelaajan 1 vuoro");
    ui->mitaTehda->setText(VAIHDE1TEKSTI);

    // Mikäli pelaajia on enemmän kuin 2, asetetaan pelaajalabeliin teksti
    if (pelaajaMaara_ > 2) {
        pelaajaKolme_.setText("Pelaaja 3");
        pKolmePisteet_.setText("0");
    }
    if (pelaajaMaara_ > 3) {
        pelaajaNelja_.setText("Pelaaja 4");
        pneljaPisteet_.setText("0");
    }

    // Alustetaan boardPtr ja statePtr ja integroidaan koodi kurssin puolen koodiin
    boardPtr = std::make_shared<Student::GameBoard>();
    statePtr = std::make_shared<Student::GameState>();
    runnerPtr = Common::Initialization::getGameRunner
            (boardPtr, statePtr, playerVector);

    // Alustetaan pelaajat gamestatessa
    std::shared_ptr<Student::GameState> state =
            std::dynamic_pointer_cast<Student::GameState>(statePtr);
    state->initializePlayers(playerVector);

    // Annetaan vuoro pelaajalle 1 ja annetaan hänelle 3 liikettä
    int liikkeitaPerVuoro = 3;
    statePtr->changePlayerTurn(1);
    playerVector.at(0)->setActionsLeft(liikkeitaPerVuoro);
    statePtr->changeGamePhase(Common::GamePhase::MOVEMENT);
    gamePhase_ = 1;

    // Merkataan pelilaudalle oikea info
    ui->vuoroLabel->setText("Pelaajan 1 vuoro");
    ui->mitaTehda->setText(VAIHDE1TEKSTI);

    // Pirretään hexit laudalle
    spawnObject();

    // Luodaan pelaajien nappulat
    int nappulanId = 1;
    for (int pelaajaID = 1; pelaajaID <= pelaajaMaara_ ; pelaajaID++) {
        for(unsigned int nappulaID = 1; nappulaID <=
            PELAAJIENNAPPULAMAARA ; ++nappulaID) {

            std::vector<Student::hexGraphics*>::iterator randIt;

            // Arvotaan nappulan sijainti, ja varmistetaan
            // ettei se tule vesihexaan tai hexaan jossa on joku muu
            // nappula
            bool x = true;
            while(x) {
                randIt = hexGraphics_.begin();
                std::advance(randIt, rand()%hexGraphics_.size());
                if ( (*randIt)->getHex()->getPieceType() != "Water" &&
                     (*randIt)->getHex()->getPawnAmount() == 0  &&
                     (*randIt)->getHex()->getPieceType() != "Coral") {
                    x = false;
                }
            }

            // Lisätään nappulat gameboardiin
            boardPtr->addPawn(pelaajaID,nappulanId,(*randIt)->
                              getHex()->getCoordinates());

            // Lisätään nappulat gamestateen
            state->addPawn(pelaajaID,nappulanId);

            ++nappulanId;
        }
    }

    // Piirretään nappulat pelilaudalle
    spawnPawns();

    // Piirretään pelilaudalle kurssin puolesta muodostetut 1
    // vene / pelaaja
    spawnTransport();

}


void MainWindow::vaihdaPeliTilaa()
{

    // Pointteri itse tehtyyn gamestate luokkaan
    std::shared_ptr<Student::GameState> state =
            std::dynamic_pointer_cast<Student::GameState>(statePtr);

    // Tarkistetaan päättyykö peli
    for (int i = 1; i <= pelaajaMaara_; i++) {
        if(state->playerPawnsLeft(i) == 0) {
            updatePoints(i);
            lopetaPeli();
            return;
        }
    }

    Common::GamePhase nykyinenVaihe;
    nykyinenVaihe = statePtr->currentGamePhase();
    Common::GamePhase seurVaihe;



    // Muutetaan pelitilaa, tai mahdollisesti seuraavan pelaajan vuoroon.
    if ( nykyinenVaihe == Common::GamePhase::MOVEMENT ) {
        // nollataan aktivoidut hexat
        Student::hexGraphics::nollaaTila();

        // Päivitetään pelaajien piste labelit
        updatePoints(state->currentPlayer());

        seurVaihe = Common::GamePhase::SINKING;
        ui->mitaTehda->setText(VAIHDE2TEKSTI);

        gamePhase_ = 2;

    } else if ( nykyinenVaihe == Common::GamePhase::SINKING) {
        seurVaihe = Common::GamePhase::SPINNING;
        ui->mitaTehda->setText(VAIHDE3TEKSTI);
        gamePhase_ = 3;

    } else if ( nykyinenVaihe == Common::GamePhase::SPINNING ) {
        // Tässä vaihtuu pelivuoro seuraavalle pelaajalle.
        int nykyinenPelaaja = statePtr->currentPlayer();
        int seuraavaPelaaja;   // Alustetaan seuraavan pelaajan muuttuja.
        // Tarkistetaan että onko nykyinen pelaaja viimeinen pelaaja...
        if (nykyinenPelaaja == pelaajaMaara_) {
            seuraavaPelaaja = 1;  // ...tällöin aloitetaan uusi kierros
        } else {  // Muuten vuoro siirtyy seuraavalle.
            seuraavaPelaaja = nykyinenPelaaja + 1;
        }
        statePtr->changePlayerTurn(seuraavaPelaaja);

        // Asetetaan uudelle pelaajalle oikea pelivaihe.
        seurVaihe = Common::GamePhase::MOVEMENT;

        // Asetetaan liikkeiden määräksi 3
        playerVector.at(seuraavaPelaaja -1)->setActionsLeft(3);

        // Muutetaan myös pelaajainfo oikeaksi.
        ui->mitaTehda->setText(VAIHDE1TEKSTI);
        QString seurPelaaja = "Pelaajan " + QString::number(seuraavaPelaaja)
                + " vuoro";
        ui->vuoroLabel->setText(seurPelaaja);
        gamePhase_ = 1;
    }
    statePtr->changeGamePhase(seurVaihe);  // Lopulta asetetaan oikea pelivaihe.

}

void MainWindow::pyoritaKiekkoa()
{
    // Ensin tarkistetaan että onko oikea gamephase tälle, muuten return
    Common::GamePhase nykyinenVaihde = statePtr->currentGamePhase();

    // Muuten pyöritetään kiekkoa
    kiekkoTiedot_ = runnerPtr->spinWheel();
    QString elain = QString::fromStdString(kiekkoTiedot_.first);
    QString siirrot = QString::fromStdString(kiekkoTiedot_.second);

}

void MainWindow::deletePawnsFromCube(Common::CubeCoordinate coord)
{
    // pointteri gamestateen
    std::shared_ptr<Student::GameState> state =
            std::dynamic_pointer_cast<Student::GameState>(statePtr);

    // Haetaan pawnGraphic
    for (auto iter = pawnGraphics_.begin(); iter != pawnGraphics_.end(); iter++) {
        QPointF pawnCoords = (*iter)->pos();

        Common::CubeCoordinate pawnCubeCoords =
                (*iter)->getPawnPointer()->getCoordinates();
        // PawnGraph on tuhottavassa hexassa
        if(coord == pawnCubeCoords) {
            auto vec = boardPtr->getHex(coord)->getPawns();
            int tuhotaanko = 1;
            for (auto it = vec.begin(); it != vec.end(); it++) {
                if((*it)->getId() == (*iter)->getId()) {
                    tuhotaanko = 0;
                }
            }
            // Jos pawni ei ole enää hexassa se tuhotaan
            if(tuhotaanko == 1) {
                // Poistetaan pawn pelilaudalta
                //boardPtr->removePawn((*iter)->getId());
                // Poistetaan se itse tehdystä gamestate luokasta
                state->deletePawn((*iter)->getId());
                // Poistetaan se scenestä
                view_->scene()->removeItem(*iter);
            }
        }
    }
}

void MainWindow::deleteTransportsFromCube(Common::CubeCoordinate coord)
{
    // Transport grafiikat
    auto vector = Student::TransportGraphics::getTransportVector();

    //Poistetaan kaikki transportterit hexasta
    for (auto iter = vector.begin(); iter != vector.end(); ++iter) {
        if((*iter)->getTransportPtr()->getHex()->getCoordinates() == coord) {
            //boardPtr->removeTransport((*iter)->getTransportId());
            view_->scene()->removeItem(*iter);
        }
    }
}

void MainWindow::deleteActorsFromCube(Common::CubeCoordinate coord)
{
    // aktoriden grafiikat
    auto vector = Student::ActorGraphics::getActorGraphVector();

    // Poistetaan kaikki aktorit hexasta
    for (auto iter = vector.begin(); iter != vector.end(); ++iter) {
        if((*iter)->getActorPtr()->getHex()->getCoordinates() == coord) {
            view_->scene()->removeItem(*iter);
        }
    }
}


void MainWindow::alustaPelilauta()
{
    // Uusi peli ja lopeta peli nappulat
    connect(ui->lopetaPeliButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->uusiPeliButton, SIGNAL(clicked()), this, SLOT(uusiPeli()));

    // QPushButton joka vaihtaa joko pelaajan tehtävää tai seuraavaan pelaajaan
    connect(ui->vaihdaVaiheButton, SIGNAL(clicked(bool)),this,
            SLOT(vaihdaPeliTilaa()));

    // Muodostetaan pelilauta, pääikkunan oikealle puolellebreak()
    QGraphicsScene* scene = new QGraphicsScene;
    view_ = new Student::CustomView(this);
    view_->setScene(scene);
    view_->setMouseTracking(true);
    int leveys = 500;
    int korkeus = 400;
    scene->setSceneRect(0,0,leveys,korkeus);

    ui->gridLayout->addWidget(view_);

    // Muodostetaan alue spinwheelille
    QGraphicsScene *spinScene = new QGraphicsScene;
    spinView_ = new QGraphicsView(this);
    spinView_->setScene(spinScene);
    spinView_->setBackgroundBrush(Qt::lightGray);
    int spinMitat = 100;
    spinScene->setSceneRect(0,0,spinMitat,spinMitat);
    ui->gridSpenWheel->addWidget(spinView_);

    // Asetetaan pelaajien ID sekä pelaajien pistelabelit "pelaajienPisteet" -
    // layoutiin
    ui->pelaajienPisteet->addWidget(&pelaajaYksi_,0,0,1,1);
    ui->pelaajienPisteet->addWidget(&pelaajaKaksi_,1,0,1,1);
    ui->pelaajienPisteet->addWidget(&pelaajaKolme_,2,0,1,1);
    ui->pelaajienPisteet->addWidget(&pelaajaNelja_,3,0,1,1);
    ui->pelaajienPisteet->addWidget(&pYksiPisteet_,0,1,1,1);
    ui->pelaajienPisteet->addWidget(&pKaksiPisteet_,1,1,1,1);
    ui->pelaajienPisteet->addWidget(&pKolmePisteet_,2,1,1,1);
    ui->pelaajienPisteet->addWidget(&pneljaPisteet_,3,1,1,1);

}


void MainWindow::tyhjennaVanhaPeli()
{
    // Poistetaan vanhat board, gamestate ja runner luokat
    boardPtr = nullptr;
    statePtr = nullptr;
    runnerPtr = nullptr;

    // Tyhjennetään playervector ja poistetaan pelaajat
    for (auto iter = playerVector.begin(); iter != playerVector.end();
         ++iter) {
        (*iter) = nullptr;
    }

    // tyhjennetään pawnit pelilaudalta
    // ja poistetaan pawnGraphic oliot
    for ( auto iter = pawnGraphics_.begin(); iter !=
          pawnGraphics_.end(); ++iter) {
        view_->scene()->removeItem(*iter);
        delete *iter;
    }
    pawnGraphics_.clear();
    Student::pawnGraphics::clearPawnVector();

    // Tyhjennetään hexat pöydältä ja poistetaan
    // hexgraphics oliot
    for (auto iter = hexGraphics_.begin(); iter != hexGraphics_.end(); iter++) {
        view_->scene()->removeItem(*iter);
        delete *iter;
    }
    hexGraphics_.clear();

    // Tyhjennetään pelaajavektori
    playerVector.clear();

    // Tyhjennetään transport vectori
    auto vektori = Student::TransportGraphics::getTransportVector();
    for(auto iter = vektori.begin(); iter != vektori.end(); iter++) {
        view_->scene()->removeItem(*iter);
        delete *iter;
    }
    Student::TransportGraphics::ClearTransportVector();

    // Tuhotaan aktorit
    auto actorVektor = Student::ActorGraphics::getActorGraphVector();
    for(auto iter = actorVektor.begin(); iter != actorVektor.end(); iter++) {
        view_->scene()->removeItem(*iter);
        delete *iter;
    }
    Student::ActorGraphics::clearActorGrahpVector();

    // Nollataan edellisen pelin pisteet 1 ja 2 pelaajalle ...
    pYksiPisteet_.setText("0");
    pKaksiPisteet_.setText("0");
    // ... ja piilotetaan 3 ja 4 pelaajan pisteet ja pelaajanimet
    pKolmePisteet_.setText("");
    pneljaPisteet_.setText("");
    pelaajaKolme_.setText("");
    pelaajaNelja_.setText("");
    // Väritetään pelaajalabelit vastaamaan pawnien värejä
    pelaajaYksi_.setStyleSheet("QLabel {color: red;}");
    pelaajaKaksi_.setStyleSheet("QLabel {color: black;}");
    pelaajaKolme_.setStyleSheet("QLabel {color: darkCyan;}");
    pelaajaNelja_.setStyleSheet("QLabel {color: magenta;}");

    // Poistetaan lopeta peli label
    delete lopetaPeliLabel_;
}


void MainWindow::spawnTransport()
{
    // Käydään hexit läpi ja jos niissä on transport, niin piirretään se
    // pelilaudalle
    for (auto iter = hexGraphics_.begin(); iter != hexGraphics_.end(); iter++) {
        if ((*iter)->getHex()->getTransports().size() == 1) {
            Student::TransportGraphics* newTransport =
                   new Student::TransportGraphics((*iter)->getHex()->getTransports().at(0),
                                         (*iter)->getHex()->getCoordinates());
            view_->scene()->addItem(newTransport);
            newTransport->updatePos((*iter)->getHex()->getCoordinates());
        }
    }
}


void MainWindow::updatePoints(int playerId)
{
    std::shared_ptr<Student::GameState> state =
            std::dynamic_pointer_cast<Student::GameState>(statePtr);
    int points = state->getPoints(statePtr->currentPlayer());
    if (playerId == 1) {
        pYksiPisteet_.setText(QString::number(points));
    }
    else if (playerId == 2) {
        pKaksiPisteet_.setText(QString::number(points));
    }
    else if (playerId == 3) {
        pKolmePisteet_.setText(QString::number(points));
    }
    else if (playerId == 4) {
        pneljaPisteet_.setText(QString::number(points));
    }
}


// Funktio piirtää pelilaudan hexagonit näytölle
void MainWindow::spawnObject()
{    
    int map_radius = 7; // Kartan koko

    // Haetaan kartalle oikea muoto
    for( int x = -map_radius; x <= map_radius; x++ ) {
        int y1 = std::max(-map_radius, -x - map_radius);
        int y2 = std::min(map_radius, -x + map_radius);

        // Luodaan hexGraphics olio ja lisätään se scenen
        for ( int y = y1; y<=y2; y++) {
            auto hex = boardPtr->getHex(Common::CubeCoordinate(x,y,-x-y));

            Student::hexGraphics* hexgraph = new Student::hexGraphics(hex, runnerPtr, playerVector, view_, statePtr, boardPtr);
            hexGraphics_.push_back(hexgraph);
            view_->scene()->addItem(hexgraph);
            connect(hexgraph,SIGNAL(vaihdaPeliTilaa()),this,SLOT(vaihdaPeliTilaa()));
            connect(hexgraph,SIGNAL(deletePawnsFromCube(Common::CubeCoordinate)),
                    this,SLOT(deletePawnsFromCube(Common::CubeCoordinate)));
            connect(hexgraph,SIGNAL(deleteTransportsFromCube(Common::CubeCoordinate)),
                    this,SLOT(deleteTransportsFromCube(Common::CubeCoordinate)));
            connect(hexgraph,SIGNAL(deleteActorsFromCube(Common::CubeCoordinate)),
                    this,SLOT(deleteActorsFromCube(Common::CubeCoordinate)));

        }
    }
}


// Funktio piirtää pelaajien nappulat näytölle
void MainWindow::spawnPawns()
{
    // Käydään läpi kaikki hexat ja tutkitaan onko niissä pelaajan nappulaa
    for (auto iter = hexGraphics_.begin(); iter != hexGraphics_.end(); iter++) {
        if ((*iter)->getHex()->getPawnAmount() == 1) {
            Student::pawnGraphics* pawnGraph = new Student::pawnGraphics(
                        (*iter)->getHex()->getPawns().at(0));
            view_->scene()->addItem(pawnGraph);
            pawnGraph->updatePos(1);
            pawnGraphics_.push_back(pawnGraph);
        }
    }
}
