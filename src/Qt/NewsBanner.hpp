#pragma once
#include <QWidget>
#include <QLabel>
#include <QScrollArea>
#include <QTimer>
#include <QStringList>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class NewsBanner : public QWidget
{
    Q_OBJECT

public:
    explicit NewsBanner(QWidget *parent = nullptr);
    ~NewsBanner() override;

private:
    // === UI ===
    QLabel *m_cLabel;
    QLabel *m_newsLabelTitle;
    QLabel *m_newsLabel;
    QScrollArea *m_newsScrollArea;
    QWidget *m_ccpContainer;

    // === Timers ===
    QTimer *m_newsTimer;    // passage à la news suivante
    QTimer *m_scrollTimer;  // scroll texte long
    QTimer *m_fetchTimer;   // fetch RSS

    // === Network ===
    QNetworkAccessManager *m_manager;

    // === Données ===
    QStringList m_newsCache;
    int m_currentIndex;
    double m_scrollOffset;
    int m_textWidth;
    double m_scrollSpeed;

    // === Méthodes internes ===
    void setupUI();
    void fetchNews();
    void updateNewsLabel(const QString &news);

private slots:
    void onNewsReply(QNetworkReply *reply);
    void handleNextNews();
    void handleScrollStep();
};

