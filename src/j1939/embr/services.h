#pragma once

#ifdef ESP_PLATFORM
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_sntp.h"
#endif

#include <estd/tuple.h>

#include <embr/bits/bits.hpp>

namespace embr {

struct service_container;

struct service_container_base
{
    enum class states
    {
        stopped,
        started,

        dependency,     ///< waiting on a dependency
        error,
    };


    // We track intermediate states here also.  This means
    // we linger on main state until intermediate state is completed.
    // i.e. we stay on 'stopped' state while substate transitions between
    // 'unstarted' and 'starting'.  Only when 'starting' is complete do
    // we then mark as 'started'
    enum class substates
    {
        // running states
        running,
        connecting,
        online,
        disconnecting,
        offline,
        degraded,
        pausing,
        stopping,
        resetting,

        // stopped states
        unstarted,
        configuring,        ///< pre-start step announcing preliminiry configuration
        configured,         ///< pre-start step finishing preliminiry configuration
        finished,
        starting,
        paused,
        resuming,

        // error states
        err_config,         ///< service configuration error, usually on startup or configuring
        err_memory,         ///< service ran out of memory, or detected memory corruption
        err_unspecified,    ///< internal error code was not recognized or provided
    };

protected:
    struct
    {
        states state_ : 8;
        substates substate__ : 8;
    };

    constexpr service_container_base() :
        state_{states::stopped},
        substate__{substates::unstarted}
    {}

public:
    constexpr states state() const { return state_; }
    constexpr substates substate() const { return substate__; }
};

struct service_manager
{
    service_container* head;

    // The first node which is interested in substate notifications.
    // Not a fantastic way to optimize the heavy traffic substate
    // notifications generate, but it's something
    service_container* substate_head;

    void state_changed(service_container& state_changed_service, int substate_category);
    void add(service_container&);
    service_container* get(unsigned id);
    // NOTE: name pointer must match exactly services::id const char[]
    service_container* get(const char* name);

/*
    template <class TService>
    void start(TService* s)
    {
        s->start();
    } */

    template <class TService, class ...TArgs>
    void start(TService& s, TArgs...args);
};

}

namespace services {

extern embr::service_manager manager;

namespace id {

extern const char events[];

}

}

const char* description(embr::service_container_base::states s);
const char* description(embr::service_container_base::substates s);

namespace embr {

// NOTE: Don't get too lost in a full service API.  For now just
// do a barebones one to keep things organized
// DEBT: Sadly, estd's linked lists are a mess, so rolling my own here
// TODO: Move this out to lib area where we can unit test it
struct service_container : service_container_base
{
    friend class service_manager;

    // DEBT: May not need changed_flag at all if our brute force
    // semi-event-list continues to play out
    //static embr::bits::experimental::word<32> changed_flag;
    static embr::bits::internal::word<32> changed_flag;
    static unsigned id_counter;

    service_container* next_;
    //service_container* child_;

    const unsigned id_;

    // TODO: app specific/sub states

    const char* name_;

protected:
    embr::bits::internal::word<32> interested_;

    // DEBT: Our 'substate' indicator looks like it may need to
    // be a bitmask
    void on_state_changed(int substate_category = 0)
    {
        /*
        if(child_ == nullptr) return;

        child_->parent_state_changed(this);

        service_container* next = child_->next_;

        while(next != nullptr)
        {
            next->parent_state_changed(this);
            next = next->next_;
        } */
        // DEBT: 1U expectation is fiddly, clean that up
        changed_flag.set(embr::bits::descriptor{id_, 1U}, true);
        services::manager.state_changed(*this, substate_category);
    }

    void state(states v, substates ssv)
    {
        if(state_ != v)
        {
            state_ = v;

            if(substate__ != ssv)
            {
                substate__ = ssv;
                on_state_changed(-1);
            }
            else
            {
                on_state_changed();
            }
        }
        else if(substate__ != ssv)
        {
            substate__ = ssv;
            on_state_changed(-1);
        }
    }

    void state(states v)
    {
        if(state_ != v)
        {
            state_ = v;
            switch(v)
            {
                case states::stopped:
                    substate__ = substates::finished;
                    break;

                case states::started:
                    substate__ = substates::running;
                    break;

                case states::error:
                    substate__ = substates::err_unspecified;
                    break;

                default: break;
            }
            on_state_changed();
        }
    }

    // DEBT: Make DEBUG/diagnostic mode which warns developer if they are in the wrong main state
    // DEBT: -1 a pretty lousy indicator for 'system' substates
    void state(substates v, int substate_category = -1)
    {
        if(substate__ != v)
        {
            substate__ = v;
            on_state_changed(substate_category);
        }
    }

    /// Convenience method, as I spent way too much time wondering
    /// why I couldn't call 'state'............ 
    inline void substate(substates v) { state(v); }

public:
    constexpr bool is_interested(unsigned id) const
    {
        return interested_.get(embr::bits::descriptor(1 << id, 1));
    }

    constexpr states state() const { return state_; }
    constexpr substates substate() const { return substate__; }

    // NOTE: 'substate' indicates category, not the state itself.
    // service-specific inspection of service_container must occur
    // to gain actual substate
    virtual void state_changed(service_container& s, int substate_category)
    {

    }

    inline service_container(const char* name) : 
        next_{nullptr},
        /*
        child_{nullptr}, */
        id_{++id_counter},
        name_{name},
        // All 1s = interested in all services' notifications
        interested_{0xFFFFFFFF}
    {
        /*
        if(depends_on != nullptr)
        {
            if(depends_on->child_ != nullptr)
                next_ = depends_on->child_->next_;

            depends_on->child_ = this;
        } */
    }

    const char* name() const { return name_; }
};



template <class ...TArgs>
struct service_container_with_dependencies : service_container
{
    estd::tuple<TArgs...> dependencies;
    //TArgs... dependencies;

    void evaluate_child(service_container& incoming) {}

    template <class ...TRemainingDependencies>
    void evaluate_child(service_container& incoming, service_container& s, TRemainingDependencies&&...args)
    {
        evaluate_child(std::forward<TRemainingDependencies>(args)...);
    }

    void state_changed(service_container& s, int substate) override
    {
        // Error:
        // error: 'index' is not a member of 'estd::tuple<event_service&, wifi_service&>&'
        /*
        estd::apply([&](TArgs&... t)
        {

            evaluate_child(s, t...);

        }, dependencies); */
    }

    service_container_with_dependencies(const char* name, TArgs&&...args) :
        service_container(name),
        dependencies(std::forward<TArgs>(args)...)
    {

    }
};


// DEBT: Templatize this
struct event_service : service_container
{
    event_service() : service_container(services::id::events) {}

    void start();
    void stop();
};

}