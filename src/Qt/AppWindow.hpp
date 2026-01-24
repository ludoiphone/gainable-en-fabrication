#pragma once
#include <QStackedWidget>
#include <QScopedPointer>

class PacController;
class MainWindow;
class AfficheCons;
class ModifCons;
class ServiceTest;

class AppWindow : public QStackedWidget
{
    Q_OBJECT

public:
    explicit AppWindow(PacController *pac, QWidget *parent = nullptr);
    ~AppWindow();

public slots:
    void showMainWindow();
    void showServiceTest();
    void showModifCons();

private slots:
    void showAfficheCons();

private:
    PacController *m_pac;

    // Pages principales avec gestion automatique via QScopedPointer
    QScopedPointer<MainWindow> mainPage;
    QScopedPointer<AfficheCons> consPage;
    QScopedPointer<ModifCons> modifPage;
    QScopedPointer<ServiceTest> serviceTestPage;
    
    void switchTo(QWidget *page);
};

