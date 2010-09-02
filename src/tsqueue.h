#ifndef TSQUEUE_H
#define TSQUEUE_H

template<class T> class threadSafeQueue
{
	public:
		threadSafeQueue() {
			fst = new struct __queue;
			fst->next = NULL;
			lst = fst;
		}

		void enqueue(T &d) {
			fst->data = &d;
			struct __queue *tmp = new struct __queue;
			tmp->next = NULL;
			fst->next = tmp;
			fst = tmp;
		}

		T* dequeue() {
			if(lst->next == NULL)
				return NULL;
			
			T *d = lst->data;

			struct __queue *tmp = lst;
			lst = lst->next;
			delete tmp;
			return d;
		}
	private:
		struct __queue {
			T *data;
			struct __queue * next;
		};
		struct __queue *fst;
		struct __queue *lst;

};

#endif

