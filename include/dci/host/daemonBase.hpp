/* This file is part of the the dci project. Copyright (C) 2013-2022 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#pragma once

#include <dci/cmt.hpp>
#include <dci/host/module/serviceBase.hpp>

namespace dci::host
{
    template <class Concrete>
    class DaemonBase
        : public idl::gen::host::Daemon<>::Opposite
        , public module::ServiceBase<Concrete>
    {
        DaemonBase(const DaemonBase&) = delete;
        void operator=(const DaemonBase&) = delete;

    public:
        DaemonBase();
        ~DaemonBase();

    protected:
        void failedImpl(ExceptionPtr&& e);

    protected:
        cmt::task::Owner            _tol;
        cmt::task::Owner            _toStaring;
        String                      _name;
        idl::host::daemon::State    _state = idl::host::daemon::State::null;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Concrete>
    DaemonBase<Concrete>::DaemonBase()
        : idl::gen::host::Daemon<>::Opposite(idl::interface::Initializer())
    {
        //in state() -> daemon::State;
        methods()->state() += this->sol() * [this]
        {
            return cmt::readyFuture(_state);
        };

        //out stateChanged(daemon::State);
        //out failed(exception);

        //in setName(string name) -> void;
        methods()->setName() += this->sol() * [this](String&& name)
        {
            _name = std::move(name);
            return cmt::readyFuture(None{});
        };

        //in start(daemon::Params params) -> void;
        methods()->start() += this->sol() * [this](idl::Config&& config)
        {
            return cmt::spawnv<None>(_toStaring, [this, config=std::move(config)]() mutable
            {
                switch(_state)
                {
                case idl::host::daemon::State::null:
                case idl::host::daemon::State::stopped:
                    break;

                case idl::host::daemon::State::starting:
                case idl::host::daemon::State::started:
                case idl::host::daemon::State::stopping:
                case idl::host::daemon::State::failed:
                    throw idl::host::daemon::Error{"unable to start, bad state"};
                }

                _state = idl::host::daemon::State::starting;
                methods()->stateChanged(_state);

                try
                {
                    static_cast<Concrete*>(this)->startImpl(std::move(config));
                }
                catch(...)
                {
                    if(idl::host::daemon::State::starting == _state)
                    {
                        _state = idl::host::daemon::State::failed;
                        methods()->stateChanged(_state);
                    }

                    std::rethrow_exception(std::current_exception());
                }

                if(idl::host::daemon::State::starting == _state)
                {
                    _state = idl::host::daemon::State::started;
                    methods()->stateChanged(_state);
                }

                return None{};
            });
        };

        //in stop() -> void;
        methods()->stop() += this->sol() * [this]()
        {
            return cmt::spawnv<None>(_tol, [this]() mutable
            {
                _toStaring.flush();

                _state = idl::host::daemon::State::stopping;
                methods()->stateChanged(_state);

                static_cast<Concrete*>(this)->stopImpl();

                if(idl::host::daemon::State::stopping == _state)
                {
                    _state = idl::host::daemon::State::stopped;
                    methods()->stateChanged(_state);
                }

                return None{};
            });
        };

        //in service() -> interface;
        methods()->service() += this->sol() * [this]()
        {
            if constexpr(requires(Concrete* c) {{c->serviceImpl()} -> std::convertible_to<idl::Interface>;})
            {
                return cmt::spawnv<idl::Interface>(_tol, [this]() mutable
                {
                    return static_cast<Concrete*>(this)->serviceImpl();
                });
            }
            else
            {
                (void)this;
                return cmt::readyFuture<idl::Interface>(dci::exception::buildInstance<idl::gen::host::daemon::NoService>());
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Concrete>
    DaemonBase<Concrete>::~DaemonBase()
    {
        this->sol().flush();
        _toStaring.stop();
        _tol.stop();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Concrete>
    void DaemonBase<Concrete>::failedImpl(ExceptionPtr&& e)
    {
        if(idl::host::daemon::State::failed != _state)
        {
            _state = idl::host::daemon::State::failed;
            methods()->stateChanged(_state);
        }

        methods()->failed(std::move(e));
    }

}
