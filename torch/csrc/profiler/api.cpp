#include <torch/csrc/profiler/api.h>

namespace torch {
namespace profiler {
namespace impl {

ProfilerStubs::~ProfilerStubs() = default;

namespace {
struct DefaultStubs : public ProfilerStubs {
  DefaultStubs(const char* name) : name_{name} {}

  void record(
      int* /*device*/,
      ProfilerEventStub* /*event*/,
      int64_t* /*cpu_ns*/) const override {
    fail();
  }
  float elapsed(
      const ProfilerEventStub* /*event*/,
      const ProfilerEventStub* /*event2*/) const override {
    fail();
    return 0.f;
  }
  virtual float elapsed(const ProfilerEventStub* /*event*/) const override {
    fail();
    return 0.f;
  }
  void mark(const char* /*name*/) const override {
    fail();
  }
  void rangePush(const char* /*name*/) const override {
    fail();
  }
  void rangePop() const override {
    fail();
  }
  bool enabled() const override {
    return false;
  }
  void onEachDevice(std::function<void(int)> /*op*/) const override {
    fail();
  }
  void synchronize() const override {
    fail();
  }
  ~DefaultStubs() override = default;

 private:
  void fail() const {
    AT_ERROR(name_, " used in profiler but not enabled.");
  }

  const char* const name_;
};
} // namespace

#define REGISTER_DEFAULT(name, upper_name)                                   \
  namespace {                                                                \
  const DefaultStubs default_##name##_stubs{#upper_name};                    \
  constexpr const DefaultStubs* default_##name##_stubs_addr =                \
      &default_##name##_stubs;                                               \
                                                                             \
  /* Constant initialization, so it is guaranteed to be initialized before*/ \
  /* static initialization calls which may invoke register<name>Methods*/    \
  inline const ProfilerStubs*& name##_stubs() {                              \
    static const ProfilerStubs* stubs_ =                                     \
        static_cast<const ProfilerStubs*>(default_##name##_stubs_addr);      \
    return stubs_;                                                           \
  }                                                                          \
  } /*namespace*/                                                            \
                                                                             \
  const ProfilerStubs* name##Stubs() {                                       \
    return name##_stubs();                                                   \
  }                                                                          \
                                                                             \
  void register##upper_name##Methods(ProfilerStubs* stubs) {                 \
    name##_stubs() = stubs;                                                  \
  }

REGISTER_DEFAULT(cuda, CUDA)
REGISTER_DEFAULT(itt, ITT)
REGISTER_DEFAULT(xpu, XPU)
#undef REGISTER_DEFAULT

} // namespace impl
} // namespace profiler
} // namespace torch
