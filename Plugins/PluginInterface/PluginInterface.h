#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include <QtPlugin>

class PluginInterface : public QObject
{
	Q_OBJECT
public:
	PluginInterface();
	virtual ~PluginInterface() {};

private:

};

QT_BEGIN_NAMESPACE
#define RemoteServicePluginInterface_iid "RemoteServicePluginInterface"
Q_DECLARE_INTERFACE(PluginInterface, RemoteServicePluginInterface_iid)
QT_END_NAMESPACE

#endif // PLUGININTERFACE_H
