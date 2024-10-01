#include <iostream>
#include <optional>
#include <cstring>
#include <chrono>
#include <map>

static constexpr uint32_t MEM_SIZE = 1 << 24;
static constexpr uint32_t MAX_ASSOC = 32;
static constexpr uint32_t MAX_STRIDE = MEM_SIZE / MAX_ASSOC;

alignas(4 * 1024) char mem[MEM_SIZE];

typedef std::chrono::duration<double, std::nano> duration;
typedef std::chrono::time_point<std::chrono::high_resolution_clock> time_point;

void set_next_ptr(char* loc, char * ptr) {
	char** addr = reinterpret_cast<char**>(loc);
	*addr = ptr;
}

void init_mem(uint32_t stride, uint32_t spots) {
	set_next_ptr(mem, &mem[stride * (spots - 1)]);
	for (int i = 0; i < spots - 1; ++i) {
		set_next_ptr(mem + stride * (i + 1), &mem[stride * i]);
	}
}

duration time(uint32_t stride, uint32_t spots) {
	char * ptr = &mem[stride * (spots - 1)];
#define STEP_1 ptr = *reinterpret_cast<char **>(ptr);
#define STEP_10 STEP_1 STEP_1 STEP_1 STEP_1 STEP_1 STEP_1 STEP_1 STEP_1 STEP_1 STEP_1
#define STEP_100 STEP_10 STEP_10 STEP_10 STEP_10 STEP_10 STEP_10 STEP_10 STEP_10 STEP_10 STEP_10
#define STEP_1000 STEP_100 STEP_100 STEP_100 STEP_100 STEP_100 STEP_100 STEP_100 STEP_100 STEP_100 STEP_100
#define STEP_10000 STEP_1000 STEP_1000 STEP_1000 STEP_1000 STEP_1000 STEP_1000 STEP_1000 STEP_1000 STEP_1000 STEP_1000
#define STEP_100000 STEP_10000 STEP_10000 STEP_10000 STEP_10000 STEP_10000 STEP_10000 STEP_10000 STEP_10000 STEP_10000 STEP_10000
	// warm up
	STEP_1000
  	auto start = std::chrono::high_resolution_clock::now();
	STEP_100000
  	auto end = std::chrono::high_resolution_clock::now();
	return end - start;
}

std::optional<uint32_t> try_estimate_assoc(uint32_t stride) {
	init_mem(stride, 1);
	auto prev_time = time(stride, 1);
	for (uint32_t assoc = 2; assoc < MAX_ASSOC; ++assoc) {
		init_mem(stride, assoc);
		auto cur_time = time(stride, assoc);
		if (cur_time.count() / prev_time.count() > 1.2) {
			return assoc - 1;
		}
		prev_time = cur_time;
	}
	return std::nullopt;
}

std::pair<uint32_t, uint32_t> try_estimate_size_assoc() {
	uint32_t stride = MAX_STRIDE; 
	std::optional<uint32_t> prev_assoc = std::nullopt;
	while (stride >= 16) {
		auto assoc = try_estimate_assoc(stride);
		if (prev_assoc.has_value() && assoc.has_value() && (*prev_assoc) * 2 == *assoc) {
			return {stride * *assoc, *prev_assoc};
		}
		prev_assoc = assoc;
		stride >>= 1;
	}
	return {0, 0};
}

struct CacheSpecs {
	uint32_t capacity;
	uint32_t associativity;
	uint32_t block_size;
};

std::pair<uint32_t, uint32_t> detect_capacity_associativity() {
	std::map<std::pair<uint32_t, uint32_t>, uint32_t> count;
	for (int _ = 0; _ < 100; ++_) {
		++count[try_estimate_size_assoc()];
	}
	std::pair<uint32_t, uint32_t> key = {0, 0};
	for (const auto& [k, v] : count) {
		if (v > count[key]) {
			key = k;
		}
	}
	return key;
}

uint32_t try_estimate_block_size(uint32_t cap, uint32_t assoc) {
	uint32_t stride = cap / assoc;
	uint32_t bl = assoc / 2;
  	duration prev_time;
	for (int cur_block_size = 16; cur_block_size < 2 * 128; cur_block_size <<= 1) {
		for (int i = 0; i < bl - 1; ++i) {
			set_next_ptr(mem + stride * (i + 1), &mem[stride * i]);
		}
		char *next = mem + stride * bl + cur_block_size;
		set_next_ptr(next, &mem[stride * (bl - 1)]);
		for (int i = 0; i < bl; ++i) {
			set_next_ptr(next + stride * (i + 1), &next[stride * i]);
		}
		init_mem(stride, assoc / 2 + 1);
		set_next_ptr(mem, next + stride * bl);
		auto cur_time = time(stride, 1);
		if (prev_time.count() / cur_time.count() > 1.2) {
			return cur_block_size;
		}
		prev_time = cur_time;
	}
	return 0;
} 

uint32_t detect_cache_size(uint32_t cap, uint32_t assoc) {
	std::map<uint32_t, uint32_t> count;
	for (int _ = 0; _ < 100; ++_) {
		++count[try_estimate_block_size(cap, assoc)];
	}
	uint32_t key = 0;
	for (const auto& [k, v] : count) {
		if (v > count[key]) {
			key = k;
		}
	}
	return key;
}

int main() {
	auto [cap, assoc] = detect_capacity_associativity();
	std::cout << "Capacity: " << cap << "\n";
	std::cout << "Associativity: " << assoc << "\n";
	std::cout << "Block size: " << detect_cache_size(cap, assoc) << "\n";
	return 0;
}
