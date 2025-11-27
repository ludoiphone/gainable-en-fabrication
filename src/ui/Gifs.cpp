#include "Gifs.hpp"
#include <QDebug>

Gifs::Gifs(PacController *pac, QWidget *parent)
    : QWidget(parent),
      m_pac(pac)
{
    m_timer = new QTimer(this);
}

Gifs::~Gifs()
{
    // Arrêter et supprimer tous les movies
    auto movies = { m_ventExtMovie, m_ventIntMovie, m_vitesseVent1MovieExt, m_vitesseVent1MovieInt,
                    m_vitesseVent2Movie, m_vitesseVent4Movie, m_compresseurFroidMovie,
                    m_compresseurChauffageMovie, m_degivrageMovie, m_egouttageMovie };

    for (QMovie* movie : movies) {
        if (movie) {
            movie->stop();
            delete movie;
        }
    }

    // Les labels seront détruits par Qt si parent défini, mais on peut les delete explicitement
    auto labels = { m_ventExtLabel, m_ventIntLabel, m_vitesseVent1LabelExt, m_vitesseVent1LabelInt,
                    m_vitesseVent2Label, m_vitesseVent4Label, m_compresseurFroidLabel,
                    m_compresseurChauffageLabel, m_degivrageLabel, m_egouttageLabel };

    for (QLabel* label : labels) {
        if (label) delete label;
    }
}

void Gifs::setup()
{
    if (!parentWidget()) return;

    // Crée les GIFs avec positions adaptées
    createGif("ventilateurExt.gif",  m_ventExtLabel,          m_ventExtMovie,           QRect(490, 470, 120, 120));
    createGif("ventilateurInt.gif",  m_ventIntLabel,          m_ventIntMovie,           QRect(350, 490, 120, 120));
    createGif("vitesse1.gif",        m_vitesseVent1LabelExt,  m_vitesseVent1MovieExt,   QRect(450, 520, 60, 60));
    createGif("vitesse1.gif",        m_vitesseVent1LabelInt,  m_vitesseVent1MovieInt,   QRect(300, 520, 60, 60));
    createGif("vitesse2.gif",        m_vitesseVent2Label,     m_vitesseVent2Movie,      QRect(450, 520, 60, 60));
    createGif("vitesse4.gif",        m_vitesseVent4Label,     m_vitesseVent4Movie,      QRect(300, 520, 60, 60));
    createGif("froid.gif",           m_compresseurFroidLabel, m_compresseurFroidMovie,  QRect(600, 480, 120, 120));
    createGif("chauffe.gif",         m_compresseurChauffageLabel, m_compresseurChauffageMovie, QRect(610, 480, 100, 100));
    createGif("degivrage.gif",       m_degivrageLabel,        m_degivrageMovie,         QRect(610, 480, 120, 120));
    createGif("egouttage.gif",       m_egouttageLabel,        m_egouttageMovie,         QRect(525, 410, 200, 210));

    qDebug() << "[Gifs] setup terminé";
}

void Gifs::createGif(const QString &file, QLabel *&label, QMovie *&movie, const QRect &rect)
{
    label = new QLabel(parentWidget());
    label->setGeometry(rect);
    movie = new QMovie(basePath + file);
    movie->setCacheMode(QMovie::CacheAll);
    label->setMovie(movie);
    movie->start();
    label->hide();
}

void Gifs::startTimer()
{
    connect(m_timer, &QTimer::timeout, this, &Gifs::updateGifs);
    m_timer->start(200);
}

void Gifs::updateGifs()
{
    if (!m_pac) return;

    // Ventilateurs
    bool actifExt = m_pac->getVentilateurExt();
    bool actifInt = m_pac->getVentilateurInt();
    int vitesseExt = m_pac->getVitesseVentilateurExt();
    int vitesseInt = m_pac->getVitesseVentilateurInt();
    bool actifComp = m_pac->getCompresseur();
    bool actifDeg = m_pac->getDegivrage();
    bool actifEgou = m_pac->getEgouttage();
    ModePAC mode = m_pac->getModeActuel();

    m_ventExtLabel->setVisible(actifExt);
    m_ventIntLabel->setVisible(actifInt);
    actifExt ? m_ventExtMovie->start() : m_ventExtMovie->stop();
    actifInt ? m_ventIntMovie->start() : m_ventIntMovie->stop();

    m_vitesseVent1LabelExt->setVisible(actifExt && vitesseExt == 1);
    m_vitesseVent2Label->setVisible(actifExt && vitesseExt == 2);
    (actifExt && vitesseExt == 1) ? m_vitesseVent1MovieExt->start() : m_vitesseVent1MovieExt->stop();
    (actifExt && vitesseExt == 2) ? m_vitesseVent2Movie->start() : m_vitesseVent2Movie->stop();

    m_vitesseVent1LabelInt->setVisible(actifInt && vitesseInt == 1);
    m_vitesseVent4Label->setVisible(actifInt && vitesseInt == 4);
    (actifInt && vitesseInt == 1) ? m_vitesseVent1MovieInt->start() : m_vitesseVent1MovieInt->stop();
    (actifInt && vitesseInt == 4) ? m_vitesseVent4Movie->start() : m_vitesseVent4Movie->stop();

    if (mode == MODE_FROID) {
        m_compresseurFroidLabel->setVisible(actifComp);
        m_compresseurChauffageLabel->hide();
        actifComp ? m_compresseurFroidMovie->start() : m_compresseurFroidMovie->stop();
    } else if (mode == MODE_CHAUFFAGE) {
        m_compresseurChauffageLabel->setVisible(actifComp);
        m_compresseurFroidLabel->hide();
        actifComp ? m_compresseurChauffageMovie->start() : m_compresseurChauffageMovie->stop();
    } else {
        m_compresseurFroidLabel->hide();
        m_compresseurChauffageLabel->hide();
    }

    m_degivrageLabel->setVisible(actifDeg);
    actifDeg ? m_degivrageMovie->start() : m_degivrageMovie->stop();

    m_egouttageLabel->setVisible(actifEgou);
    actifEgou ? m_egouttageMovie->start() : m_egouttageMovie->stop();
}

