#pragma once
#include <QWidget>
#include <QPushButton>
#include <QTouchEvent>
#include <QElapsedTimer>
#include <QRect>
#include "core/PacController.hpp"

class PacControl : public QWidget {
    Q_OBJECT
public:
    explicit PacControl(PacController *pac, QWidget *parent = nullptr);

signals:
    void demandeAfficheCons();  // signal vers MainWindow
    void pacDemarre();
    void pacArrete();

protected:
    bool event(QEvent *event) override;

private slots:
    void startPac();
    void stopPac();

private:
    void setupButtons();
    
    QPushButton *marche;
    QPushButton *stop;
    QPushButton *consigne;
    
    QRect zone1;
    QRect zone2;
    
    QElapsedTimer touchTimer;
    bool timerStarted = false;
    bool openAfficheCons;
    
    PacController *m_pac;
};

