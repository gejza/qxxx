#include "mainwindow.h"
#include "osdfilter.h"
#include "playlistmodel.h"
#include "ui_mainwindow.h"
#include "videolibrary.h"
#include "statisticsview.h"
#include "db.h"

#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QLabel>
#include <QMessageBox>
#include <QSaveFile>
#include <QTimer>
#include <QSortFilterProxyModel>
#include <qfiledialog.h>
#include <qfiledialog.h>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	m_playlist = new PlaylistModel(VideoLibrary::instance(), this);
	ui->playTable->setModel(m_playlist);

	setWindowTitle(QString::fromLatin1("QtAV simple player example"));
	m_player = new QtAV::AVPlayer(this);
	connect(m_player, &AVPlayer::loaded, this, &MainWindow::onLoaded);
	m_osd = new OSDFilter(this);

	m_vo = new QtAV::VideoOutput(ui->verticalLayout);
	if (!m_vo->widget()) {
		QMessageBox::warning(0, QString::fromLatin1("QtAV error"), tr("Can not create video renderer"));
		return;
	}
	m_player->setRenderer(m_vo);
	m_osd->installTo(m_vo);
	connect(ui->actionShowNextOSD, &QAction::triggered, [this](){
		m_osd->useNextShowType();
	});

	ui->verticalLayout->insertWidget(0, m_vo->widget());

	connect(m_player, &QtAV::AVPlayer::positionChanged, ui->timeline, &Timeline::setPos);
	//connect(m_player, &QtAV::AVPlayer::started, this, QOverload<>::of(&MainWindow::updateSlider));
	//connect(m_player,&QtAV::AVPlayer::notifyIntervalChanged, this, &MainWindow::updateSliderUnit);

	connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openMedia);
	connect(ui->actionPlay, &QAction::triggered, this, &MainWindow::playPause);
	connect(ui->actionStop, &QAction::triggered, m_player, &QtAV::AVPlayer::stop);
	connect(ui->actionSetStart, &QAction::triggered, ui->timeline, &Timeline::markRangeBegin);
	connect(ui->actionSetStop, &QAction::triggered,  ui->timeline, &Timeline::markRangeEnd);

	m_position = new QLabel("...", this);
	ui->statusBar->addPermanentWidget(m_position);
	connect(m_player, &QtAV::AVPlayer::positionChanged, this, [this](qint64 pos){
		ui->timeline->setPos(pos);
		m_position->setText(ui->timeline->toString());
	});
	connect(ui->actionBack, &QAction::triggered, this, &MainWindow::onBack);
	connect(ui->actionForward, &QAction::triggered, this, &MainWindow::onForward);
	connect(ui->actionFaster, &QAction::triggered, ui->timeline, &Timeline::setFaster);
	connect(ui->actionSlower, &QAction::triggered, ui->timeline, &Timeline::setSlowly);
	connect(ui->actionNext, &QAction::triggered, this, &MainWindow::onRandomPlay);

	connect(ui->timeline, &Timeline::positionReqest, m_player, QOverload<qint64>::of(&QtAV::AVPlayer::seek));
	connect(m_player, &QtAV::AVPlayer::durationChanged, this, [this](qint64 pos){
		ui->timeline->setEnd(pos);
	});

	connect(ui->actionSearchFiles, &QAction::triggered, this, &MainWindow::onFindFiles);
	connect(ui->playTable, &QTableView::activated, this, [this](const QModelIndex& idx){
		play(m_playlist->at(idx));
	});
	QTimer::singleShot(0, this, &MainWindow::loadPlaylist);
	connect(this, &QObject::destroyed, this, &MainWindow::savePlaylist);

	connect(ui->actionRandomPlay, &QAction::triggered, this, &MainWindow::onRandomPlay);
	connect(ui->actionShowInfo, &QAction::triggered, this, &MainWindow::showInfo);
	connect(ui->actionSave, &QAction::triggered, VideoLibrary::instance(), &VideoLibrary::savePlaylist);

	qsrand(QTime::currentTime().msecsSinceStartOfDay());

	ui->menuPlaylist->addAction(ui->dockWidget->toggleViewAction());
	connect(ui->actionComputeHashes, &QAction::triggered, VideoLibrary::instance(), &VideoLibrary::computeHashes);
}

MainWindow::~MainWindow()
{
	VideoLibrary::instance()->saveData();
	//XDb::instance()->close();
	delete ui;
}

void MainWindow::play(VideoFile *vf)
{
	// unload prev
	m_current = vf;
	m_player->play(vf->filePath());
	ui->statusBar->showMessage(vf->filePath());
	ui->playTable->setCurrentIndex(m_playlist->find(vf));
}

void MainWindow::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void MainWindow::openMedia()
{
	QStringList fs = QFileDialog::getOpenFileNames(this, tr("Open a video"));
	if (fs.isEmpty())
		return;
	for (QString fn : fs) {
		VideoLibrary::instance()->getVideo(fn);
	}
}

void MainWindow::playPause()
{
	if (!m_player->isPlaying()) {
		m_player->play();
		return;
	}
	m_player->pause(!m_player->isPaused());
}

void MainWindow::onRandomPlay()
{
	if (m_playlist->rowCount() == 0) {
		return;
	}
	int next = qrand()%m_playlist->rowCount();
	play(m_playlist->at(next));
}

void MainWindow::onBack()
{
	m_player->setPosition(ui->timeline->seekBackward());
}

void MainWindow::onForward()
{
	m_player->setPosition(ui->timeline->seekForward());
}

void MainWindow::onFindFiles()
{
	QString file = QFileDialog::getExistingDirectory(this, tr("Open dir"), "/ext3/xxx");
	VideoLibrary::instance()->find(file);
}

void MainWindow::savePlaylist()
{
	VideoLibrary::instance()->savePlaylist();
}

void MainWindow::loadPlaylist()
{
	VideoLibrary::instance()->loadPlaylist();
}

void MainWindow::onLoaded()
{
	if (m_current) {
		m_current->setFromStats(m_player->statistics());
	}
}

void MainWindow::showInfo()
{
	if (!m_stats)
		m_stats = new StatisticsView(this);
	if (m_player)
		m_stats->setStatistics(m_player->statistics());
	m_stats->show();
}



