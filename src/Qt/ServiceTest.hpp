#pragma once
#include <QWidget>
#include "Gifs.hpp"

class QPushButton;
class QLabel;
class QTimer;
class QGroupBox;

class PacController;

class ServiceTest : public QWidget
{
    Q_OBJECT

public:
    explicit ServiceTest(PacController* pac, QWidget* parent = nullptr);

signals:
    void demandeQuitter();

private:
    PacController *m_pac;
    
    QLabel *m_labelBackgroundServiceTest;
    
    QLabel *m_title;
    
    QPushButton *m_relaysBtn;
    QPushButton *m_gifsBtn;
    QPushButton *m_switchsBtn;
    QPushButton *m_quitBtn;
    
    QGroupBox *m_relaysBox;
    QPushButton *m_relayEteHiver;
    QPushButton *m_relayCompresseur;
    QPushButton *m_relayV4V;
    QPushButton *m_relayVentilateurExt;
    QPushButton *m_relayVitessesExt;
    QPushButton *m_relayVentilateurInt;
    QPushButton *m_relayVitessesInt;
    QPushButton *m_relayAllOff;
    
    QGroupBox *m_inputsBox;
    QLabel *m_thermostatLabel;
    QLabel *m_filtreLabel;
    QTimer *m_timerInputs;
    
    QTimer *m_timerGifs;
    Gifs *m_gifsWidget;
    int m_gifIndex;
    QList<QLabel*> m_allGifLabels;
    
    void setRelayBtn(QPushButton* btn, const QString& label, bool on);
    
    void updateInputs();
    void setState(QLabel* lbl, const QString& label, bool on, bool inverted = false);
    
    bool m_gifsRunning;
    void showNextGif();
    void hideAllGifs();
    
    void stopRelaysTest();
    void stopInputsTest();
    void stopGifsTest();
};
