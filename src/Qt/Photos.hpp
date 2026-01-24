#pragma once
#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QPixmap>
#include <QStringList>

class Photos : public QWidget
{
    Q_OBJECT

public:
    explicit Photos(const QString &folderPath, QWidget *parent = nullptr);
    ~Photos() = default;

    void setInterval(int ms); // modifier intervalle défilement

private:
    QLabel *m_photoLabel;
    QTimer *m_timer;
    QStringList m_photoFiles;
    QList<QPixmap> m_pixmaps;
    int m_photoIndex = 0;
    QString m_folderPath;

    void loadPhotos();
    void showNextPhoto();

};
