#pragma once
#include "qobject.h"


/*
@brief MKS needs some information to start. 
      - Username and password
	  - outputfolder
	  - MKS path

*/
class MKSHelper : public QObject
{
	Q_OBJECT

private: 
	QString m_Username;
	QString m_Password;
	QString m_PathToMks;
	QString m_ExeMks;
	QString m_OutputFolder;

public:
	/*
	@brief 
	@param [in] PathToMKS Comes from the DB.
	@param [in] Username Username of the MKS Account.
	@param [in] Password Password of the MKS Account.
	@param [in] Parent Parent object.
	@todo The Password needs to be verschlüsselt
	*/
	MKSHelper(QString PathToMKS, QString ExeMks, QString Username, QString Password, QString Outputfolder, QObject *Parent = nullptr);
	~MKSHelper();

	bool CheckAndDownload(QString MksLink);
	bool Download(QString MksLink);
	QString GetFolderByMksLink(QString MksLink);
	QString GetPrgFile(QString MksLink);
	QString GetUsbFile(QString MksLink);
signals:
	void SendProgress(quint8 u8Progress,QString Message = "");

};

