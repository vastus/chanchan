#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <tuple>
#include <utility>

namespace chanchan {

using std::condition_variable;
using std::make_shared;
using std::make_tuple;
using std::mutex;
using std::optional;
using std::queue;
using std::shared_ptr;
using std::tuple;
using std::unique_lock;

struct noncopyable {
	noncopyable() = default;
	noncopyable(const noncopyable&) = delete;
	noncopyable& operator=(const noncopyable&) = delete;
};

template <typename T>
struct mutual final : private noncopyable {
	mutex m;
	queue<T> q;
	condition_variable cv;

	// TODO: why isn't this covered by noncopyable
	mutual() = default;

	mutual(mutual&& o) noexcept
		: q{std::move(o.q)}
	{}
};

template <typename T>
struct sender final : private noncopyable {
	const shared_ptr<mutual<T>> _mutual;

	sender(shared_ptr<mutual<T>> mutual)
		: _mutual{mutual}
	{}

	// move ctor
	sender(sender&& s) noexcept
		: _mutual{std::move(s._mutual)}
	{}

	constexpr void send(T x) {
		auto& [m, q, cv] = *_mutual;
		auto lock = unique_lock(m);

		q.push(x);
		lock.unlock();

		cv.notify_one();
	}
};

template <typename T>
struct receiver final : private noncopyable {
	shared_ptr<mutual<T>> _mutual;

	receiver(shared_ptr<mutual<T>> mutual)
		: _mutual{mutual}
	{}

	// move ctor
	receiver(receiver&& s) noexcept
		: _mutual{std::move(s._mutual)}
	{}

	constexpr optional<T> recv() {
		if (_mutual.use_count() == 1)
			return {};

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
	return make_tuple(std::move(tx), std::move(rx));
}

}  // namespace chanchan
