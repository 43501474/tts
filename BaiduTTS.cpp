#include "BaiduTTS.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <fstream>

using namespace std;

const QString BaiduTTS::m_sOAuthUrl("https://openapi.baidu.com/oauth/2.0/token?grant_type=client_credentials&client_id=%1&client_secret=%2&");

const QString BaiduTTS::m_sTTSGetUrl("http://tsn.baidu.com/text2audio?tex=%1&lan=zh&cuid=%2&ctp=1&tok=%3");

BaiduTTS::BaiduTTS()
	: m_sAppid("7739437")
	, m_sAPIKey("SDLeGToa0DIzrOVgqGgr47je")
	, m_sSecretKey("2642e0ebdfd8df5580d68c63626c1778")
	, m_bPassedAuth(false)
	, m_manager(new QNetworkAccessManager(nullptr))
	, m_nState(NEED_AUTH)
	, m_srcTest(nullptr)
	, m_desPath(nullptr)
	, m_pOfstream(nullptr)
{
	connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

BaiduTTS::~BaiduTTS()
{
	m_manager->deleteLater();
	delete m_pOfstream;
}

int BaiduTTS::doAuth()
{
	QString sOAuthReqUrl = m_sOAuthUrl.arg(m_sAPIKey).arg(m_sSecretKey);

	QNetworkRequest request;
	request.setUrl(QUrl(sOAuthReqUrl));

	QNetworkReply *reply = m_manager->get(request);
	connect(reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
		this, SLOT(slotError(QNetworkReply::NetworkError)));
	connect(reply, SIGNAL(sslErrors(QList<QSslError>)),
		this, SLOT(slotSslErrors(QList<QSslError>)));
	// reply->deleteLater();
	return 0;
}

int BaiduTTS::tts(const char* src_text, const char* des_path)
{
	m_srcTest = src_text;
	m_desPath = des_path;

	m_pOfstream = new fstream(m_desPath, ofstream::binary | ofstream::out);
	if (!*m_pOfstream)
		return -1;

	if (m_bPassedAuth == false && doAuth())
		return -1;

	return 0;
}

bool BaiduTTS::slotReadyRead()
{
	qDebug() << "BaiduTTS::slotReadyRead";
	
	QNetworkReply* pReplay = dynamic_cast<QNetworkReply*>(sender());
	if (pReplay)
	{
		QByteArray baReply = pReplay->readAll();
		if (m_nState == NEED_AUTH)
		{
			getAuthToken(baReply);

			if (m_sToken.isEmpty())
				return false;

			QString sText = QString::fromLocal8Bit("¶ËÎç½Ú³ÔôÕ×Ó");
			QString sOAuthReqUrl = m_sTTSGetUrl.arg(sText.toUtf8().toPercentEncoding().constData()).arg("009923").arg(m_sToken);

			QNetworkRequest request;
			request.setUrl(QUrl(sOAuthReqUrl));
			QNetworkReply *reply = m_manager->get(request);
			m_nState = TTS_REQ;
			connect(reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
			connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
				this, SLOT(slotError(QNetworkReply::NetworkError)));
			connect(reply, SIGNAL(sslErrors(QList<QSslError>)),
				this, SLOT(slotSslErrors(QList<QSslError>)));
		}
		else if (m_nState == TTS_REQ)
		{
			bool bContainValidHeader = false;
			QList<QNetworkReply::RawHeaderPair> lstRHP = pReplay->rawHeaderPairs();
			for (QNetworkReply::RawHeaderPair p : lstRHP)
			{
				if (p.first == "Content-type" && p.second.contains("mp3"))
				{
					m_pOfstream->seekp(0, fstream::end);
					m_pOfstream->write(baReply.data(), baReply.length());
					bContainValidHeader = true;
				}
			}

			if (!bContainValidHeader)
			{
				qDebug() << "Header indicate Err";
				qDebug() << pReplay->rawHeaderList();
				qDebug() << baReply;
				return false;
			}
		}
	}
	else
	{
		qDebug() << "sender isn't QNetworkReply";
		return false;
	}
		
	return true;
}

bool BaiduTTS::getAuthToken(QByteArray &baReply)
{
	qDebug() << baReply;

	QJsonParseError err;
	QJsonDocument oDoc = QJsonDocument::fromJson(baReply, &err);
	if (oDoc.isNull())
	{
		qDebug() << err.errorString();
		return false;
	}

	if (oDoc.isObject())
	{
		QJsonObject oObj = oDoc.object();
		if (oObj.contains("access_token"))
		{
			m_sToken = oObj["access_token"].toString();
			if (!m_sToken.isEmpty())
				m_nState = AUTH_PASSED;
		}
		else
		{
			qDebug() << "root obj not contain token";
			return false;
		}
	}
	else
	{
		qDebug() << "root is not obj";
		return false;
	}

	return true;
}

bool BaiduTTS::slotError(QNetworkReply::NetworkError code)
{
	qDebug() << "BaiduTTS::slotError" << code;
	return true;
}

bool BaiduTTS::slotSslErrors(const QList<QSslError> &errors)
{
	qDebug() << "BaiduTTS::slotSslErrors";
	return true;
}

bool BaiduTTS::replyFinished(QNetworkReply*)
{
	qDebug() << "BaiduTTS::replyFinished";
	return true;
}

