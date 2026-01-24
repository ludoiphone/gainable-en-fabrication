#include "NewsBanner.hpp"
#include <QHBoxLayout>
#include <QScrollBar>
#include <QFontMetrics>
#include <QNetworkRequest>
#include <QXmlStreamReader>

NewsBanner::NewsBanner(QWidget *parent)
    : QWidget(parent),
      m_currentIndex(0),
      m_scrollOffset(0),
      m_textWidth(0),
      m_scrollSpeed(0.8)
{
    setFixedHeight(33);
    setStyleSheet("background-color:black;");

    setupUI();

    // Timers
    m_newsTimer = new QTimer(this);
    m_scrollTimer = new QTimer(this);
    m_fetchTimer = new QTimer(this);

    connect(m_newsTimer, &QTimer::timeout, this, &NewsBanner::handleNextNews);
    connect(m_scrollTimer, &QTimer::timeout, this, &NewsBanner::handleScrollStep);
    connect(m_fetchTimer, &QTimer::timeout, this, &NewsBanner::fetchNews);

    // Network
    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, &QNetworkAccessManager::finished, this, &NewsBanner::onNewsReply);

    // Démarrage
    m_fetchTimer->start(600000); // fetch toutes les 10 minutes
    fetchNews(); // fetch initial
}

NewsBanner::~NewsBanner()
{
    m_newsTimer->stop();
    m_scrollTimer->stop();
    m_fetchTimer->stop();
}

void NewsBanner::setupUI()
{
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    // CCP container
    m_ccpContainer = new QWidget(this);
    m_ccpContainer->setFixedHeight(33);
    m_ccpContainer->setStyleSheet("background:black;");
    QHBoxLayout *ccpLayout = new QHBoxLayout(m_ccpContainer);
    ccpLayout->setContentsMargins(0,0,0,0);
    ccpLayout->setSpacing(0);

    // Logo "C"
    m_cLabel = new QLabel("C", m_ccpContainer);
    m_cLabel->setFont(QFont("Arial", 18, QFont::Black));
    m_cLabel->setAlignment(Qt::AlignCenter);
    m_cLabel->setStyleSheet("color:white; background:black;");
    m_cLabel->setFixedWidth(38);
    ccpLayout->addWidget(m_cLabel);

    // Label rouge "NEWS"
    m_newsLabelTitle = new QLabel("NEWS", m_ccpContainer);
    m_newsLabelTitle->setFont(QFont("Arial", 17, QFont::Black));
    m_newsLabelTitle->setAlignment(Qt::AlignCenter);
    m_newsLabelTitle->setStyleSheet("background:#D50000; color:white;");
    m_newsLabelTitle->setFixedWidth(95);
    m_newsLabelTitle->setFixedHeight(29); // hauteur légèrement inférieure à 33 pour laisser noir
    ccpLayout->addWidget(m_newsLabelTitle);

    // Petit séparateur noir
    QWidget *rightBlack = new QWidget(m_ccpContainer);
    rightBlack->setStyleSheet("background:black;");
    rightBlack->setFixedWidth(2);
    ccpLayout->addWidget(rightBlack);

    layout->addWidget(m_ccpContainer);

    // Scroll area pour texte défilant
    m_newsScrollArea = new QScrollArea(this);
    m_newsScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_newsScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_newsScrollArea->setFrameShape(QFrame::NoFrame);
    m_newsScrollArea->setWidgetResizable(true);
    m_newsScrollArea->setStyleSheet("background:white; border:none;");

    m_newsLabel = new QLabel(m_newsScrollArea);
    m_newsLabel->setFont(QFont("Arial", 13));
    m_newsLabel->setStyleSheet("color:black; padding-left:8px; background:transparent;");
    m_newsLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    m_newsScrollArea->setWidget(m_newsLabel);
    layout->addWidget(m_newsScrollArea, 1);
}

// ============================================================
// Fetch RSS
// ============================================================
void NewsBanner::fetchNews()
{
    QNetworkRequest req(QUrl("https://www.cnews.fr/rss/categorie/france"));
    req.setRawHeader("User-Agent", "Mozilla/5.0");
    m_manager->get(req);
}

// ============================================================
// Reception réponse RSS
// ============================================================
void NewsBanner::onNewsReply(QNetworkReply *reply)
{
    QStringList titles;
    QByteArray data = reply->readAll();
    reply->deleteLater();

    QXmlStreamReader xml(data);
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement() && xml.name() == "title") {
            QString t = xml.readElementText();
            if (!t.contains("CNEWS"))
                titles << t;
        }
    }

    if (titles.isEmpty())
        titles << "Pas de news disponibles";

    m_newsCache = titles;
    m_currentIndex = 0;
    if (!m_newsCache.isEmpty())
        updateNewsLabel(m_newsCache[m_currentIndex]);
}

// ============================================================
// Update texte affiché
// ============================================================
void NewsBanner::updateNewsLabel(const QString &news)
{
    m_newsTimer->stop();
    m_scrollTimer->stop();

    m_newsLabel->setText(news);
    QFontMetrics fm(m_newsLabel->font());
    m_textWidth = fm.horizontalAdvance(news);
    const int padding = 30;

    m_newsLabel->setFixedWidth(m_textWidth + padding);

    m_scrollOffset = 0;
    m_newsScrollArea->horizontalScrollBar()->setValue(0);

    int viewportW = m_newsScrollArea->viewport()->width();

    if (m_textWidth + padding <= viewportW) {
        // texte court, centré
        m_newsLabel->move((viewportW - (m_textWidth + padding)) / 2, 0);
        m_newsTimer->start(7000); // passe à la news suivante
    } else {
        // texte long, scroll
        m_newsLabel->move(0,0);
        m_scrollTimer->start(40);
    }
}

// ============================================================
// Timer → prochaine news
// ============================================================
void NewsBanner::handleNextNews()
{
    if (m_newsCache.isEmpty()) return;
    m_currentIndex = (m_currentIndex + 1) % m_newsCache.size();
    updateNewsLabel(m_newsCache[m_currentIndex]);
}

// ============================================================
// Timer → scroll texte
// ============================================================
void NewsBanner::handleScrollStep()
{
    int viewportW = m_newsScrollArea->viewport()->width();
    int maxOffset = m_textWidth - viewportW + 30; // padding

    m_scrollOffset += m_scrollSpeed;

    if (m_scrollOffset >= maxOffset) {
        m_scrollTimer->stop();
        QTimer::singleShot(2000, this, &NewsBanner::handleNextNews);
        return;
    }

    m_newsScrollArea->horizontalScrollBar()->setValue(int(m_scrollOffset));
}

