import "std";

impl array<T> :: {
	data: ptr;
	size: u64;
	index: u64;

	init(self: array<T>, u64 size): array<T> -> {
		self.size = size;
		self.index = 0;
		self.data = memalloc(sizeof(T) * self.size);

		return self;
	}

	push(self: array<T>, T element): array<T> -> {
		memsetvalue(self.data, self.index, element);
	}

	pop(self: array): [err, T] -> {
		if (self.index > 0)
		{
			self.index = self.index - 1;
			removed_item: T = memgetvalue(self.data, self.index);

			return removed_item;
		}

		return err::init("array is already empty.");
	}

	get(self: array, u64 index): [err, T] {
		if (index < self.size)
			return memgetvalue(self.data, index);

		return err::init("index is larger than the size of array.");
	}
}

main(argc: i32, argv: array<string>): i32 -> {
	print("Hello, World!\n");

	first_element: [err, string] = argv::get(0);
	if (type(first_element, err)) return -1;

	printf("first_element = %s\n", first_element);

	x: ptr = alloc(10 * sizeof(i32));
	$(x, 0) = 10;

	return 0;
}
