#ifndef MODIFCONS_H
#define MODIFCONS_H

#include <QtWidgets>
#include "consignes.h"

class ModifCons: public QWidget
{
    Q_OBJECT

public:
    ModifCons();
    
private:
    Consignes *m_modifConsignes;
    
    QWidget *m_window2;
    
    QComboBox *select;
    
    QDoubleSpinBox *m_selCons;
    
    double consSelect;
    double consMini;
    double consMaxi;
    
    void selectCons();
    void adjustCons();
    void saveCons();
    
    QLabel *m_label2;
    
    QPushButton *m_valid1;
    QPushButton *m_quit1;
    QPushButton *m_valid2;
};

#endif // MODIFCONS_H
