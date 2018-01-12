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

class OSDFilter;

class PlaylistModel;
class StatisticsView;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

public Q_SLOTS:
	void play(int idx);
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
	OSDFilter *m_osd = nullptr;
	StatisticsView* m_stats = nullptr;
};

#endif // MAINWINDOW_H
