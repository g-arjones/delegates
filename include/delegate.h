#ifndef DLGT_DELEGATE_H_
#define DLGT_DELEGATE_H_

// #if (__cplusplus > 199711L)
// #  define DELEGATES_CPP11_SUPPORT
// #endif
namespace dlgt {
// #if defined (DELEGATES_CPP11_SUPPORT) && !defined(DELEGATES_TEST_PRE_CPP11)
// // we have C++11 support...yeah!
// #<{(|*
//  * non specialized template declaration for delegate
//  |)}>#
// template <typename T>
// class delegate;
//
// #<{(|*
//  * specialization for member functions
//  *
//  * \tparam T            class-type of the object who's member function to call
//  * \tparam R            return type of the function that gets captured
//  * \tparam params       variadic template list for possible arguments
//  *                      of the captured function
//  |)}>#
// template <typename T, typename R, typename... Params>
// class delegate<R (T::*)(Params...)>
// {
// public:
//     typedef R (T::*func_type)(Params...);
//
//     delegate(func_type func, T& callee)
//         : callee_(callee)
//         , func_(func)
//     {}
//
//     R operator()(Params... args) const
//     {
//         return (callee_.*func_)(args...);
//     }
//
//     bool operator==(const delegate& other) const
//     {
//         return (&callee_ == &other.callee_) && (func_ == other.func_);
//     }
//     bool operator!= (const delegate& other) const
//     {
//         return !((*this) == other);
//     }
//
// private:
//     T& callee_;
//     func_type func_;
// };
//
// #<{(|*
//  * specialization for const member functions
//  |)}>#
// template <typename T, typename R, typename... Params>
// class delegate<R (T::*)(Params...) const>
// {
// public:
//     typedef R (T::*func_type)(Params...) const;
//
//     delegate(func_type func, const T& callee)
//         : callee_(callee)
//         , func_(func)
//     {}
//
//     R operator()(Params... args) const
//     {
//         return (callee_.*func_)(args...);
//     }
//
//     bool operator==(const delegate& other) const
//     {
//         return (&callee_ == &other.callee_) && (func_ == other.func_);
//     }
//     bool operator!= (const delegate& other) const
//     {
//         return !(*this == other);
//     }
//
// private:
//     const T& callee_;
//     func_type func_;
// };
//
// #<{(|*
//  * specialization for free functions
//  *
//  * \tparam R            return type of the function that gets captured
//  * \tparam params       variadic template list for possible arguments
//  *                      of the captured function
//  |)}>#
// template <typename R, typename... Params>
// class delegate<R (*)(Params...)>
// {
// public:
//     typedef R (*func_type)(Params...);
//
//     delegate(func_type func)
//         : func_(func)
//     {}
//
//     R operator()(Params... args) const
//     {
//         return (*func_)(args...);
//     }
//
//     bool operator==(const delegate& other) const
//     {
//         return func_ == other.func_;
//     }
//     bool operator!= (const delegate& other) const
//     {
//         return !((*this) == other);
//     }
//
// private:
//     func_type func_;
// };
//
// #<{(|*
//  * function to deduce template parameters from call-context
//  |)}>#
// template <typename F, typename T>
// delegate<F> make_delegate(F func, T& obj)
// {
//     return delegate<F>(func, obj);
// }
//
// template <typename T>
// delegate<T> make_delegate(T func)
// {
//     return delegate<T>(func);
// }
//
// // a little backward compatilbility layer
// #define DELEGATE make_delegate
// #define DELEGATE_CONST make_delegate
// #define DELEGATE_FREE make_delegate
//
// #else // hmmm...no C++11 support...yuk!
template <typename T>
class delegate;

template <typename R>
class delegate<R()>
{
public:
    typedef R (*func_type)(void*);

    delegate(func_type func)
        : func_(func)
    {}

    R operator()() const
    {
        return (*func_)(_c);
    }

    /**
     * Checks if the function has been set to a target.
     */
    operator bool() const;

    bool operator==(const delegate& other) const
    {
        return (func_ == other.func_);
    }
    bool operator!= (const delegate& other) const
    {
        return !(*this == other);
    }

private:
    func_type func_;
    void* _c;
    delegate(func_type f, void* c = 0)
        : func_(f)
        , _c(c)
    {
    }
    template<class T, R (T::*M)()>
    static R member_function_caller(void* c)
    {
        return (static_cast<T*>(c)->*M)();
    }
    template<class T, R (T::*M)() const>
    static R member_function_caller(void* c)
    {
        return (static_cast<const T*>(c)->*M)();
    }
public:

    template<class T, R (T::*M)()>
    static delegate create(T& obj)
    {
        return delegate(&member_function_caller<T, M>, &obj);
    }
    template<class T, R (T::*M)() const>
    static delegate create(const T& obj)
    {
        return delegate(&member_function_caller<T, M>, (void*)&obj);
    }
};
template <typename R, typename P0>
class delegate<R(P0)>
{
public:
    typedef R (*func_type)(void*, P0);

    delegate(func_type func)
        : func_(func)
    {}

    R operator()(P0 p0) const
    {
        return (*func_)(_c, p0);
    }

    /**
     * Checks if the function has been set to a target.
     */
    operator bool() const
    {
        return func_ != create<&delegate::empty_function>()._f;
    }

    bool operator==(const delegate& other) const
    {
        return (func_ == other.func_);
    }
    bool operator!= (const delegate& other) const
    {
        return !(*this == other);
    }

private:
    func_type func_;
    void* _c;
    delegate(func_type f, void* c = 0)
        : func_(f)
        , _c(c)
    {
    }
    template<class T, R (T::*M)(P0)>
    static R member_function_caller(void* c, P0 p0)
    {
        return (static_cast<T*>(c)->*M)(p0);
    }
    template<class T, R (T::*M)(P0) const>
    static R member_function_caller(void* c, P0 p0)
    {
        return (static_cast<const T*>(c)->*M)(p0);
    }
    R empty_function(P0 /* p0 */)
    {
        return R();
    }
public:

    template<class T, R (T::*M)(P0)>
    static delegate create(T& obj)
    {
        return delegate(&member_function_caller<T, M>, &obj);
    }
    template<class T, R (T::*M)(P0) const>
    static delegate create(const T& obj)
    {
        return delegate(&member_function_caller<T, M>, (void*)&obj);
    }
};

// template <typename F, typename T>
// delegate<F> make_delegate(F func, T& obj)
// {
//     return delegate<R()>::create<T, &T::func>(&obj);
//     // return delegate<F>(func, obj);
// }
//
// template <typename T>
// delegate<T> make_delegate(T func)
// {
//     return delegate<T>(func);
// }
// auto d = DelegateTypeA::create<A, &A::simple>(a);
// auto d = make_delegate(&A::simple, a);
// template <typename R, typename P>
// class delegate<R (*)(P)>
// {
// public:
//     typedef R (*func_type)(P);
//
//     delegate(func_type func)
//         : func_(func)
//     {}
//
//     R operator()(P x) const
//     {
//         return (*func_)(x);
//     }
//
//     bool operator==(const delegate& other) const
//     {
//         return (func_ == other.func_);
//     }
//     bool operator!= (const delegate& other) const
//     {
//         return !((*this) == other);
//     }
//
// private:
//     func_type func_;
// };
// #<{(|*
//  * specialization for member functions with 0 parmeter
//  *
//  * \tparam T            class-type of the object who's member function to call
//  * \tparam R            return type of the function that gets captured
//  |)}>#
// template <typename T, typename R>
// class delegate<R (T::*)()>
// {
// public:
//     typedef R (T::*func_type)();
//
//     delegate(func_type func, T& callee)
//         : callee_(callee)
//         , func_(func)
//     {}
//
//     R operator()() const
//     {
//         return (callee_.*func_)();
//     }
//
//     bool operator==(const delegate& other) const
//     {
//         return (&callee_ == &other.callee_) && (func_ == other.func_);
//     }
//     bool operator!= (const delegate& other) const
//     {
//         return !((*this) == other);
//     }
//
// private:
//     T& callee_;
//     func_type func_;
// };
// template <typename T, typename R>
// class delegate<R (T::*)() const>
// {
// public:
//     typedef R (T::*func_type)() const;
//
//     delegate(func_type func, T& callee)
//         : callee_(callee)
//         , func_(func)
//     {}
//
//     R operator()() const
//     {
//         return (callee_.*func_)();
//     }
//
//     bool operator==(const delegate& other) const
//     {
//         return (&callee_ == &other.callee_) && (func_ == other.func_);
//     }
//     bool operator!= (const delegate& other) const
//     {
//         return !((*this) == other);
//     }
//
// private:
//     T& callee_;
//     func_type func_;
// };
// #<{(|*
//  * specialization for member functions with 1 parmeter
//  *
//  * \tparam T            class-type of the object who's member function to call
//  * \tparam R            return type of the function that gets captured
//  * \tparam P            type of first parameter of the captured function
//  |)}>#
// template <typename T, typename R, typename P>
// class delegate<R (T::*)(P)>
// {
// public:
//     typedef R (T::*func_type)(P);
//
//     delegate(func_type func, T& callee)
//         : callee_(callee)
//         , func_(func)
//     {}
//
//     R operator()(P arg) const
//     {
//         return (callee_.*func_)(arg);
//     }
//
//     bool operator==(const delegate& other) const
//     {
//         return (&callee_ == &other.callee_) && (func_ == other.func_);
//     }
//     bool operator!= (const delegate& other) const
//     {
//         return !((*this) == other);
//     }
//
// private:
//     T& callee_;
//     func_type func_;
// };
// template <typename T, typename R, typename P>
// class delegate<R (T::*)(P) const>
// {
// public:
//     typedef R (T::*func_type)(P) const;
//
//     delegate(func_type func, T& callee)
//         : callee_(callee)
//         , func_(func)
//     {}
//
//     R operator()(P arg) const
//     {
//         return (callee_.*func_)(arg);
//     }
//
//     bool operator==(const delegate& other) const
//     {
//         return (&callee_ == &other.callee_) && (func_ == other.func_);
//     }
//     bool operator!= (const delegate& other) const
//     {
//         return !((*this) == other);
//     }
//
// private:
//     T& callee_;
//     func_type func_;
// };
// template <typename T, typename R, typename P1, typename P2>
// class delegate<R (T::*)(P1, P2)>
// {
// public:
//     typedef R (T::*func_type)(P1, P2);
//
//     delegate(func_type func, T& callee)
//         : callee_(callee)
//         , func_(func)
//     {}
//
//     R operator()(P1 arg, P2 arg2) const
//     {
//         return (callee_.*func_)(arg, arg2);
//     }
//
//     bool operator==(const delegate& other) const
//     {
//         return (&callee_ == &other.callee_) && (func_ == other.func_);
//     }
//     bool operator!= (const delegate& other) const
//     {
//         return !((*this) == other);
//     }
//
// private:
//     T& callee_;
//     func_type func_;
// };
// template <typename T, typename R, typename P1, typename P2, typename P3>
// class delegate<R (T::*)(P1, P2, P3)>
// {
// public:
//     typedef R (T::*func_type)(P1, P2, P3);
//
//     delegate(func_type func, T& callee)
//         : callee_(callee)
//         , func_(func)
//     {}
//
//     R operator()(P1 arg, P2 arg2, P3 arg3) const
//     {
//         return (callee_.*func_)(arg, arg2, arg3);
//     }
//
//     bool operator==(const delegate& other) const
//     {
//         return (&callee_ == &other.callee_) && (func_ == other.func_);
//     }
//     bool operator!= (const delegate& other) const
//     {
//         return !((*this) == other);
//     }
//
// private:
//     T& callee_;
//     func_type func_;
// };
// template <typename T, typename R, typename P1, typename P2, typename P3, typename P4>
// class delegate<R (T::*)(P1, P2, P3, P4)>
// {
// public:
//     typedef R (T::*func_type)(P1, P2, P3, P4);
//
//     delegate(func_type func, T& callee)
//         : callee_(callee)
//         , func_(func)
//     {}
//
//     R operator()(P1 arg, P2 arg2, P3 arg3, P4 arg4) const
//     {
//         return (callee_.*func_)(arg, arg2, arg3, arg4);
//     }
//
//     bool operator==(const delegate& other) const
//     {
//         return (&callee_ == &other.callee_) && (func_ == other.func_);
//     }
//     bool operator!= (const delegate& other) const
//     {
//         return !((*this) == other);
//     }
//
// private:
//     T& callee_;
//     func_type func_;
// };
/**
 * function to deduce template parameters from call-context
 */

// #endif // DELEGATES_CPP11_SUPPORT

} // namespace delegate

#endif /* end of include guard */
