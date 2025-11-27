#pragma once
#include "core/PacController.hpp"
#include <QStackedWidget>
#include <QScopedPointer>

class MainWindow;
class AfficheCons;
class ModifCons;

class AppWindow : public QStackedWidget
{
    Q_OBJECT

public:
    explicit AppWindow(PacController *pac, QWidget *parent = nullptr);
    ~AppWindow();

private slots:
    void showMainWindow();
    void showAfficheCons();
    void showModifCons();

private:
    PacController *m_pac;

    // Pages principales avec gestion automatique via QScopedPointer
    QScopedPointer<MainWindow> mainPage;
    QScopedPointer<AfficheCons> consPage;
    QScopedPointer<ModifCons> modifPage;
    
    void switchTo(QWidget *page);
};

