#ifndef FLASHPLU_H
#define FLASHPLU_H

#include "flashplu_global.h"
#include <QtPlugin>

/*Interface definition for all Plugins*/
#include "..\PluginInterface\PluginInterface.h"

class FLASHPLU_EXPORT FlashPlu : public QObject
{
	Q_OBJECT
    Q_PLUGIN_METADATA(IID "RemoteServicePluginInterface" FILE "FlashPlu.json")
public:
	FlashPlu();
	virtual ~FlashPlu() {};

private:

};


#endif // FLASHPLU_H
