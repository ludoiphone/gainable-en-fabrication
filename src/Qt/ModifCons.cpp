#include "ModifCons.hpp"

#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QDoubleSpinBox>
#include <QGraphicsDropShadowEffect>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPixmap>
#include <QFont>

ModifCons::ModifCons(QWidget *parent)
    : QWidget(parent),
      currentIndex(-1)
{
    setFixedSize(1024, 600);
    
    // --- Background ---
    m_background = new QLabel(this);
    m_background->setPixmap(QPixmap("/home/ludo/Desktop/PAC2/images/modifCons.jpg"));
    m_background->setGeometry(0, 0, 1024, 600);
    m_background->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_background->lower();
    
    setAttribute(Qt::WA_AcceptTouchEvents);
    
    // Zones tactiles invisibles
    zone1 = QRect(600, 544, 128, 50);
    zone2 = QRect(735, 544, 128, 50);

    // --- Initialisation des consignes ---
    m_modifConsignes.reset(new Consignes("/home/ludo/Desktop/PAC2/config/consignes.ini"));
    m_modifConsignes->update();

    m_listContainer = new QWidget(this);
    m_listContainer->setGeometry(26, 0, 550, 600);
    m_listLayout = new QVBoxLayout(m_listContainer);
    m_listLayout->setSpacing(3);
    m_listLayout->setContentsMargins(0, 3, 0, 3);

    m_buttonGroup = new QButtonGroup(this);
    m_buttonGroup->setExclusive(true);

    auto addConfig = [this](const QString &titre, const std::string &key,
                            double minV, double maxV, double def) {
        configs.append({
            titre,
            [this, key]() -> double { return m_modifConsignes->get(key); },
            minV,
            maxV,
            def,
            [this, key](double v) { m_modifConsignes->set(key, v); }
        });
    };

    // --- consignes cohérentes ---
    addConfig("Eté / Hiver", "ConsigneEteHiver", 10.0, 16.0, 13.5);
    addConfig("Canicule", "ConsigneCanicule", 28.0, 32.0, 30.0);
    addConfig("Delta", "ConsigneDelta", 6.0, 10.0, 6.0);
    addConfig("Blocage Chauffage", "ConsigneBlocageChauffage", 9.0, 13.0, 11.0);
    addConfig("Grande Vitesse Ext Chauffage", "ConsigneGrandeVitesseExterieurChauffage", 0.0, 3.0, 6.0);
    addConfig("Départ Ventilation Int Chauffage", "ConsigneDepartVentilationInterieurChauffage", 30.0, 40.0, 35.0);
    addConfig("Petite Vitesse Int Chauffage", "ConsignePetiteVitesseInterieurChauffage", 21.0, 25.0, 23.0);
    addConfig("Fin Dégivrage Chauffage", "ConsigneFinDegivrageChauffage", 10.0, 14.0, 12.5);
    addConfig("Petite Vitesse Ext Froid", "ConsignePetiteVitesseExterieurFroid", 14.0, 18.0, 16.0);
    addConfig("Grande Vitesse Ext Froid", "ConsigneGrandeVitesseExterieurFroid", 16.0, 22.0, 20.0);
    addConfig("Petite Vitesse Int Froid", "ConsignePetiteVitesseInterieurFroid", 21.0, 25.0, 23.0);
    addConfig("Départ Dégivrage Froid", "ConsigneDepartDegivrageFroid", -5.0, -1.0, -3.0);
    addConfig("Fin Dégivrage Froid", "ConsigneFinDegivrageFroid", 13.0, 17.0, 15.0);
    
    createConsigneButtons();

    // --- Zone de modification ---
    m_groupBox = new QGroupBox(this);
    m_groupBox->setTitle("");
    m_groupBox->setGeometry(573, 200, 419, 200);
    
    m_titleLabel = new QLabel(m_groupBox);
    m_titleLabel->setGeometry(10, 5, 399, 30);
    m_titleLabel->setFont(QFont("Times", 14, QFont::Bold));
    m_titleLabel->setStyleSheet("color: red; background: transparent;");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->hide();
    
    QGraphicsDropShadowEffect *shadowTitle = new QGraphicsDropShadowEffect(this);
    shadowTitle->setBlurRadius(0);
    shadowTitle->setOffset(1, 1);
    shadowTitle->setColor(Qt::white);
    m_titleLabel->setGraphicsEffect(shadowTitle);

    m_lastValueLabel = new QLabel(m_groupBox);
    m_lastValueLabel->setFont(QFont("Times", 12));
    m_lastValueLabel->setStyleSheet("color: white; background: transparent;");
    m_lastValueLabel->setAlignment(Qt::AlignCenter);
    m_lastValueLabel->setGeometry(10, 23, 399, 30);
    
    QGraphicsDropShadowEffect *shadowVal = new QGraphicsDropShadowEffect(this);
    shadowVal->setBlurRadius(0);
    shadowVal->setOffset(1, 1);
    shadowVal->setColor(Qt::black);
    m_lastValueLabel->setGraphicsEffect(shadowVal);

    m_selCons = new QDoubleSpinBox(m_groupBox);
    m_selCons->setFont(QFont("Times", 18));
    m_selCons->setDecimals(1);
    m_selCons->setGeometry(110, 50, 200, 60);
    m_selCons->setSingleStep(0.1);
    m_selCons->setSuffix(" °C");
    m_selCons->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
    m_selCons ->setStyleSheet("QDoubleSpinBox::down-button{width: 80} QDoubleSpinBox::up-button{width: 80}");
    m_selCons->hide();

    if (QLineEdit *editor = m_selCons->findChild<QLineEdit*>()) {
        editor->setReadOnly(true);
        editor->setFocusPolicy(Qt::NoFocus);
        editor->setContextMenuPolicy(Qt::NoContextMenu);
    }

    m_validBtn = new QPushButton("Valider", m_groupBox);
    m_validBtn->setGeometry(100, 130, 100, 50);
    m_validBtn->hide();

    m_quitBtn = new QPushButton("Quitter", m_groupBox);
    m_quitBtn->setGeometry(220, 130, 100, 50);
    m_quitBtn->setStyleSheet("background-color: red;");
    m_quitBtn->hide();

    connect(m_validBtn, &QPushButton::clicked, this, &ModifCons::saveCons);
    connect(m_quitBtn, &QPushButton::clicked, this, &ModifCons::closeConsigne);

    m_quitGlobalBtn = new QPushButton("Quitter", this);
    m_quitGlobalBtn->setGeometry(870, 544, 128, 50);
    m_quitGlobalBtn->setStyleSheet("font-size: 15px; background-color: red");
    
    connect(m_quitGlobalBtn, &QPushButton::clicked, this, [this]() {
        emit quitterDemande();
    });
}

// -------------------- Création des boutons --------------------
void ModifCons::createConsigneButtons()
{
    for (int i = 0; i < configs.size(); ++i) {
        double val = configs[i].getValue();

        QPushButton *btn = new QPushButton(m_listContainer);
        btn->setFixedHeight(43);
        btn->setCheckable(true);

        QString color = configs[i].titre.contains("Canicule") ? "orange"
                        : configs[i].titre.contains("Delta") ? "orange"
                        : configs[i].titre.contains("Froid") ? "blue"
                        : configs[i].titre.contains("Hiver") ? "mediumvioletred"
                        : "red";

        QHBoxLayout *layout = new QHBoxLayout(btn);
        layout->setContentsMargins(20, 3, 20, 3);

        QLabel *titre = new QLabel(configs[i].titre, btn);
        titre->setStyleSheet(QString("color: %1; font-size: 14px;").arg(color));

        QLabel *valeur = new QLabel(QString("%1 °C").arg(val, 0, 'f', 1), btn);
        valeur->setStyleSheet(QString("color: %1; font-size: 14px;").arg(color));

        layout->addWidget(titre);
        layout->addStretch();
        layout->addWidget(valeur);

        btn->setStyleSheet(R"(
            QPushButton { background:white; border-radius:10px; border:1px solid gray; }
            QPushButton:checked { background:#3399ff; color:white; }
        )");

        m_listLayout->addWidget(btn);
        m_consigneButtons.append(btn);
        m_buttonGroup->addButton(btn, i);

        connect(btn, &QPushButton::clicked, this, [=]() { selectCons(i); });
    }
}

void ModifCons::selectCons(int index)
{
    if (index < 0 || index >= configs.size()) return;

    clearHighlight();
    currentIndex = index;

    const auto &cfg = configs[index];
    consSelect = cfg.getValue();
    consMini = cfg.minVal;
    consMaxi = cfg.maxVal;

    QString titre = QString("%1 (défaut : %2 °C)")
        .arg(cfg.titre).arg(cfg.defaut, 0, 'f', 1);

    m_titleLabel->setText(titre);
    m_lastValueLabel->setText(QString("Valeur actuelle : %1 °C").arg(consSelect, 0, 'f', 1));

    adjustCons();
}

void ModifCons::adjustCons()
{
    m_titleLabel->show();
    m_selCons->setRange(consMini, consMaxi);
    m_selCons->setValue(consSelect);
    m_selCons->show();
    m_groupBox->show();
    m_validBtn->show();
    m_quitBtn->show();
}

void ModifCons::clearHighlight()
{
    if (auto btn = qobject_cast<QPushButton *>(m_buttonGroup->checkedButton())) {
        m_buttonGroup->setExclusive(false);
        btn->setChecked(false);
        m_buttonGroup->setExclusive(true);
    }
}

void ModifCons::saveCons()
{
    if (currentIndex < 0 || currentIndex >= configs.size()) return;
    double val = m_selCons->value();

    configs[currentIndex].save(val);
    m_modifConsignes->sauvegarder();

    closeConsigne();
}

void ModifCons::closeConsigne()
{
    clearHighlight();
    m_groupBox->hide();
    m_selCons->hide();
    m_validBtn->hide();
    m_quitBtn->hide();
    currentIndex = -1;
}

bool ModifCons::event(QEvent *event)
{
    if (event->type() == QEvent::TouchBegin ||
        event->type() == QEvent::TouchUpdate)
    {
        QTouchEvent *touch = static_cast<QTouchEvent *>(event);

        bool zone1Touched = false;
        bool zone2Touched = false;

        for (const QTouchEvent::TouchPoint &p : touch->touchPoints())
        {
            QPoint pos = p.pos().toPoint();

            if (zone1.contains(pos))
                zone1Touched = true;

            if (zone2.contains(pos))
                zone2Touched = true;
        }

        // Les deux zones touchées
        if (zone1Touched && zone2Touched)
        {
            if (!timerStarted) {
                touchTimer.start();
                timerStarted = true;
            }

            // maintien 1 seconde
            if (touchTimer.elapsed() >= 1000 && !serviceOpened)
            {
                serviceOpened = true;
                emit demandeServiceTest();
                return true;
            }
        }
        else
        {
            timerStarted = false;
        }
    }

    if (event->type() == QEvent::TouchEnd)
    {
        timerStarted = false;
        serviceOpened = false;
    }

    return QWidget::event(event);
}

ModifCons::~ModifCons() = default;


