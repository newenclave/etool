#pragma once

namespace etool { namespace subsystems {
    class iface {
    public:
        virtual ~iface() = default;
        virtual void init() = 0;
        virtual void start() = 0;
        virtual void stop() = 0;
    };
}}
