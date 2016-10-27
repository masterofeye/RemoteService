#ifndef MKSCLIENT_H
#define MKSCLIENT_H

#include <QtWidgets/QMainWindow>
#include "ui_mksclient.h"

class MKSClient : public QMainWindow
{
	Q_OBJECT

public:
	MKSClient(QWidget *parent = 0);
	~MKSClient();

private:
	Ui::MKSClientClass ui;
};

#endif // MKSCLIENT_H
