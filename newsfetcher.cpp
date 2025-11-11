#include "newsfetcher.hpp"

#include <QNetworkReply>
#include <QXmlStreamReader>
#include <QDebug>

NewsFetcher::NewsFetcher(QObject *parent) : QObject(parent)
{
    m_manager = new QNetworkAccessManager(this);
    m_timer = new QTimer(this);

    connect(m_manager, &QNetworkAccessManager::finished, this, [=](QNetworkReply *reply){
        QStringList titles;
        QByteArray data = reply->readAll();

        QXmlStreamReader xml(data);
        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.isStartElement() && xml.name() == "title") {
                QString t = xml.readElementText();
                if (!t.contains("CNEWS"))
                    titles << t;
            }
        }

        if (xml.hasError())
            qDebug() << "Erreur XML :" << xml.errorString();

        if (titles.isEmpty())
            emit newsReady({"Pas de news disponibles."});
        else
            emit newsReady(titles);

        reply->deleteLater();
    });

    connect(m_timer, &QTimer::timeout, this, &NewsFetcher::fetchNews);
}

void NewsFetcher::start()
{
    fetchNews();            // premier appel
    m_timer->start(600000); // toutes les 10 minutes
}

void NewsFetcher::fetchNews()
{
    //QNetworkRequest req(QUrl("https://flipboard.com/@cnews_fr/actu-paris-qqrkg7fez.rss"));
    QNetworkRequest req(QUrl("https://www.cnews.fr/rss/categorie/france"));
    
    req.setRawHeader("User-Agent", 
                     "Mozilla/5.0 (X11; Linux armv7l) AppleWebKit/537.36 "
                     "(KHTML, like Gecko) Chrome/120.0 Safari/537.36");
    
    m_manager->get(req);
}
