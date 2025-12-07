#pragma once

#include <QWidget>
#include <QLabel>
#include <QVector>
#include <QTimer>
#include "core/PacController.hpp"

class TemperatureWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TemperatureWidget(PacController *pac, QWidget *parent = nullptr);

private:
    struct TempDisplay {
        QLabel *title;
        QLabel *value;
    };

    QVector<TempDisplay> m_displays;
    PacController *m_pac;
    QTimer *m_updateTemps;
    
    void refreshTemperatures();
    
};

