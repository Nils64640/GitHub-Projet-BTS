#include "httprest.h"

HttpRest::HttpRest():dataBuffer(new QByteArray){}
void HttpRest::requeteRestPOST(QString url_str, QString data)
{
    QByteArray message(data.toStdString().c_str());
    QByteArray postDataSize = QByteArray::number(message.size());
    QUrl url(url_str);

    // Préparation
QNetworkRequest request(url);
request.setRawHeader("User-Agent", "Essai POST");
request.setRawHeader("Content-Type", "application/json");
request.setRawHeader("Content-Length", postDataSize);

    // lancement requête POST
reply = qnam.post(request, message);

    // connexion signaux/slots pour réponse
connect(reply, &QIODevice::readyRead, this, &HttpRest::httpReadyRead);
connect(reply, &QNetworkReply::finished, this, &HttpRest::httpFinished);
}

    // Nouvelles données reçues
void HttpRest::httpReadyRead()
{
    dataBuffer->append(reply->readAll());
}

    // fin de réception
void HttpRest::httpFinished()
{
if (reply->error() == QNetworkReply::NoError) // Vérification si pas d'erreur
    {
qDebug()<< "Pas d'erreur ";
qDebug()<< reply->rawHeader("Content-Type");
    }
else qDebug()<< "Erreur: " << reply->error();

    // vider les buffers de réception
 reply->deleteLater();
 reply = Q_NULLPTR;
}

    // getter pour attribut private
QNetworkReply* HttpRest::getReply()
{
    return reply;
}

    // getter pour attribut private
QByteArray* HttpRest::getDataBuffer()
{
return dataBuffer;
}
HttpRest::~HttpRest(){}
