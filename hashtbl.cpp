#include <boost/preprocessor/repeat.hpp>
#include <tuple>
#include <utility>
#include <string>
#include <vector>
#include <iostream>
#include <array>
#include <cstring>
#include <type_traits>

bool comparator(const char* a, const char* b) {
	return strcmp(a, b) == 0;
}

struct lolkey {
	constexpr lolkey() = default;
};

bool comparator(lolkey, lolkey) {
	return true;
}

template <size_t IDX = 0, typename TPL, typename KEY, typename F>
bool execute_for_key(TPL tpl, KEY key, F functor) {
	if constexpr (std::tuple_size_v<TPL> > IDX) {
		if constexpr(std::is_same_v<std::decay_t<KEY>,
			std::decay_t<decltype(std::get<IDX>(tpl).key)>>) {
			if (comparator(std::get<IDX>(tpl).key, key)) {
				functor(std::get<IDX>(tpl).val);
				return true;
			}
		}
		return execute_for_key<IDX + 1>(tpl, key, functor);
	}
	return false;
}

constexpr size_t calc_hash(const char* s)
{
	size_t init_val = 997;
	while (*s != 0) {
		init_val *= size_t(*(s++)) * 37ULL ^ 85969ULL;
	}
	return init_val;
}

constexpr size_t calc_hash(lolkey) {
	return 7;
}

#define HASH_TABLE_ENTRY(key, val) \
hash_table_entry<calc_hash(key), decltype(key), decltype(val)> (key, val)

template <size_t hash, typename KEY, typename VAL>
struct hash_table_entry
{
	hash_table_entry(KEY k, VAL v) : key(k), val(v) {}
	const KEY key;
	const VAL val;

	static constexpr auto HASH = hash;
};

#define MAX_SWITCHES 100

#define NV_SWITCH_1(unused, idx, unused2) \
case idx: if constexpr (idx < std::tuple_size_v<TPL>) { return execute_for_key(std::get<idx>(tpl), key, functor); }

/*template <typename TPL, typename KEY, typename F>
bool execute(const TPL& tpl, KEY key, F functor) {
	const size_t bucket = calc_hash(key) % std::tuple_size_v<TPL>;

	static_assert(std::tuple_size_v<TPL> < MAX_SWITCHES);
	switch (bucket) {
		BOOST_PP_REPEAT(MAX_SWITCHES, NV_SWITCH_1, 1);
	default: return false;
	}
}*/

template <size_t BEG = 0, typename TPL, typename KEY, typename F>
bool execute(const TPL& tpl, KEY key, F functor) {
	const size_t bucket = calc_hash(key) % std::tuple_size_v<TPL>;

	switch (bucket) {
	case BEG * 5 + 0: if constexpr (BEG * 5 + 0 < std::tuple_size_v<TPL>) { return execute_for_key(std::get<BEG * 5 + 0>(tpl), key, functor); } return false;
	case BEG * 5 + 1: if constexpr (BEG * 5 + 1 < std::tuple_size_v<TPL>) { return execute_for_key(std::get<BEG * 5 + 1>(tpl), key, functor); } return false;
	case BEG * 5 + 2: if constexpr (BEG * 5 + 2 < std::tuple_size_v<TPL>) { return execute_for_key(std::get<BEG * 5 + 2>(tpl), key, functor); } return false;
	case BEG * 5 + 3: if constexpr (BEG * 5 + 3 < std::tuple_size_v<TPL>) { return execute_for_key(std::get<BEG * 5 + 3>(tpl), key, functor); } return false;
	case BEG * 5 + 4: if constexpr (BEG * 5 + 4 < std::tuple_size_v<TPL>) { return execute_for_key(std::get<BEG * 5 + 4>(tpl), key, functor); } return false;
	default:          if constexpr (BEG * 5 + 5 < std::tuple_size_v<TPL>) { return execute<BEG + 1>(tpl, key, functor); }
	}
	return false;
}


template <size_t BUCKET_ID, size_t BUCKET_COUNT, typename TPL, typename HEAD, typename ... REST>
constexpr auto build_bucket(TPL tpl, HEAD head, REST ... rest)
{
	static constexpr auto good_hash = HEAD::HASH % BUCKET_COUNT == BUCKET_ID;
	static constexpr auto more_elem = sizeof...(REST) > 0;

	if constexpr (more_elem && good_hash) {
		const auto new_tpl = std::tuple_cat(tpl, std::make_tuple(head));
		return build_bucket<BUCKET_ID, BUCKET_COUNT>(new_tpl, rest...);
	}

	if constexpr (more_elem && !good_hash) {
		return build_bucket<BUCKET_ID, BUCKET_COUNT>(tpl, rest...);
	}

	if constexpr (!more_elem && good_hash) {
		return std::tuple_cat(tpl, std::make_tuple(head));
	}

	if constexpr (!more_elem && !good_hash) {
		return tpl;
	}
}

template <typename ... ARGS, size_t ... N>
constexpr auto make_hash_table_new_impl(std::index_sequence<N...>, ARGS ... args)
{
	return std::tuple_cat(
		std::make_tuple(
			build_bucket<N, sizeof...(ARGS)>(
				std::tuple<>{}, args...
				)
		)...
	);
}

template<typename ... ARGS>
constexpr auto make_hash_table_v1(ARGS... args)
{
	return make_hash_table_new_impl(
		std::make_index_sequence<sizeof...(ARGS)>{}, args...
	);
}

int main() {
	const auto hashtable = make_hash_table_v1(
		HASH_TABLE_ENTRY("key123", 1),
		HASH_TABLE_ENTRY("key2", "chars"),
		HASH_TABLE_ENTRY(lolkey{}, nullptr),
		HASH_TABLE_ENTRY("key123", 1),
		HASH_TABLE_ENTRY("key2", "chars"),
		HASH_TABLE_ENTRY(lolkey{}, nullptr),
		HASH_TABLE_ENTRY("key123", 1),
		HASH_TABLE_ENTRY("key2", "chars"),
		HASH_TABLE_ENTRY(lolkey{}, nullptr),
		HASH_TABLE_ENTRY("key123", 1),
		HASH_TABLE_ENTRY("key2", "chars"),
		HASH_TABLE_ENTRY(lolkey{}, nullptr)
	);

	const auto func = [](auto val) {
		std::cout << typeid(val).name() << std::endl;
	};

	execute(hashtable, "key123", func);
	execute(hashtable, "key2", func);
	execute(hashtable, "key3", func);
	execute(hashtable, lolkey{}, func);
	system("pause");
}
