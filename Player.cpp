#include "Player.h"
#include "FileSection.h"
#include "BaiduTTS.h"

#include <QMediaPlaylist>
#include <QMediaPlayer>
#include <QThread>
#include <QFile>
#include <assert.h>

class CPlayerData
{
public:
	QMediaPlayer* m_player;
	CFileSection* m_pFileSection;
	QString sCur;
	QString sNext;
};

CPlayer::CPlayer(const QString& sFileName)
	:m_d(new CPlayerData)
{
	m_d->m_player = nullptr;
	m_d->m_pFileSection = nullptr;
	m_d->sCur = "tts0.mp3";
	m_d->sNext = "tts1.mp3";

	m_d->m_player = new QMediaPlayer;
	m_d->m_player->setVolume(100);
	connect(m_d->m_player, static_cast<void(QMediaPlayer::*)(QMediaPlayer::Error)>(&QMediaPlayer::error),
		[=](QMediaPlayer::Error error) {
		qDebug() << "ERROR:" << error;
	});
	connect(m_d->m_player, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(onStateChange(QMediaPlayer::State)));

	m_d->m_pFileSection = new CFileSection(sFileName);
}

void CPlayer::play()
{
	QFile f(m_d->sCur);
	if (!f.exists() || f.size() == 0)
	{
		int ret = downloadSound(m_d->sCur);
		if (ret != 0)
			return;
	}

	if (f.size() == 0)
		return;

	m_d->m_player->setMedia(QUrl::fromLocalFile(m_d->sCur));
	int ret = downloadSound(m_d->sNext);
	if (ret != 0)
		disconnect(m_d->m_player, 0, this, 0);
	m_d->m_player->play();
	m_d->sNext.swap(m_d->sCur);
}

CPlayer::~CPlayer()
{
	delete m_d->m_player;
	delete m_d->m_pFileSection;
	delete m_d;
}

//void CPlayer::fillList(int)
//{
//	QMediaContent curMedia = m_playList->currentMedia();
//	qDebug() << "======  now playing: " << curMedia.canonicalUrl().fileName();
//	
//	if (curMedia.canonicalUrl().fileName().contains("tts0"))
//	{
//		if (m_playList->mediaCount() == 1)
//			downloadAndAddToList("tts1.mp3");
//		else
//			downloadAndAddToList("tts1.mp3");
//	}
//	else
//		downloadAndAddToList("tts0.mp3");
//}
//
//void CPlayer::initList()
//{
//	downloadAndAddToList("tts0.mp3");	
//}
//
//void CPlayer::downloadAndAddToList(const QString& sPath)
//{
//	int nCount = m_playList->mediaCount();
//	if (nCount > 1)
//	{
//		int nCurIdx = m_playList->currentIndex();
//		assert(nCurIdx < 2 && nCurIdx > -1);
//		QMediaContent tobeRemoved = m_playList->media(1 - nCurIdx);
//		m_playList->removeMedia(1 - nCurIdx);
//		qDebug() << "Removing" << 1 - nCurIdx << tobeRemoved.canonicalUrl();
//	}
//	downloadSound(sPath);
//	QUrl tts1Url = QUrl::fromLocalFile(sPath);
//	qDebug() << "Adding" << tts1Url;
//	m_playList->addMedia(tts1Url);
//}

int CPlayer::downloadSound(const QString& path)
{
	BaiduTTS oTTS(path.toStdString().c_str());
	QString s;

	s = m_d->m_pFileSection->getNextSentence();
	qDebug() << "--" << s;
	if (s.isEmpty())
		return 1;

	oTTS.tts(s.toLocal8Bit().data());

	return 0;
}

const QString& CPlayer::fileContent()
{
	return m_d->m_pFileSection->fileContent();
}

void CPlayer::onStateChange(QMediaPlayer::State s)
{
	if (s == QMediaPlayer::StoppedState)
		play();
}
