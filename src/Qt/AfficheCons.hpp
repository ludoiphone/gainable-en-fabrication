#pragma once
#include "logic/Consignes.hpp"
#include <QWidget>
#include <QScopedPointer>

class QTimer;
class QLabel;
class QPushButton;
class QGraphicsDropShadowEffect;

class AfficheCons: public QWidget
{
    Q_OBJECT

public:
    explicit AfficheCons(QWidget *parent = nullptr);

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private slots:
    void readCons();
    void modifConsignes();

signals:
    void demandeModif();   // Émis quand on clique sur "Modif Consignes"
    void demandeQuitter(); // Émis quand on clique sur "Quitter"

private:
    QScopedPointer<Consignes> m_configCons;   // lecture des consignes (fichier.ini)

    QTimer *m_updateTimer = nullptr;
    int m_timerCons = 1000; // maj écran dispcons

    QLabel *m_labelBackground = nullptr;
    
    QLabel *m_titreCons[13];
    QLabel *m_afficheCons[13];
    
    QGraphicsDropShadowEffect *m_effects[13];

    QPushButton *m_modifC = nullptr;
    QPushButton *m_quitter = nullptr;
    
    QStringList keys;
};

