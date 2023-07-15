#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnPlayPause_clicked();

    void on_btnStop_clicked();

    void on_sliderVolume_valueChanged(int value);

    void on_actionOpen_triggered();

    void changeMetadata();

    void populateSongList(const QString& directory);
    void playSelectedSong(QListWidgetItem* item);

    void loadSettings();
    void saveSettings();

    void updateSliderRange(qint64 duration);

    void seekToPosition(int position);



private:
    Ui::MainWindow *ui;
    QMediaPlayer* player;
    QString currentDirectory;
    QString songName;
};
#endif // MAINWINDOW_H
