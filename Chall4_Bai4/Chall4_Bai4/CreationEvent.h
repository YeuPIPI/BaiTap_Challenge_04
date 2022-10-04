#pragma once
#ifndef _CreationEvent_h__
#define _CreationEvent_h__

#include <boost/container_hash/hash.hpp>

namespace CreationEvent {
    typedef boost::function<void(void)> TNotificationFunc;
    void registerCreationCallback(TNotificationFunc callback);
}

#endif#pragma once
