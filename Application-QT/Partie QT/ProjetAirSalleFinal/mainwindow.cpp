#include <QCoreApplication>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTime>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    httpRest(new HttpRest)

{

ui->setupUi(this);

    //Mise en place du chemin pour le background et de la couleur des QLabel
setWindowBackground("H:/PROJET_2023/ProjetAirSalleFinal/1041.jpg");

    // Customisation des Labels
 styleLabel(ui->label);
 styleLabel(ui->label_2);
 styleLabel(ui->label_3);
 styleLabel(ui->label_4);

    // Customisation des écrans LCD
 styleLCD(ui->CoV_lcdNumber);
 styleLCD(ui->temp_lcdNumber);
 styleLCD(ui->humid_lcdNumber);
 styleLCD(ui->Co2_lcdNumber);



    //Affectation des numéros de port et de la vitesse
  setWindowTitle(tr("HTTP POST"));
m_lecteur_port_serie.setPortName(PORT);
m_lecteur_port_serie.setBaudRate(VITESSE);


    //Erreur d'ouverture de port
if (!m_lecteur_port_serie.open(QIODevice::ReadWrite ))
    {
        qDebug() << "Échec de l'ouverture du port : ";
        qDebug() << PORT <<  "\n";
        qDebug() << "Type d'erreur : ";
        qDebug() << m_lecteur_port_serie.errorString() << "\n";
    }


    //Connexion : la fonction errorOccurred appelle la fonction DetecteErreur
connect(&m_lecteur_port_serie, &QSerialPort::errorOccurred,
            this, &MainWindow::DetecteErreur);
    //Connexion : la fonction readyRead appelle la fonction LitDonneesRecues
 connect(&m_lecteur_port_serie, &QSerialPort::readyRead,
            this, &MainWindow::LitDonneesRecues);


    //timer qui POST a 5 secondes d'intervalle
m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::Timer);
m_timer->start(5000);

}


/******************************************************PARTIE DESIGN FENETRE***********************************************************/


    // Customisation des écrans LCD
void MainWindow::styleLCD(QLCDNumber *lcd)
{
 lcd->setStyleSheet("QLCDNumber { background-color: grey; color: white; }");
 lcd->setSegmentStyle(QLCDNumber::Filled);
}

    // Customisation des Labels
void MainWindow::styleLabel(QLabel *label) {
    label->setStyleSheet

      (
        "QLabel {"
        "  border: 2px solid black;"  // bordure
        "  border-radius: 10px;"  // coins
        "  padding: 10px;"  // espace entre le text et les bordures
        "  margin: 10px;"  // espace
        "  background-color: white;"  // background couleur
        "  color: black;"  // text couleur
        "  font: bold 14px;"  // font style
        "}"
      );
}

    //Fonction pour la mise en place du background
void MainWindow::setWindowBackground(const QString &imagePath)
{
    QPixmap background(imagePath);
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);
}

    //fonction pour ajuster le background a la fenêtre
void MainWindow::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);
   setWindowBackground("H:/PROJET_2023/ProjetAirSalleFinal/1041.jpg");
}

/**************************************************************************************************************************************/


    //Identification d'erreur
void MainWindow::DetecteErreur(QSerialPort::SerialPortError erreur)
{
    //Confirmation, identification de l'erreur
    if (erreur == QSerialPort::ReadError)
    {
        qDebug() << "Erreur d'entrée-sortie en lecture du port : ";
        qDebug() << m_lecteur_port_serie.portName() << "\n";
        qDebug() << "Erreur : ";
        qDebug() << m_lecteur_port_serie.errorString() << "\n";

    //Sortie de l'application
QCoreApplication::exit(1);
    }
}

    //fonction pour lire les données du capteur
void MainWindow::LitDonneesRecues()
{

    QByteArray tableau_octets_lus;
    tableau_octets_lus.append(m_lecteur_port_serie.readAll());
    m_lecteur_port_serie.clear();
    qDebug()  << "Taille de la trame : " << tableau_octets_lus.size();
    if (tableau_octets_lus.size()<24) return;
    if (tableau_octets_lus.at(14)==0x0C)
    {
        m_temperature = tableau_octets_lus.at(9)*0.2;
        m_co2 = tableau_octets_lus.at(8)*10;
        m_humidite = tableau_octets_lus.at(7)*0.5;
    }
    if(tableau_octets_lus.at(14)==0x12)
        m_cov = tableau_octets_lus.at(8)*10;

    ui->temp_lcdNumber->display(m_temperature);
    qDebug()  << "Valeur temperature : " << m_temperature;
    ui->Co2_lcdNumber->display(m_co2);
    qDebug()  << "Valeur co2 : " << m_co2;
    ui->humid_lcdNumber->display(m_humidite);
    qDebug()  << "Valeur humidite : " << m_humidite;
    ui->CoV_lcdNumber->display(m_cov);
    qDebug()  << "Valeur cov : " << m_cov;
}

    //fonction pour la requete dans l'API
void MainWindow::post(QString designation, float valeur)
{
    QString data = "{";
    data.append("\"data\":");
    data.append(QString::number(valeur) );
    data.append(",");
    data.append("\"date\":");
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QTimeZone parisTimeZone("Europe/Paris");
    QDateTime parisDateTime = currentDateTime.toTimeZone(parisTimeZone);

    QString parisDateTimeString = parisDateTime.toString(Qt::ISODateWithMs);
    data.append("\"" + parisDateTimeString + "\"");
    data.append("}");
    QString requete = "http://10.0.5.154/api/" + designation;
qDebug()  << "Requete : " << requete + data;
    httpRest->requeteRestPOST(requete, data);

}


    // fin de réception
void MainWindow::restFinishedPOST()
{
    QByteArray *dataBuffer = httpRest->getDataBuffer();
    qDebug()<< dataBuffer->toStdString().c_str();
    dataBuffer->clear();
}

    //Fonction pour poster dans l'API avec une enumeration et un switch
void MainWindow::post()
{
    GRANDEUR grandeur=(GRANDEUR)m_grandeur;
    switch(grandeur)
    {
        case temperature:
            post("temperatures", m_temperature);
            break;
        case COV :
            post("hygrometries", m_humidite);
            break;
        case CO2 :
            post("c_o2s", m_co2);
            break;
        case humidite :
            post("c_o_vs", m_cov);
            break;
        default :
            m_grandeur = -1;
    }
    m_grandeur ++;
    qDebug()  << "Post n° : " << m_grandeur;
}


    //Fonction pour poster apres le timer fini (5s)
void MainWindow::Timer()
{
    post();
}


    //Destructeur
MainWindow::~MainWindow()
{
delete ui;
delete httpRest;
}

