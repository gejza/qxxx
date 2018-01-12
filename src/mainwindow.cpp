#include "mainwindow.h"
#include "osdfilter.h"
#include "playlistmodel.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QLabel>
#include <QMessageBox>
#include <QSaveFile>
#include <QTimer>



MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	m_playlist = new PlaylistModel(this);
	ui->playTable->setModel(m_playlist);

	setWindowTitle(QString::fromLatin1("QtAV simple player example"));
	m_player = new QtAV::AVPlayer(this);
	m_osd = new OSDFilter(this);

	m_vo = new QtAV::VideoOutput(this);
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

	connect(ui->timeline, &Timeline::positionReqest, m_player, QOverload<qint64>::of(&QtAV::AVPlayer::seek));
	connect(m_player, &QtAV::AVPlayer::durationChanged, this, [this](qint64 pos){
		ui->timeline->setEnd(pos);
	});

	connect(ui->actionSearchFiles, &QAction::triggered, this, &MainWindow::onFindFiles);
	connect(ui->playTable, &QTableView::activated, this, [this](const QModelIndex& idx){
		m_player->play(m_playlist->at(idx).filePath());
	});
	QTimer::singleShot(0, this, &MainWindow::loadPlaylist);
	connect(this, &QObject::destroyed, this, &MainWindow::savePlaylist);

	connect(ui->actionRandomPlay, &QAction::triggered, this, &MainWindow::onRandomPlay);
}

MainWindow::~MainWindow()
{
	delete ui;
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
	QString file = "/ext3/xxx/x40/movies-2017-09-05-1-1d2334877a4c4b299025ea6bfd21321c-video.mp4"; // QFileDialog::getOpenFileName(0, tr("Open a video"));
	if (file.isEmpty())
		return;
	m_player->play(file);
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
	int pi = qrand()%m_playlist->rowCount();
	m_player->play(m_playlist->at(pi).filePath());
	ui->playTable->setCurrentIndex(m_playlist->index(pi, 0));
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
	m_playlist->find(file);
}

void MainWindow::savePlaylist()
{
	QSaveFile f("playlist.json");
	QJsonDocument doc(m_playlist->toJson());
	if (f.open(QIODevice::WriteOnly)) {
		f.write(doc.toJson());
		f.commit();
	}
}

void MainWindow::loadPlaylist()
{
	QFile f("playlist.json");
	if (f.open(QIODevice::ReadOnly)) {
		auto doc = QJsonDocument::fromJson(f.readAll());
		m_playlist->fromJson(doc.array());
	}
}



