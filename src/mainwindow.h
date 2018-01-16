#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtAV>

class QSlider;
class QPushButton;

namespace Ui {
class MainWindow;
}

class QLabel;
class QSortFilterProxyModel;

class OSDFilter;

class PlaylistModel;
class StatisticsView;
class VideoFile;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	void play(VideoFile* vf);
public Q_SLOTS:
	void showInfo();
	void openMedia();
	void playPause();
	void onRandomPlay();
	void onBack();
	void onForward();
	void onFindFiles();
	void savePlaylist();
	void loadPlaylist();
protected:
	void changeEvent(QEvent *e);

private:
	Ui::MainWindow *ui;
	QtAV::VideoOutput *m_vo;
	QtAV::AVPlayer *m_player;
	QLabel* m_position;
	PlaylistModel* m_playlist = nullptr;
	QSortFilterProxyModel* m_proxy = nullptr;
	OSDFilter *m_osd = nullptr;
	StatisticsView* m_stats = nullptr;
	VideoFile* m_current = nullptr;
};

#endif // MAINWINDOW_H
