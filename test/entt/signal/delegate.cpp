#include <gtest/gtest.h>
#include <entt/signal/delegate.hpp>

int delegate_function(const int &i) {
    return i*i;
}

struct delegate_functor {
    int operator()(int i) {
        return i+i;
    }
};

struct const_nonconst_noexcept {
    void f() { ++cnt; }
    void g() noexcept { ++cnt; }
    void h() const { ++cnt; }
    void i() const noexcept { ++cnt; }
    mutable int cnt{0};
};

TEST(Delegate, Functionalities) {
    entt::delegate<int(int)> ffdel;
    entt::delegate<int(int)> mfdel;
    delegate_functor functor;

    ASSERT_FALSE(ffdel);
    ASSERT_FALSE(mfdel);
    ASSERT_EQ(ffdel, mfdel);

    ffdel.connect<&delegate_function>();
    mfdel.connect<&delegate_functor::operator()>(&functor);

    ASSERT_TRUE(ffdel);
    ASSERT_TRUE(mfdel);

    ASSERT_EQ(ffdel(3), 9);
    ASSERT_EQ(mfdel(3), 6);

    ffdel.reset();

    ASSERT_FALSE(ffdel);
    ASSERT_TRUE(mfdel);

    ASSERT_EQ(ffdel, entt::delegate<int(int)>{});
    ASSERT_NE(ffdel, mfdel);
}

TEST(Delegate, Comparison) {
    entt::delegate<int(int)> delegate;
    entt::delegate<int(int)> def;
    delegate.connect<&delegate_function>();

    ASSERT_EQ(def, entt::delegate<int(int)>{});
    ASSERT_NE(def, delegate);

    ASSERT_TRUE(def == entt::delegate<int(int)>{});
    ASSERT_TRUE (def != delegate);
}

TEST(Delegate, ConstNonConstNoExcept) {
    entt::delegate<void()> delegate;
    const_nonconst_noexcept functor;

    delegate.connect<&const_nonconst_noexcept::f>(&functor);
    delegate();

    delegate.connect<&const_nonconst_noexcept::g>(&functor);
    delegate();

    delegate.connect<&const_nonconst_noexcept::h>(&functor);
    delegate();

    delegate.connect<&const_nonconst_noexcept::i>(&functor);
    delegate();

    ASSERT_EQ(functor.cnt, 4);
}

TEST(Delegate, Constructors) {
    delegate_functor functor;
    entt::delegate<int(int)> empty{};
    entt::delegate<int(int)> func{entt::connect_arg<&delegate_function>};
    entt::delegate<int(int)> member{entt::connect_arg<&delegate_functor::operator()>, &functor};

    ASSERT_FALSE(empty);
    ASSERT_TRUE(func);
    ASSERT_TRUE(member);
}

TEST(Delegate, DeducedGuidelines) {
    const_nonconst_noexcept functor;

    entt::delegate func_deduced{entt::connect_arg<&delegate_function>};
    entt::delegate member_f_deduced{entt::connect_arg<&const_nonconst_noexcept::f>, &functor};
    entt::delegate member_g_deduced{entt::connect_arg<&const_nonconst_noexcept::g>, &functor};
    entt::delegate member_h_deduced{entt::connect_arg<&const_nonconst_noexcept::h>, &functor};
    entt::delegate member_i_deduced{entt::connect_arg<&const_nonconst_noexcept::i>, &functor};

    static_assert(std::is_same_v<typename decltype(func_deduced)::function_type, int(const int &)>);
    static_assert(std::is_same_v<typename decltype(member_f_deduced)::function_type, void()>);
    static_assert(std::is_same_v<typename decltype(member_g_deduced)::function_type, void()>);
    static_assert(std::is_same_v<typename decltype(member_h_deduced)::function_type, void()>);
    static_assert(std::is_same_v<typename decltype(member_i_deduced)::function_type, void()>);

    ASSERT_TRUE(func_deduced);
    ASSERT_TRUE(member_f_deduced);
    ASSERT_TRUE(member_g_deduced);
    ASSERT_TRUE(member_h_deduced);
    ASSERT_TRUE(member_i_deduced);
}
