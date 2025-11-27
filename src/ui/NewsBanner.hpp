#pragma once
#include <QWidget>
#include <QLabel>
#include <QScrollArea>
#include <QThread>
#include <QTimer>
#include <QStringList>

class NewsFetcher;

class NewsBanner : public QWidget
{
    Q_OBJECT

public:
    explicit NewsBanner(QWidget *parent = nullptr);
    ~NewsBanner();

private:
    // === UI ===
    QLabel *m_cLabel;
    QLabel *m_newsLabelTitle;
    QLabel *m_newsLabel;
    QScrollArea *m_newsScrollArea;
    QWidget *m_ccpContainer;

    // === Timers ===
    QTimer *m_newsTimer;
    QTimer *m_scrollTimer;

    // === Thread ===
    QThread *m_newsThread;
    NewsFetcher *m_newsFetcher;

    // === Données ===
    QStringList m_newsCache;
    int m_currentIndex;
    double m_scrollOffset;
    int m_textWidth;
    double m_scrollSpeed;

    // === Méthodes internes ===
    void setupUI();
    void startNewsThread();
    void updateNewsLabel(const QString &news);

private slots:
    void onNewsReceived(QStringList list);
};

