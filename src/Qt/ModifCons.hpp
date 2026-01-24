#pragma once
#include "logic/Consignes.hpp"
#include <QWidget>
#include <QVector>
#include <functional>
#include <QTouchEvent>
#include <QElapsedTimer>
#include <QRect>

class QLabel;
class QPushButton;
class QButtonGroup;
class QVBoxLayout;
class QGroupBox;
class QDoubleSpinBox;

struct ConsigneConfig {
    QString titre;
    std::function<double()> getValue;
    double minVal;
    double maxVal;
    double defaut;
    std::function<void(double)> save;
};

class ModifCons : public QWidget
{
    Q_OBJECT

public:
    explicit ModifCons(QWidget *parent = nullptr);
    ~ModifCons();
    
signals:
    void quitterDemande();
    void demandeServiceTest();

private:
    void createConsigneButtons();
    void selectCons(int index);
    void adjustCons();
    void clearHighlight();
    void saveCons();
    void closeConsigne();

    QLabel* m_background;
    QWidget* m_listContainer;
    QVBoxLayout* m_listLayout;

    QVector<QPushButton*> m_consigneButtons;
    QButtonGroup* m_buttonGroup;

    QGroupBox* m_groupBox;
    QLabel *m_titleLabel;
    QDoubleSpinBox* m_selCons;
    QLabel* m_lastValueLabel;
    QPushButton* m_validBtn;
    QPushButton* m_quitBtn;
    QPushButton* m_quitGlobalBtn;
    
    // Zones tactiles cachées
    QRect zone1;
    QRect zone2;
    
    // Gestion multi-touch
    QElapsedTimer touchTimer;
    bool timerStarted = false;
    bool serviceOpened = false;
    
    // Override événement
    bool event(QEvent* event) override;

    QScopedPointer<Consignes> m_modifConsignes;
    QVector<ConsigneConfig> configs;

    int currentIndex;
    double consSelect;
    double consMini;
    double consMaxi;
};


