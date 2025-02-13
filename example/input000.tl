import "std";

impl array<T> :: {
	data: ptr;
	size: u32;

	init(self: array<T>, size: u32): array<T> -> {
	}
}

table of impl {
	array: {
		args: [T],
		struct: {
			data: ptr;
			size: u32;
		},
		functions: {
			init: {
				args: { (self, array<T>), (size, u32) },
				return: { array<T> },
				body: {
				}
			}
		}
	}
}

main(argc: i32, argv: array<string>): i32 -> {
	// it will init the array with i32 type,
	// basically it will create a struct with
	//	data, size as properties
	//	and init as method.

	// because x will be present
	// in the symbol table,
	// we can call functions of x.
	x: array<i32> = x::init(10);

	// then, it will return x.
	// but because these aren't pointers
	// x would not change.
	y: array<i32> = x::init(20);

	// array<i32>
	// it will be present in table,
	// where new types will be pushed 
	// into array,
	// to create functions and
	// their kind of properties
	// at compile time.

	return 0;
}

table {
	"i32",
	"string"
}

at compile time, those things will be created
and called.
