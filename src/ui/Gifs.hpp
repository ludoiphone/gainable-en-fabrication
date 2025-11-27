#pragma once
#include <QWidget>
#include <QLabel>
#include <QMovie>
#include <QTimer>
#include "core/PacController.hpp"

class Gifs : public QWidget
{
    Q_OBJECT
public:
    explicit Gifs(PacController *pac, QWidget *parent = nullptr);
    ~Gifs();

    void setup();      // initialisation des GIFs
    void startTimer(); // démarre le timer interne de mise à jour

private slots:
    void updateGifs();

private:
    PacController *m_pac;
    QTimer *m_timer;

    // Labels et Movies
    QLabel *m_ventExtLabel;
    QLabel *m_ventIntLabel;
    QLabel *m_vitesseVent1LabelExt;
    QLabel *m_vitesseVent1LabelInt;
    QLabel *m_vitesseVent2Label;
    QLabel *m_vitesseVent4Label;
    QLabel *m_compresseurFroidLabel;
    QLabel *m_compresseurChauffageLabel;
    QLabel *m_degivrageLabel;
    QLabel *m_egouttageLabel;

    QMovie *m_ventExtMovie;
    QMovie *m_ventIntMovie;
    QMovie *m_vitesseVent1MovieExt;
    QMovie *m_vitesseVent1MovieInt;
    QMovie *m_vitesseVent2Movie;
    QMovie *m_vitesseVent4Movie;
    QMovie *m_compresseurFroidMovie;
    QMovie *m_compresseurChauffageMovie;
    QMovie *m_degivrageMovie;
    QMovie *m_egouttageMovie;

    QString basePath = "/home/ludo/Desktop/test4/gifs/";

    void createGif(const QString &file, QLabel *&label, QMovie *&movie, const QRect &rect);
};

