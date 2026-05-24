#include <compare>
#include <vector>

namespace mstd::test {

struct AllEqual {
  auto operator<=>(AllEqual) const noexcept {
    return std::weak_ordering::equivalent;
  }
};

struct WeaklyOrdered {
  int value = 0;

  std::weak_ordering operator<=>(WeaklyOrdered rhs) const noexcept {
    return value <=> rhs.value;
  }
  bool operator==(const WeaklyOrdered &) const = default;
};

struct BitOr1 {
  int value = 0;

  bool operator==(BitOr1 rhs) const noexcept {
    return (value | 1) == (rhs.value | 1);
  }

  std::weak_ordering operator<=>(BitOr1 rhs) const noexcept {
    return (value | 1) <=> (rhs.value | 1);
  }
};

struct Val {
  bool moved_from_ctor = false;
  bool moved_from_assign = false;
  int val;
  Val(int val = 0) : val(val) {}
  Val(const Val &other) : val(other.val) {}
  Val(Val &&other) : val(other.val) { other.moved_from_ctor = true; }
  Val &operator=(Val &&other) {
    val = other.val;
    other.moved_from_assign = true;
    return *this;
  }
  Val &operator=(const Val &other) {
    val = other.val;
    return *this;
  }
};

inline auto operator<=>(const Val &a, const Val &b) { return a.val <=> b.val; }

struct T
{
    int i;
};

inline std::strong_ordering operator<=>(T l, T r) { return l.i <=> r.i; }

struct U
{
};

class PathPoint
{
public:
    PathPoint(char t, const std::vector<double> &c)
        : type(t), coords(c) {}
    PathPoint(char t, std::vector<double> &&c)
        : type(t), coords(std::move(c)) {}
    char getType() const { return type; }
    const std::vector<double> &getCoords() const { return coords; }

private:
    char type;
    std::vector<double> coords;
};

struct PathPointCmp
{
    auto operator()(const PathPoint &__lhs, const PathPoint &__rhs) const
    {
        return __lhs.getType() <=> __rhs.getType();
    }
};

struct Zero
{
};
inline auto operator<=>(Zero, int i) { return 0 <=> i; }
inline auto operator<=>(int i, Zero) { return i <=> 0; }

struct  One
{
};
inline auto operator<=>(One, int i) { return 1 <=> i; }
inline auto operator<=>(int i, One) { return i <=> 1; }

struct Mapped
{
    Mapped() = default;
    explicit Mapped(const Mapped &) = default;
};

struct DefaultConstructibleType
{
    int val;

    DefaultConstructibleType() : val(123)
    {
    }

    DefaultConstructibleType(const DefaultConstructibleType &) = delete;
    DefaultConstructibleType(DefaultConstructibleType &&) = delete;

    DefaultConstructibleType &operator=(int x)
    {
        val = x;
        return *this;
    }
};

struct aggressive_aggregate
{
    int a;
    int b;
};

inline auto operator<=>(const aggressive_aggregate &a, const aggressive_aggregate &b)
{
    return a.a <=> b.a;
};

} // namespace mstd::test
