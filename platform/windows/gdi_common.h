
#ifndef GDI_COMMON_H
#define GDI_COMMON_H

#include <core/object/class_db.h>
#include <condition_variable>
#include <mutex>

class QtCallGengine : public Object {
	GDCLASS(QtCallGengine, Object);

protected:
	static void _bind_methods() {
		ADD_SIGNAL(MethodInfo("resize"));
	}

public:
	QtCallGengine(){};
	~QtCallGengine(){};
};

class QtSyncGengine {
private:
	std::mutex _mutex;
	std::condition_variable _condition;

	bool _has_change = false;

public:
	static QtSyncGengine *get_singleton() {
		static QtSyncGengine sync;
		return &sync;
	}

	void wait() {
		_has_change = true;
		std::unique_lock<std::mutex> lock(_mutex);
		while (_has_change) {
			_condition.wait(lock);
		}
	}

	void wake() {
		if (!_has_change) {
			return;
		}
		std::unique_lock<std::mutex> lock(_mutex);
		_has_change = false;
		_condition.notify_one();
	}

	QtSyncGengine(){};
	~QtSyncGengine(){};
};

#endif //GDI_COMMON_H
