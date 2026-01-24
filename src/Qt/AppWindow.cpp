#include "AppWindow.hpp"
#include "Qt/MainWindow.hpp"
#include "Qt/AfficheCons.hpp"
#include "Qt/ModifCons.hpp"
#include "Qt/ServiceTest.hpp"
#include <QTimer>

#include <QDebug>

AppWindow::AppWindow(PacController *pac, QWidget *parent)
    : QStackedWidget(parent),
    m_pac(pac)
{
    // Création des pages principales
    mainPage.reset(new MainWindow(m_pac, this));
    consPage.reset(new AfficheCons(this));

    addWidget(mainPage.data());    // index 0
    addWidget(consPage.data());    // index 1

    setCurrentWidget(mainPage.data());

    // Connexions via signaux
    connect(mainPage.data(), &MainWindow::demandeAfficheCons, this, &AppWindow::showAfficheCons);
    connect(consPage.data(), &AfficheCons::demandeQuitter, this, &AppWindow::showMainWindow);
    connect(consPage.data(), &AfficheCons::demandeModif, this, &AppWindow::showModifCons);
}

AppWindow::~AppWindow() = default;

void AppWindow::switchTo(QWidget *page)
{
    if (!page || page == currentWidget())
        return;
    setCurrentWidget(page);
}

void AppWindow::showMainWindow()
{
    switchTo(mainPage.data());
}

void AppWindow::showAfficheCons()
{
    switchTo(consPage.data());
}

void AppWindow::showModifCons()
{
    // Supprimer l'ancienne page si elle existe
    if (!modifPage.isNull()) {
        removeWidget(modifPage.data());
        modifPage->deleteLater();
        modifPage.reset();
    }

    // Créer une nouvelle instance de ModifCons
    modifPage.reset(new ModifCons(this));
    addWidget(modifPage.data());

    // Connexion du signal quitterDemande vers AfficheCons
    connect(modifPage.data(), &ModifCons::quitterDemande, this, &AppWindow::showMainWindow);
    
    connect(modifPage.data(), &ModifCons::demandeServiceTest, this, &AppWindow::showServiceTest);

    switchTo(modifPage.data());
}

void AppWindow::showServiceTest()
{    
    if (!serviceTestPage.isNull()) {
        removeWidget(serviceTestPage.data());
        serviceTestPage->deleteLater();
        serviceTestPage.reset();
    }

    serviceTestPage.reset(new ServiceTest(m_pac, this));
    addWidget(serviceTestPage.data());

    connect(serviceTestPage.data(), &ServiceTest::demandeQuitter, this, [this]() {
        m_pac->serviceEnable(false);
        showMainWindow();
    });

    switchTo(serviceTestPage.data());
}

