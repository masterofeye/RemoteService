#include "JobQueue.h"

namespace RW{
	namespace CORE
	{
		JobQueue::JobQueue(QObject *Parent) : QObject(Parent),
			m_Head(nullptr),
			m_Tail(nullptr),
			m_Counter(0)
		{
			m_Head = new AbstractCommand(nullptr);
			m_Tail.store(m_Head);
			m_Divider.store(m_Head);
		}


		JobQueue::~JobQueue()
		{
		}
	}
}