///
/// Events.h
/// Vorb Engine
///
/// Created by Cristian Zaloj on 8 Nov 2014
/// Copyright 2014 Regrowth Studios
/// All Rights Reserved
///
/// Summary:
/// C#-style events
///

#pragma once

#ifndef Events_h__
#define Events_h__

typedef const void* Sender; ///< A pointer to an object that sent the event
template<typename... Params> class Event;

template<typename Ret, typename... Args>
class RDelegate {
    template<typename... Params> friend class Event;
public:
    typedef void* Caller;
    typedef void* Function;
    typedef Ret(*CallStub)(Caller, Function, Args...); ///< Function type for a stub
    typedef void(*Deleter)(Caller);

    RDelegate(Caller c, Function f, CallStub s, Deleter d) :
        m_caller(c),
        m_func(f),
        m_stub(s),
        m_deletion(d) {
        // Empty
    }
    RDelegate() :
        m_stub(simpleCall),
        m_deletion(doNothing) {
        // Empty
    }
    ~RDelegate() {
        m_deletion(m_caller);
    }

    template<typename T>
    static RDelegate create(const T* o, Ret(T::*f)(Args...) const) {
        return RDelegate(const_cast<T*>(o), *(Function*)&f, objectCall<T>, doNothing);
    }
    template<typename T>
    static RDelegate* createCopy(const T* o, Ret(T::*f)(Args...) const) {
        T* no = (T*)operator new(sizeof(T));
        new (no)T(*o);
        return new RDelegate(no, *(Function*)&f, objectCall<T>, freeObject<T>);
    }
    template<typename T>
    static RDelegate create(T* o, Ret(T::*f)(Args...)) {
        return RDelegate(o, *(Function*)&f, objectCall<T>, doNothing);
    }
    template<typename T>
    static RDelegate* createCopy(T* o, Ret(T::*f)(Args...)) {
        T* no = (T*)operator new(sizeof(T));
        new (no) T(*o);
        return new RDelegate(no, *(Function*)&f, objectCall<T>, freeObject<T>);
    }
    static RDelegate create(Ret(*f)(Args...)) {
        return RDelegate(nullptr, f, simpleCall, doNothing);
    }

    Ret invoke(Args... args) const {
        return m_stub(m_caller, m_func, args...);
    }
    Ret operator()(Args... args) const {
        return m_stub(m_caller, m_func, args...);
    }

    bool operator==(const RDelegate& other) const {
        return m_func == other.m_func && m_caller == other.m_caller;
    }
    bool operator!=(const RDelegate& other) const {
        return m_func != other.m_func || m_caller != other.m_caller;
    }
protected:
    template<typename T>
    static Ret objectCall(Caller obj, Function func, Args... args) {
        typedef Ret(T::*fType)(Args...);
        
        // Properly cast internal values
        auto p = static_cast<T*>(obj);
        fType f = *(fType*)&func;

        // Call function using object
        return (p->*f)(args...);
    }
    static Ret simpleCall(Caller obj, Function func, Args... args) {
        typedef Ret(*fType)(Args...);

        // Call function
        return ((fType)func)(args...);
    }

    static void doNothing(Caller c) {
        // Empty
    }
    template<typename T>
    static void freeObject(Caller c) {
        T* obj = (T*)c;
        obj->~T();
        operator delete(c);
    }

    void neuter() {
        m_deletion = doNothing;
    }
    template<typename T>
    void engorge() {
        m_deletion = freeObject<T>;
    }

    Caller m_caller;
    Function m_func;
private:
    Deleter m_deletion;
    CallStub m_stub;
};

template<typename... Args>
class Delegate : public RDelegate<void, Args...> {
public:
    Delegate(Caller c, Function f, CallStub s, Deleter d) : RDelegate<void, Args...>(c, f, s, d) {
        // Empty
    }
    Delegate() : RDelegate<void, Args...>() {
        // Empty
    }

    template<typename T>
    static Delegate create(const T* o, void(T::*f)(Args...) const) {
        return Delegate(const_cast<T*>(o), *(Function*)&f, objectCall<T>, doNothing);
    }
    template<typename T>
    static Delegate* createCopy(const T* o, void(T::*f)(Args...) const) {
        T* no = (T*)operator new(sizeof(T));
        new (no)T(*o);
        return new Delegate(no, *(Function*)&f, objectCall<T>, freeObject<T>);
    }
    template<typename T>
    static Delegate create(T* o, void(T::*f)(Args...)) {
        return Delegate(o, *(Function*)&f, objectCall<T>, doNothing);
    }
    template<typename T>
    static Delegate* createCopy(T* o, void(T::*f)(Args...)) {
        T* no = (T*)operator new(sizeof(T));
        new (no)T(*o);
        return new Delegate(no, *(Function*)&f, objectCall<T>, freeObject<T>);
    }
    static Delegate create(void(*f)(Args...)) {
        return Delegate(nullptr, f, simpleCall, doNothing);
    }
};

template<typename Ret, typename... Args>
RDelegate<Ret, Args...> makeRDelegate(Ret(*f)(Args...)) {
    return RDelegate<Ret, Args...>::create(f);
}
template<typename T, typename Ret, typename... Args>
RDelegate<Ret, Args...> makeRDelegate(T& obj, Ret(T::*f)(Args...)const) {
    return RDelegate<Ret, Args...>::create<T>(&obj, f);
}
template<typename T, typename Ret, typename... Args>
RDelegate<Ret, Args...> makeRDelegate(T& obj, Ret(T::*f)(Args...)) {
    return RDelegate<Ret, Args...>::create<T>(&obj, f);
}
template<typename Ret, typename... Args, typename F>
RDelegate<Ret, Args...> makeRDelegate(F& obj) {
    typedef Ret(F::*fType)(Args...) const;
    fType f = &F::operator();
    return RDelegate<Ret, Args...>::create<F>(&obj, f);
}

template<typename Ret, typename... Args, typename F>
RDelegate<Ret, Args...>* makeRFunctor(F& obj) {
    typedef Ret(F::*fType)(Args...) const;
    fType f = &F::operator();
    return RDelegate<Ret, Args...>::createCopy<F>(&obj, f);
}
template<typename T, typename Ret, typename... Args>
RDelegate<Ret, Args...>* makeRFunctor(T& obj, Ret(T::*f)(Args...)const) {
    return RDelegate<Ret, Args...>::createCopy<T>(&obj, f);
}
template<typename T, typename Ret, typename... Args>
RDelegate<Ret, Args...>* makeRFunctor(T& obj, Ret(T::*f)(Args...)) {
    return RDelegate<Ret, Args...>::createCopy<T>(&obj, f);
}

template<typename... Args>
Delegate<Args...> makeDelegate(void(*f)(Args...)) {
    return Delegate<Args...>::create(f);
}
template<typename T, typename... Args>
Delegate<Args...> makeDelegate(T& obj, void(T::*f)(Args...)const) {
    return Delegate<Args...>::create<T>(&obj, f);
}
template<typename T, typename... Args>
Delegate<Args...> makeDelegate(T& obj, void(T::*f)(Args...)) {
    return Delegate<Args...>::create<T>(&obj, f);
}
template<typename... Args, typename F>
Delegate<Args...> makeDelegate(F& obj) {
    typedef void(F::*fType)(Args...) const;
    fType f = &F::operator();
    return Delegate<Args...>::create<F>(&obj, f);
}

template<typename... Args, typename F>
Delegate<Args...>* makeFunctor(F& obj) {
    typedef void(F::*fType)(Args...) const;
    fType f = &F::operator();
    return Delegate<Args...>::createCopy<F>(&obj, f);
}
template<typename T, typename... Args>
Delegate<Args...>* makeFunctor(T& obj, void(T::*f)(Args...)const) {
    return Delegate<Args...>::createCopy<T>(&obj, f);
}
template<typename T, typename... Args>
Delegate<Args...>* makeFunctor(T& obj, void(T::*f)(Args...)) {
    return Delegate<Args...>::createCopy<T>(&obj, f);
}

/// An event that invokes methods taking certain arguments
/// @tparam Params: Metadata sent in event invocation
template<typename... Params>
class Event {
public:
    typedef RDelegate<void, Sender, Params...> Listener; ///< Callback delegate type

    /// Create an event with a sender attached to it
    /// @param sender: Owner object sent with each invocation
    Event(Sender sender = nullptr) :
        m_sender(sender) {
        // Empty
    }

    /// Call all bound methods
    /// @param p: Arguments used in function calls
    void send(Params... p) {
        for (auto& f : m_funcs) {
            f(m_sender, p...);
        }
    }
    /// Call all bound methods
    /// @param p: Arguments used in function calls
    void operator()(Params... p) {
        this->send(p...);
    }

    /// Add a function to this event
    /// @param f: A subscriber
    /// @return The delegate passed in
    Event& add(Listener f) {
        f.neuter();
        m_funcs.push_back(f);
        return *this;
    }
    /// Add a function to this event
    /// @param f: A unknown-type subscriber
    /// @tparam F: Functor type
    /// @return The newly made delegate (CALLER DELETE)
    template<typename F>
    Listener* addFunctor(F f) {
        Listener* functor = makeRFunctor<void, Sender, Params...>(f);
        this->add(*functor);
        return functor;
    }
    /// Add a function to this event whilst allowing chaining
    /// @param f: A subscriber
    /// @return Self
    Event& operator +=(const Listener& f) {
        return add(f);
    }

    /// Remove a function (just one) from this event
    /// @param f: A subscriber
    Event& remove(const Listener& f) {
        auto fLoc = std::find(m_funcs.begin(), m_funcs.end(), f);
        if (fLoc == m_funcs.end()) return *this;
        m_funcs.erase(fLoc);
        return *this;
    }
    /// Remove a function (just one) from this event whilst allowing chaining
    /// @param f: A subscriber
    /// @return Self
    Event& operator -=(const Listener& f) {
        return remove(f);
    }
private:
    Sender m_sender; ///< Event owner
    std::vector<Listener> m_funcs; ///< List of bound functions (subscribers)
};

/// Manages destruction of generated delegates
class AutoDelegatePool {
public:
    typedef Delegate<>* Deleter; ///< A deletion function prototype
    typedef std::vector<Deleter> DeleterList; ///< A container of deleters

    /// Calls dispose
    virtual ~AutoDelegatePool() {
        dispose();
    }

    /// Binds a callback to an event and adds it for automatic destruction
    /// @tparam F: f's functor type
    /// @tparam Params: f's extra invocation parameters
    /// @param e: Event
    /// @param f: Callback function
    template<typename F, typename... Params>
    void addAutoHook(Event<Params...>& e, F f) {
        auto fd = e.addFunctor<F>(f);

        Deleter d = makeFunctor<>([fd, &e] () {
            e -= *fd;
            delete fd;
        });
        m_deletionFunctions.push_back(d);
    }

    /// Properly disposes all delegates added to this pool
    void dispose() {
        for (auto f : m_deletionFunctions) {
            f->invoke();
            delete f;
        }
        DeleterList().swap(m_deletionFunctions);
    }
private:
    DeleterList m_deletionFunctions; ///< List of delegates to be deleted
};

#endif // Events_h__