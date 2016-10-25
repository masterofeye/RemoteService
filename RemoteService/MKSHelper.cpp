#include "MKSHelper.h"

#include <QDir>
#include "UtilProcess.h"

MKSHelper::MKSHelper(QString PathToMKS, 
				     QString ExeMks,
					 QString Username, 
					 QString Password, 
					 QString Outputfolder,
					 QObject *Parent) : QObject(Parent),
	m_PathToMks(PathToMKS),
	m_ExeMks(ExeMks),
	m_Username(Username),
	m_Password(Password),
	m_OutputFolder(Outputfolder)
{
}


MKSHelper::~MKSHelper()
{
}


bool MKSHelper::CheckAndDownload(QString MksLink)
{
	if (!QDir(GetFolderByMksLink(MksLink)).exists())
	{
		if (!Download(MksLink))
			return false;
	}
	else
	{
		//Download im Cache verfügbar
		return true;
	}
}

bool MKSHelper::Download(QString MksLink){

	if (m_Username.isEmpty() || m_Password.isEmpty())
		return false;

	QString targetDir = GetFolderByMksLink(MksLink);
	QStringList ArgumentList;

	emit SendProgress(10, "Load File");
	ArgumentList.append("servers");
	if (UtilProcess::StartWithReturn(m_PathToMks + "\\" + m_ExeMks, &ArgumentList).length() < 5)
	{
		ArgumentList.clear();
		ArgumentList.append("connect");
		ArgumentList.append("--hostname=\"ims-id\"");
		ArgumentList.append("--user=\"" + m_Username + "\"");
		ArgumentList.append("--password=\"" + m_Password + "\"");

		UtilProcess::Start(m_PathToMks + "\\" + m_ExeMks, &ArgumentList,false,10000);
	}
	emit SendProgress(15, "Download");
	
	QStringList ArgumentList2;
	ArgumentList2.append("dropsandbox");
	ArgumentList2.append("--yes");
	ArgumentList2.append("--delete=\"none\"");
	ArgumentList2.append("--status=\"none\"");
	ArgumentList2.append("\"" + targetDir + "/project.pj\"");

	UtilProcess::Start(m_PathToMks + "\\" + m_ExeMks, &ArgumentList, false, 10000);

	ArgumentList.clear();
	ArgumentList.append("createsandbox");
	ArgumentList.append("--yes");
	ArgumentList.append("--status=\"default\"");
	ArgumentList.append("--hostname=\"ims-id\" ");
	ArgumentList.append("--port=\"7001\"");
	ArgumentList.append("---project=\"" + MksLink + "\"");
	ArgumentList.append("\"" + targetDir + "/project.pj\"");

	UtilProcess::Start(m_PathToMks + "\\" + m_ExeMks, &ArgumentList, false, 1800000);


	UtilProcess::Start(m_PathToMks + "\\" + m_ExeMks, &ArgumentList2, false, 10000);

	emit SendProgress(50, "Download complete");

	return true;
}

/*
@brief Returns the local MKS directory.
@param MksLink Link to the file in MKS.
*/
QString MKSHelper::GetFolderByMksLink(QString MksLink)
{
	QString ret = MksLink.left(MksLink.lastIndexOf("dag_PF3.sw.sys.") + 15).replace("#b=", "/").replace("#adapt#gen#", "#").replace("#", "_");
	return m_OutputFolder + "/" + ret;
}

QString MKSHelper::GetPrgFile(QString MksLink){
	return false;
}
QString MKSHelper::GetUsbFile(QString MksLink){
	if (!CheckAndDownload(MksLink))
		return nullptr;

	if(!QDir(m_OutputFolder).exists())
	{
		QDir Directory(GetFolderByMksLink(MksLink));
		QFileInfoList fileInfoList = Directory.entryInfoList();

		if (fileInfoList.length() == 0)
			return nullptr;
		else
		{

		}

		QDir DirectoryBat(GetFolderByMksLink(MksLink));
		DirectoryBat.setNameFilters(QStringList() << "*.bat");

	}
}