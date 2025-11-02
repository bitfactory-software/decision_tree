#include <catch2/catch_test_macros.hpp>
#include <co_go/resource.hpp>
#include <mutex>
#include <print>

namespace {
std::mutex m;
const bool mt_run = true;
}  // namespace

TEST_CASE("simple [resource]") {
  bool run = false;
  bool catched = false;
  try {
    auto lg{[&](bool log) -> co_go::resource<std::mutex> {
      if (log) std::println("Before locking");
      if (mt_run) m.lock();
      co_yield m;
      if (mt_run) m.unlock();
      if (log) std::println("After locking");
    }(true)};
    std::println("process...");
    run = true;
  } catch (...) {
    catched = true;
  }
  CHECK(run);
  CHECK(!catched);
}

TEST_CASE("throw [resource]") {
  bool run = false;
  bool catched = false;
  try {
    auto lg{[&](bool log) -> co_go::resource<std::mutex> {
      if (log) std::println("Before locking");
      if (mt_run) m.lock();
      co_yield m;
      if (mt_run) m.unlock();
      if (log) std::println("After locking");
    }(true)};
    throw 0;
    //std::println("process...");  // unreachable code
    //run = true;// unreachable code
  } catch (...) {
    catched = true;
  }
  CHECK(!run);
  CHECK(catched);
}
