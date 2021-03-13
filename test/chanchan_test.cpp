#include "chanchan/chanchan.h"

#include <assert.h>

#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <tuple>
#include <utility>

namespace chan = chanchan;

int main(void) {
	{
		auto [tx, rx] = chan::make_channel<int>();

		tx.send(42);

		int x = rx.recv();
		std::cout << "got: " << x << "\n";
		assert(x == 42);
	}

	{
		auto [tx, rx] = chan::make_channel<int>();

		// no sender - if receiver doesn't check for the number of senders the
		// receiver will block forever
		tx.~sender();

		int x = rx.recv();
		std::cout << "got: " << x << "\n";
		assert(x == -1);
	}
}
