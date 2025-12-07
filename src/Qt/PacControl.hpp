#pragma once
#include <QWidget>
#include <QPushButton>
#include "core/PacController.hpp"

class PacControl : public QWidget {
    Q_OBJECT
public:
    explicit PacControl(PacController *pac, QWidget *parent = nullptr);

signals:
    void demandeAfficheCons();  // signal vers MainWindow
    void pacDemarre();
    void pacArrete();

private slots:
    void startPac();
    void stopPac();

private:
    void setupButtons();
    
    QPushButton *marche;
    QPushButton *stop;
    QPushButton *consigne;
    
    
    PacController *m_pac;
};

