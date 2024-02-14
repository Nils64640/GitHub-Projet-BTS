#include <QObject>
#include <QtNetwork>

class HttpRest: public QObject
{
    Q_OBJECT
private slots:
    void httpFinished();
    void httpReadyRead();
public:
    HttpRest();
    ~HttpRest();
    void requeteRestPOST(QString url_str, QString data);
    QNetworkReply* getReply();
    QByteArray* getDataBuffer();
private:
    QNetworkAccessManager qnam;
    QNetworkReply* reply;
    QByteArray* dataBuffer;
};
//#endif // HTTPREST_H
