#include "BaseDbObject.h"

namespace RW{
	namespace MYSQL
	{
		BaseDbObject::BaseDbObject(QObject *Parent) : QObject(Parent),
			m_logger(spdlog::get("file_logger"))
		{
		}


		BaseDbObject::~BaseDbObject()
		{
		}
	}
}