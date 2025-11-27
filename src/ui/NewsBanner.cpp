#include "NewsBanner.hpp"
#include "logic/newsfetcher.hpp"

#include <QHBoxLayout>
#include <QScrollBar>
#include <QFont>
#include <QDebug>

NewsBanner::NewsBanner(QWidget *parent)
    : QWidget(parent),
      m_currentIndex(0),
      m_scrollOffset(0),
      m_textWidth(0),
      m_scrollSpeed(0.5)
{
    setFixedHeight(33);
    setStyleSheet("background-color:black;");

    setupUI();

    m_newsTimer = new QTimer(this);
    m_scrollTimer = new QTimer(this);

    startNewsThread();
}

NewsBanner::~NewsBanner()
{
    if (m_newsThread && m_newsThread->isRunning()) {
        m_newsThread->quit();
        m_newsThread->wait();
    }
}

void NewsBanner::setupUI()
{
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    // =====================================
    //         CCP ( LOGO C + NEWS )
    // =====================================

    m_ccpContainer = new QWidget(this);
    m_ccpContainer->setFixedHeight(33);
    m_ccpContainer->setStyleSheet("background:black;");

    QHBoxLayout *ccpLayout = new QHBoxLayout(m_ccpContainer);
    ccpLayout->setContentsMargins(0,0,0,0);
    ccpLayout->setSpacing(0);

    // --- LOGO "C" ---
    m_cLabel = new QLabel("C", m_ccpContainer);
    m_cLabel->setFont(QFont("Arial", 18, QFont::Black));
    m_cLabel->setAlignment(Qt::AlignCenter);
    m_cLabel->setStyleSheet("color:white; background:black;");
    m_cLabel->setFixedWidth(38);      // marge noire identique haut/bas/droite/gauche
    ccpLayout->addWidget(m_cLabel);

    // --- RECTANGLE ROUGE "NEWS" ---
    m_newsLabelTitle = new QLabel("NEWS", m_ccpContainer);
    m_newsLabelTitle->setFont(QFont("Arial", 17, QFont::Black));
    m_newsLabelTitle->setAlignment(Qt::AlignCenter);
    m_newsLabelTitle->setStyleSheet("background:#D50000; color:white;");
    m_newsLabelTitle->setFixedWidth(95); // taille fidèle au logo
    ccpLayout->addWidget(m_newsLabelTitle);

    // --- MARGE NOIRE ENTRE LOGO ET TICKER ---
    QWidget *rightBlack = new QWidget(m_ccpContainer);
    rightBlack->setStyleSheet("background:black;");
    rightBlack->setFixedWidth(2);
    ccpLayout->addWidget(rightBlack);

    layout->addWidget(m_ccpContainer);

    // =====================================
    //         TEXTE QUI DÉFILE
    // =====================================

    m_newsScrollArea = new QScrollArea(this);
    m_newsScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_newsScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_newsScrollArea->setFrameShape(QFrame::NoFrame);
    m_newsScrollArea->setWidgetResizable(true);
    m_newsScrollArea->setStyleSheet("background:white;");

    m_newsLabel = new QLabel(m_newsScrollArea);
    m_newsLabel->setFont(QFont("Arial", 10));
    m_newsLabel->setStyleSheet("color:black; padding-left:8px;");
    m_newsLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_newsLabel->setWordWrap(false);

    m_newsScrollArea->setWidget(m_newsLabel);

    layout->addWidget(m_newsScrollArea, 1);
}

void NewsBanner::startNewsThread()
{
    m_newsThread = new QThread(this);
    m_newsFetcher = new NewsFetcher();
    m_newsFetcher->moveToThread(m_newsThread);

    connect(m_newsThread, &QThread::started,
            m_newsFetcher, &NewsFetcher::start);

    connect(m_newsFetcher, &NewsFetcher::newsReady,
            this, &NewsBanner::onNewsReceived);

    connect(m_newsThread, &QThread::finished,
            m_newsFetcher, &QObject::deleteLater);

    m_newsThread->start();
}

void NewsBanner::onNewsReceived(QStringList list)
{
    m_newsCache = list;

    if (!m_newsCache.isEmpty())
        updateNewsLabel(m_newsCache[m_currentIndex]);
}

void NewsBanner::updateNewsLabel(const QString &news)
{
    m_newsLabel->setText(news);

    QFontMetrics fm(m_newsLabel->font());
    m_textWidth = fm.horizontalAdvance(news);
    const int padding = 30;

    m_newsLabel->setFixedWidth(m_textWidth + padding);
    m_scrollOffset = 0;

    m_newsScrollArea->horizontalScrollBar()->setValue(0);

    m_scrollTimer->stop();
    m_newsTimer->stop();

    int viewportW = m_newsScrollArea->viewport()->width();

    // Texte court → centré
    if (m_textWidth + padding <= viewportW)
    {
        m_newsLabel->move((viewportW - (m_textWidth + padding)) / 2, 0);

        connect(m_newsTimer, &QTimer::timeout, this, [this]() {
            m_currentIndex = (m_currentIndex + 1) % m_newsCache.size();
            updateNewsLabel(m_newsCache[m_currentIndex]);
        });
        m_newsTimer->start(10000);
    }
    else
    {
        m_newsLabel->move(0, 0);

        connect(m_scrollTimer, &QTimer::timeout, this, [this, viewportW, padding]() {
            m_scrollOffset += m_scrollSpeed;

            int maxOffset = m_textWidth - viewportW + padding;

            if (m_scrollOffset >= maxOffset)
            {
                m_scrollTimer->stop();
                QTimer::singleShot(2000, this, [this]() {
                    m_currentIndex = (m_currentIndex + 1) % m_newsCache.size();
                    updateNewsLabel(m_newsCache[m_currentIndex]);
                });
            }
            else
            {
                m_newsScrollArea->horizontalScrollBar()->setValue(int(m_scrollOffset));
            }
        });

        m_scrollTimer->start(30);
    }
}

