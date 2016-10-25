#pragma once

#include <QtCore>
#include "AbstractCommand.h"
namespace RW{
	namespace CORE
	{
		/*
		http://www.drdobbs.com/parallel/writing-lock-free-code-a-corrected-queue/210604448?pgno=2
		http://www.1024cores.net/home/lock-free-algorithms/queues
		*/
		class JobQueue : public QObject
		{
			Q_OBJECT
		private:
			AbstractCommand *m_Head;
			QAtomicPointer <AbstractCommand> m_Divider, m_Tail;
			int m_Counter;


		public:
			JobQueue(QObject *Parent = nullptr);
			~JobQueue();


			void push(AbstractCommand * const value)
			{

				m_Tail.load()->Next = value;
				m_Tail = m_Tail.load()->Next;// This moves the QueueNode into the atomic pointer, making it safe :)

				while (m_Head != m_Divider.load())
				{
					AbstractCommand *tmp = m_Head;
					m_Head = m_Head->Next;
					delete tmp;
				}
				m_Counter++;
			}

			bool peek(AbstractCommand ** Result)
			{
				if (m_Divider.load() != m_Tail.load())
				{
					// Problem area
					AbstractCommand *next = m_Divider.load()->Next;
					if (next)
					{
						*Result = next;
						return true;
					}
				}
				return false;
			}

			bool pop(AbstractCommand **Result)
			{
				bool res = this->peek(Result);
				if (res)
				{
					m_Divider = m_Divider.load()->Next;
					m_Counter--;
				}
				return res;
			}

			bool IsEmpty()
			{
				return m_Counter == 0;
			}

		};
	}
}
