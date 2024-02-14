#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include "httprest.h"
#include <QLCDNumber>
#include <QLabel>

#define PORT "COM3"
#define VITESSE QSerialPort::Baud57600

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
enum GRANDEUR{temperature,humidite,COV,CO2};

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::MainWindow *ui;
    HttpRest *httpRest;
    QSerialPort m_lecteur_port_serie;

public:

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
     void Attend(int ms);

private:

    float m_temperature = 0;
    float m_humidite = 0;
    float m_co2 = 0;
    float m_cov = 0;
    int m_grandeur=0;
    void post(QString designation, float valeur);
    void post();
    void setWindowBackground(const QString &imagePath);
    void resizeEvent(QResizeEvent* event);
    void styleLCD(QLCDNumber *lcd);
    void styleLabel(QLabel *label);
    QTimer *m_timer;

private slots:

    void Timer();
    void restFinishedPOST();
    void DetecteErreur(QSerialPort::SerialPortError erreur);
    void LitDonneesRecues();

};
#endif // MAINWINDOW_H
