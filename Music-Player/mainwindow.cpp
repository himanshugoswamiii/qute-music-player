#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <iostream>
#include <QSettings>
#include <QMediaMetaData>
#include <QImage>
#include <QPixmap>
#include <QTimer>
#include <QStringList>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* Initialize */
    player = new QMediaPlayer(this);

    /* Code starts Here */
    setWindowTitle("Qute Music Player");

    loadSettings(); // load previously saved settings

    // Play the song when an item in list is clicked
    connect(ui->songListWidget, &QListWidget::itemClicked, this, &MainWindow::playSelectedSong);

    /* seek to Position */
    ui->positionSlider->setRange(0, 0);
    ui->positionSlider->setValue(0);     // Set initial value to 0

    //connect(player, &QMediaPlayer::durationChanged, this, &MainWindow::updateSliderRange);
    connect(ui->positionSlider, &QSlider::sliderMoved, this, &MainWindow::seekToPosition);



}

MainWindow::~MainWindow()
{
    saveSettings(); // Save the app settings when closing
    delete player;
    delete ui;
}


void MainWindow::updateSliderRange(qint64 duration)
{
    ui->positionSlider->setRange(0, static_cast<int>(duration));
}
void MainWindow::seekToPosition(int position)
{
    player->setPosition(static_cast<qint64>(position));
}



void MainWindow::on_btnPlayPause_clicked()
{
    if (player->state() == QMediaPlayer::PlayingState) {
        player->pause();
    } else {
        player->play();
    }
}


void MainWindow::on_btnStop_clicked()
{
    player->stop();
}


void MainWindow::on_sliderVolume_valueChanged(int value)
{
    player->setVolume(value);
}

void MainWindow::populateSongList(const QString& directory)
{
    QDir musicDir(directory);
    QStringList filters;
    filters << "*.mp3" << "*.wav";  // Add more file formats if needed
    QStringList fileList = musicDir.entryList(filters, QDir::Files);

    qDebug() << "Song list populated. Total items: " << fileList.count();


    ui->songListWidget->clear();  // Clear the previous song list
    ui->songListWidget->addItems(fileList);
}

void MainWindow::on_actionOpen_triggered()
{
    currentDirectory = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QDir::homePath());
    if (!currentDirectory.isEmpty()) {
        // Perform operations with the selected directory
        populateSongList(currentDirectory);
    }

}

void MainWindow::changeMetadata()
{
    // Get the list of keys there is metadata available for
    QStringList metadatalist = player->availableMetaData();

    // Get the size of the list
    int list_size = metadatalist.size();

    //qDebug() << player->isMetaDataAvailable() << list_size;

    // Define variables to store metadata key and value
    QString metadata_key;
    QVariant var_data;

    for (int indx = 0; indx < list_size; indx++)
    {
        // Get the key from the list
        metadata_key = metadatalist.at(indx);

        // Get the value for the key
        var_data = player->metaData(metadata_key);

        qDebug() << metadata_key << var_data.toString();
    }

    //
    QString title = player->metaData(QMediaMetaData::Title).toString();
    if (title != "") {
        ui->labelSong->setText(title);
    } else {
        ui->labelSong->setText(songName);
    }

    QString artistName = player->metaData(QMediaMetaData::ContributingArtist).toString();
    qDebug() << artistName;
    ui->labelArtist->setText(artistName);

    QString albumTitle = player->metaData(QMediaMetaData::AlbumTitle).toString();
    ui->labelAlbumTitle->setText(albumTitle);

    QString albumYear = player->metaData(QMediaMetaData::Year).toString();
    if (albumYear != ""){
        ui->labelYear->setText(albumYear);
    } else {
        ui->labelYear->setText("NA");
    }


    QVariant albumArtVariant = player->metaData(QMediaMetaData::CoverArtImage);
    if (albumArtVariant.isValid()) {
        QImage albumArtImage = albumArtVariant.value<QImage>();

        // Set maximum size for the label
        QSize labelSize = ui->labelAlbumArt->size();

        // Scale down the album art image while maintaining aspect ratio
        QPixmap albumArtPixmap = QPixmap::fromImage(albumArtImage);
        albumArtPixmap = albumArtPixmap.scaled(labelSize*0.9, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // Set the scaled album art image as the pixmap for the label
        ui->labelAlbumArt->setPixmap(albumArtPixmap);

    }

    // set duration
    updateSliderRange(player->duration()); // Update the slider range based on the media's duration
    ui->positionSlider->setValue(0);

}


void MainWindow::playSelectedSong(QListWidgetItem* item)
{
    songName = item->text();
    QString fullPath = currentDirectory + "/" + songName;

    qDebug() << "Full path of the selected song: " << fullPath;

    // Retrieve metadata before setting media source
    player->setMedia(QUrl::fromLocalFile(fullPath));

    // Start playback after setting metadata
    player->play();

    // Call the changeMetadata function after a short delay (100ms) because MetaData takes some time to load
    QTimer::singleShot(100, this, &MainWindow::changeMetadata);

}

/* ---- SETTINGS ---- */

void MainWindow::loadSettings()
{
    // Retrieve the last selected directory from the file
    QSettings settings("qute-apps", "music-player");  // Customize with your own organization and application names
    currentDirectory = settings.value("lastDirectory").toString();
    if (!currentDirectory.isEmpty()) {
        populateSongList(currentDirectory);
    }
    // Retrieve the last volume setting from the file
    int volume = settings.value("volume", 50).toInt();  // Default volume is set to 50 if no value is found
    player->setVolume(volume);
    // Set the volume slider position
    ui->sliderVolume->setValue(volume);
}

void MainWindow::saveSettings()
{
    // Save the selected directory to a file
    QSettings settings("qute-apps", "music-player");  // Customize with your own organization and application names
    settings.setValue("lastDirectory", currentDirectory);
    settings.setValue("volume", player->volume());
}




