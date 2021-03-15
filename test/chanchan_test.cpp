#include "chanchan/chanchan.h"

#include <assert.h>

#include <iostream>
#include <memory>
#include <optional>
#include <type_traits>

namespace chan = chanchan;

using namespace std;

template <int I = 0, typename T, typename F>
constexpr void for_each(const T& t, F f) {
	if constexpr (I < tuple_size<T>::value) {
		f(get<I>(t));
		for_each<I + 1>(t, f);
	}
}

int main(void) {
	{
		static auto mutual = chan::mutual<int>{};
		static auto sender = chan::sender<int>{nullptr};
		static auto receiver = chan::receiver<int>{nullptr};

		auto entities = make_tuple(move(mutual), move(sender), move(receiver));

		for_each(entities, [](auto& e){
			// using T = typename remove_cvref<decltype(e)>::type;
			using T = remove_cvref_t<decltype(e)>;
			static_assert(is_final_v<T>);
			static_assert(!is_copy_constructible_v<T>);
			static_assert(is_nothrow_move_constructible_v<T>);
		});
	}

	{
		auto [tx, rx] = chan::make_channel<int>();

		tx.send(42);

		int x = rx.recv().value();
		std::cout << "x=" << x << "\n";
		assert(x == 42);
	}

	{
		auto [tx, rx] = chan::make_channel<int>();

		// no sender - if receiver doesn't check for the number of senders the
		// receiver will block forever
		tx.~sender();

		auto x = rx.recv();
		std::cout << "x=" << (x.has_value() ? "something" : "nothing") << "\n";
		assert (!x.has_value());
	}
}
