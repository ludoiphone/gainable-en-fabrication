#include "MainWindow.hpp"
#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QFont>
#include <QTimer>
#include <QDebug>


void MainWindow::setupNewsBanner()
{
    // Widget principal du bandeau
    m_banner = new QWidget(this);
    m_banner->setGeometry(0, 0, 1024, 33);
    m_banner->setStyleSheet("background-color:black;");

    QHBoxLayout *bannerLayout = new QHBoxLayout(m_banner);
    bannerLayout->setContentsMargins(0,0,0,0);
    bannerLayout->setSpacing(0);

    // Icône C
    m_cLabel = new QLabel("C", m_banner);
    m_cLabel->setFont(QFont("Arial",16,QFont::Bold));
    m_cLabel->setAlignment(Qt::AlignCenter);
    m_cLabel->setStyleSheet("color:white; background-color:black;");
    m_cLabel->setFixedWidth(20);
    bannerLayout->addWidget(m_cLabel);

    // Bloc NEWS
    QWidget *newsContainer = new QWidget(m_banner);
    QVBoxLayout *newsLayout = new QVBoxLayout(newsContainer);
    newsLayout->setContentsMargins(0,2,2,2);

    m_newsLabelTitle = new QLabel("NEWS", newsContainer);
    m_newsLabelTitle->setFont(QFont("Arial",12,QFont::Bold));
    m_newsLabelTitle->setAlignment(Qt::AlignCenter);
    m_newsLabelTitle->setStyleSheet("color:white; background-color:red;");
    m_newsLabelTitle->setFixedWidth(80);
    newsLayout->addWidget(m_newsLabelTitle);
    bannerLayout->addWidget(newsContainer);

    // Zone de défilement
    m_newsScrollArea = new QScrollArea(m_banner);
    m_newsScrollArea->setStyleSheet("background-color:white;");
    m_newsScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_newsScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_newsScrollArea->setFrameShape(QFrame::NoFrame);

    m_newsLabel = new QLabel(m_newsScrollArea);
    m_newsLabel->setFont(QFont("Arial",10));
    m_newsLabel->setStyleSheet("color:black; background-color:white; padding-left:8px;");
    m_newsLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_newsLabel->setWordWrap(false);
    m_newsLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);

    m_newsScrollArea->setWidget(m_newsLabel);
    m_newsScrollArea->setWidgetResizable(false);
    bannerLayout->addWidget(m_newsScrollArea, 1);
}

// ===================== Initialisation des timers =====================
void MainWindow::setupNewsTimer()
{
    m_newsTimer = new QTimer(this);
    m_scrollTimer = new QTimer(this);
}

// ===================== Lancement du thread de News =====================
void MainWindow::startNewsThread()
{
    /*if (m_newsThread && m_newsThread->isRunning())
        return; // déjà démarré*/
    
    m_newsThread = new QThread(this);
    m_newsFetcher = new NewsFetcher();
    m_newsFetcher->moveToThread(m_newsThread);

    // Quand le thread démarre, on lance le fetcher
    connect(m_newsThread, &QThread::started, m_newsFetcher, &NewsFetcher::start);

    // Quand des news sont prêtes → mise à jour du bandeau
    connect(m_newsFetcher, &NewsFetcher::newsReady, this, [this](QStringList list) {
        m_newsCache = list;
        if (!m_newsCache.isEmpty())
            updateNewsLabel(m_newsCache[m_currentIndex]);
    });

    // Nettoyage quand le thread est fini
    connect(m_newsThread, &QThread::finished, m_newsFetcher, &QObject::deleteLater);

    m_newsThread->start();
}

// ===================== Mise à jour du texte =====================
void MainWindow::updateNewsLabel(const QString &news)
{
    m_newsLabel->setText(news);
    QFontMetrics fm(m_newsLabel->font());
    m_textWidth = fm.horizontalAdvance(news);
    const int padding = 30;
    m_newsLabel->setFixedWidth(m_textWidth + padding);
    m_newsLabel->setFixedHeight(m_banner->height());

    if (!m_newsScrollArea) return;

    int areaWidth = m_newsScrollArea->viewport()->width();

    // Réinitialisation complète des timers
    m_scrollTimer->stop();
    m_newsTimer->stop();
    m_scrollTimer->disconnect();
    m_newsTimer->disconnect();
    m_scrollOffset = 0;
    m_newsScrollArea->horizontalScrollBar()->setValue(0);

    if (m_textWidth + padding <= areaWidth) {
        // ========= Cas 1 : texte court =========
        m_newsLabel->move((areaWidth - (m_textWidth + padding)) / 2, 0);

        // Passe à la news suivante après 10s
        connect(m_newsTimer, &QTimer::timeout, this, [this]() {
            if (m_newsCache.isEmpty()) return;
            m_currentIndex = (m_currentIndex + 1) % m_newsCache.size();
            updateNewsLabel(m_newsCache[m_currentIndex]);
        });
        m_newsTimer->start(10000);

    } else {
        // ========= Cas 2 : texte long =========
        m_newsLabel->move(0, 0);

        const int interval = 30; // ms
        connect(m_scrollTimer, &QTimer::timeout, this, [this, areaWidth, padding]() {
            m_scrollOffset += m_scrollSpeed;
            int maxOffset = m_textWidth - areaWidth + padding;

            if (m_scrollOffset >= maxOffset) {
                m_scrollTimer->stop();

                QTimer::singleShot(2000, this, [this]() {
                    if (m_newsCache.isEmpty()) return;
                    m_currentIndex = (m_currentIndex + 1) % m_newsCache.size();
                    updateNewsLabel(m_newsCache[m_currentIndex]);
                });

            } else {
                m_newsScrollArea->horizontalScrollBar()->setValue(int(m_scrollOffset));
            }
        });
        m_scrollTimer->start(interval);
    }
}
