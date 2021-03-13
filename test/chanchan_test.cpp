#include "chanchan/chanchan.h"

#include <assert.h>

#include <iostream>
#include <optional>

namespace chan = chanchan;

using std::optional;

int main(void) {
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
