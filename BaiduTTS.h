#pragma once
#include "ITTS.h"
#include "QString"
#include "QObject"
#include "QList"
#include "QNetworkReply"
#include "QSslError"
#include <fstream>

class QNetworkAccessManager;
class BaiduTTS : public QObject, public ITTS
{
	Q_OBJECT

public:
	BaiduTTS();
	~BaiduTTS();

	int doAuth();
	virtual int tts(const char* src_text, const char* des_path) override;

public slots:
	//bool slotReadyRead();

	bool getAuthToken(const QByteArray &sReply);

	//bool slotError(QNetworkReply::NetworkError code);
	//bool slotSslErrors(const QList<QSslError> &errors);
	//bool replyFinished(QNetworkReply*);
	bool loadAuthFromCache();
private:

	static const QString m_sOAuthUrl;
	static const QString m_sTTSGetUrl;
	static const QString m_sConfigFile;

	QNetworkAccessManager *m_manager;

	QString m_sAppid;
	QString m_sAPIKey;
	QString m_sSecretKey;
	QString m_sToken;

	enum EState {
		NEED_AUTH,
		AUTH_PASSED,
		TTS_REQ,
		TTS_RSP
	} m_nState;
	const char* m_srcTest;
	const char* m_desPath;
	std::fstream* m_pOfstream;
};

