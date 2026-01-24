#include "Gifs.hpp"

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
                    m_vitesseVent2Movie, m_vitesseVent4Movie, m_compresseurFroidMovie, m_compresseurChauffageMovie,
                    m_caniculeMovie, m_degivrageMovie, m_egouttageMovie, m_filtreANettoyerMovie, m_filtreNettoyageEnCourMovie,
                    m_filtreArretAbsoluMovie, m_saisonsMovie, m_eteMovie, m_hiverMovie, m_stabilisationMovie };

    for (QMovie* movie : movies) {
        if (movie) {
            movie->stop();
            delete movie;
        }
    }

    // Les labels seront détruits par Qt si parent défini, mais on peut les delete explicitement
    auto labels = { m_ventExtLabel, m_ventIntLabel, m_vitesseVent1LabelExt, m_vitesseVent1LabelInt,
                    m_vitesseVent2Label, m_vitesseVent4Label, m_compresseurFroidLabel, m_compresseurChauffageLabel,
                    m_caniculeLabel, m_degivrageLabel, m_egouttageLabel, m_filtreANettoyerLabel, m_filtreNettoyageEnCourLabel,
                    m_filtreArretAbsoluLabel, m_saisonsLabel, m_eteLabel, m_hiverLabel, m_stabilisationLabel };

    for (QLabel* label : labels) {
        if (label) delete label;
    }
}

void Gifs::setup()
{
    if (!parentWidget()) return;

    // Crée les GIFs avec positions adaptées
    createGif("ventilateurExt.gif",  m_ventExtLabel,          m_ventExtMovie,           QRect(542, 535, 60, 60));
    createGif("ventilateurInt.gif",  m_ventIntLabel,          m_ventIntMovie,           QRect(422, 535, 60, 60));
    createGif("vitesse1.gif",        m_vitesseVent1LabelExt,  m_vitesseVent1MovieExt,   QRect(602, 535, 60, 60));
    createGif("vitesse1.gif",        m_vitesseVent1LabelInt,  m_vitesseVent1MovieInt,   QRect(482, 535, 60, 60));
    createGif("vitesse2.gif",        m_vitesseVent2Label,     m_vitesseVent2Movie,      QRect(602, 535, 60, 60));
    createGif("vitesse4.gif",        m_vitesseVent4Label,     m_vitesseVent4Movie,      QRect(482, 535, 60, 60));
    createGif("froid.gif",           m_compresseurFroidLabel, m_compresseurFroidMovie,  QRect(662, 535, 60, 60));
    createGif("chauffage.gif",       m_compresseurChauffageLabel, m_compresseurChauffageMovie, QRect(662, 535, 60, 60));
    createGif("canicule.gif",        m_caniculeLabel,         m_caniculeMovie,          QRect(362, 535, 60, 60));
    createGif("degivrage.gif",       m_degivrageLabel,        m_degivrageMovie,         QRect(662, 535, 60, 60));
    createGif("egouttage.gif",       m_egouttageLabel,        m_egouttageMovie,         QRect(662, 535, 60, 60));
    createGif("filtreANettoyer.gif", m_filtreANettoyerLabel,  m_filtreANettoyerMovie,   QRect(302, 535, 60, 60));
    createGif("filtreNettoyageEnCour.gif", m_filtreNettoyageEnCourLabel, m_filtreNettoyageEnCourMovie, QRect(302, 535, 60, 60));
    createGif("filtreArretAbsolu.gif", m_filtreArretAbsoluLabel, m_filtreArretAbsoluMovie, QRect(392 ,180 , 240, 240));
    createGif("saisons.gif",         m_saisonsLabel,          m_saisonsMovie,           QRect(362, 535, 60, 60));
    createGif("ete.gif",             m_eteLabel,              m_eteMovie,               QRect(362, 535, 60, 60));
    createGif("hiver.gif",           m_hiverLabel,            m_hiverMovie,             QRect(362, 535, 60, 60));
    createGif("stabilisation.gif",   m_stabilisationLabel,    m_stabilisationMovie,     QRect(302, 535, 60, 60));
}

void Gifs::createGif(const QString &file, QLabel *&label, QMovie *&movie, const QRect &rect)
{
    label = new QLabel(parentWidget());
    label->setGeometry(rect);
    movie = new QMovie(basePath + file);
    movie->setCacheMode(QMovie::CacheAll);
    label->setMovie(movie);
    movie->start();
    movie->setPaused(true);
    label->hide();
}

void Gifs::startTimer()
{
    connect(m_timer, &QTimer::timeout, this, &Gifs::updateGifs);
    m_timer->start(250);
}

void Gifs::updateGifs()
{
    if (!m_pac) return;
    
    bool froidActif = (m_pac->getStatusUI().mode == MODE_FROID || m_pac->getStatusUI().mode == MODE_CANICULE) && m_pac->getStatusUI().compresseurActif;
    bool chauffageActif = m_pac->getStatusUI().mode == MODE_CHAUFFAGE && m_pac->getStatusUI().compresseurActif && !m_pac->getStatusUI().degivrageActif;
    bool filtreANettoyerActif  = m_pac->getStatusUI().filtreANettoyer || m_pac->getStatusUI().filtreArretAbsolu;
    bool caniculeActif = !m_pac->getStatusUI().transitionSaisons && m_pac->getStatusUI().canicule;
    bool hiverActif = !m_pac->getStatusUI().transitionSaisons && !m_pac->getStatusUI().canicule && m_pac->getStatusUI().hiver;
    bool eteActif = !m_pac->getStatusUI().transitionSaisons && !m_pac->getStatusUI().canicule && m_pac->getStatusUI().ete;

    m_compresseurFroidLabel->setVisible(froidActif);
    m_compresseurFroidMovie->setPaused(!froidActif);
    m_compresseurChauffageLabel->setVisible(chauffageActif);
    m_compresseurChauffageMovie->setPaused(!chauffageActif);
    
    m_ventExtLabel->setVisible(m_pac->getStatusUI().ventExtActif);
    m_ventExtMovie->setPaused(!m_pac->getStatusUI().ventExtActif);
    m_vitesseVent1LabelExt->setVisible(m_pac->getStatusUI().ventExtVitesse == VentExtSpeed::V1);
    m_vitesseVent1MovieExt->setPaused(m_pac->getStatusUI().ventExtVitesse == VentExtSpeed::OFF);
    m_vitesseVent2Label->setVisible(m_pac->getStatusUI().ventExtVitesse == VentExtSpeed::V2);
    m_vitesseVent2Movie->setPaused(m_pac->getStatusUI().ventExtVitesse == VentExtSpeed::OFF);

    m_ventIntLabel->setVisible(m_pac->getStatusUI().ventIntActif);
    m_ventIntMovie->setPaused(!m_pac->getStatusUI().ventIntActif);
    m_vitesseVent1LabelInt->setVisible(m_pac->getStatusUI().ventIntVitesse == VentIntSpeed::V1);
    m_vitesseVent1MovieInt->setPaused(m_pac->getStatusUI().ventIntVitesse == VentIntSpeed::OFF);
    m_vitesseVent4Label->setVisible(m_pac->getStatusUI().ventIntVitesse == VentIntSpeed::V4);
    m_vitesseVent4Movie->setPaused(m_pac->getStatusUI().ventIntVitesse == VentIntSpeed::OFF);

    m_degivrageLabel->setVisible(m_pac->getStatusUI().degivrageActif);
    m_degivrageMovie->setPaused(!m_pac->getStatusUI().degivrageActif);
    m_egouttageLabel->setVisible(m_pac->getStatusUI().egouttageActif);
    m_egouttageMovie->setPaused(!m_pac->getStatusUI().egouttageActif);
    
    m_filtreANettoyerLabel->setVisible(filtreANettoyerActif);
    m_filtreANettoyerMovie->setPaused(!filtreANettoyerActif);
    m_filtreNettoyageEnCourLabel->setVisible(m_pac->getStatusUI().filtreNettoyageEnCours);
    m_filtreNettoyageEnCourMovie->setPaused(!m_pac->getStatusUI().filtreNettoyageEnCours);
    m_filtreArretAbsoluLabel->setVisible(m_pac->getStatusUI().filtreArretAbsolu);
    m_filtreArretAbsoluMovie->setPaused(!m_pac->getStatusUI().filtreArretAbsolu);
    
    m_saisonsLabel->setVisible(m_pac->getStatusUI().transitionSaisons);
    m_saisonsMovie->setPaused(!m_pac->getStatusUI().transitionSaisons);
    m_caniculeLabel->setVisible(caniculeActif);
    m_caniculeMovie->setPaused(!caniculeActif);
    m_hiverLabel->setVisible(hiverActif);
    m_hiverMovie->setPaused(!hiverActif);
    m_eteLabel->setVisible(eteActif);
    m_eteMovie->setPaused(!eteActif);
    
    m_stabilisationLabel->setVisible(m_pac->getStatusUI().stabilisationActif);
    m_stabilisationMovie->setPaused(!m_pac->getStatusUI().stabilisationActif);
}

