#pragma once
#include <QMainWindow>
#include <QMediaPlayer>

class QString;
class CPlayerData;
class CPlayer: public QMainWindow
{
	Q_OBJECT

public:
	CPlayer(const QString& sFileName);
	~CPlayer();

	void play();
	int downloadSound(const QString& path);
	const QString& fileContent();
	//	void downloadAndAddToList(const QString& sPath);
//	void initList();
public slots:
	void onStateChange(QMediaPlayer::State);

private:
	CPlayerData* m_d;
};
