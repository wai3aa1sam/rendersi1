#pragma once

#define RDS_ENABLE_FUZZ_TESTING 0

#if RDS_ENABLE_FUZZ_TESTING && !RDS_IS_ASAN_ENABLE
	#error "should enable asan with enable fuzz testing"
#endif //  RDS_IS_FUZZ_TESTING && !RDS_IS_ASAN_ENABLE

#ifndef RDS_ENABLE_BENCHMARK
	#define RDS_ENABLE_BENCHMARK 1
#endif