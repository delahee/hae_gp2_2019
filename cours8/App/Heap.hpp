#pragma once
#include <functional>
template
<typename T>
class Heap {
public:
	std::vector<T> data;
	std::function<bool(const T & a, const T&b)> less;

	T extractMin() {
		T elem = data[0];
		pop();
		return elem;
	};

	void heapify( int pos ) {
		size_t largest = pos;
		size_t l = 2 * pos + 1;
		size_t r = 2 * pos + 2;

		if (l < data.size() && less(data[l] , data[largest])) {
			largest = l;
		}

		if (r < data.size() && less(data[r] , data[largest]))
			largest = r;

		if (largest != pos) {
			std::swap(data[pos], data[largest]);
			heapify(largest);
		}
	}

	bool sanityCheck() {
		for (int i = 0; i < data.size() / 2; i++) {
			if ((i*2+1) < data.size() && data[2 * i + 1] < data[i])
				throw "assert";
			if ((i * 2 + 2) < data.size() &&  data[2 * i + 2] < data[i])
				throw "assert";
		}
	}

	void push(const T & elem) {
		data.insert(data.begin(), elem);
		heapify(0);
	}

	void pop() {
		if (size() == 1) {
			data.clear();
			return;
		}
		std::swap(data[0], data[data.size() - 1]);
		data.erase(data.begin() + data.size() -1 );
		heapify(0);
	}

	int size() {
		return data.size();
	};
};
