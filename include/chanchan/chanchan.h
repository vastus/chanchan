#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <tuple>

namespace chanchan {

using std::condition_variable;
using std::make_shared;
using std::make_tuple;
using std::mutex;
using std::queue;
using std::shared_ptr;
using std::tuple;
using std::unique_lock;

template <typename T>
struct mutual final {
	mutex m;
	queue<T> q;
	condition_variable cv;
};

template <typename T>
struct sender final {
	shared_ptr<mutual<T>> _mutual;

	constexpr void send(T x) {
		auto& [m, q, cv] = *_mutual;
		auto lock = unique_lock(m);

		q.push(x);
		lock.unlock();

		cv.notify_one();
	}
};

template <typename T>
struct receiver final {
	shared_ptr<mutual<T>> _mutual;

	// TODO: change to optional
	constexpr T recv() {
		if (_mutual.use_count() == 1)
			return -1;

		auto& [m, q, cv] = *_mutual;
		auto lock = unique_lock(m);

		while (q.empty())
			cv.wait(lock);

		T x = q.front(); q.pop();
		return x;
	}
};

template <typename T>
constexpr auto make_channel() {
	auto s = make_shared<mutual<T>>();
	auto tx = sender<T>{s};
	auto rx = receiver<T>{s};
	return make_tuple(tx, rx);
}

}  // namespace chanchan
