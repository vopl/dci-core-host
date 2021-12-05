/* This file is part of the the dci project. Copyright (C) 2013-2021 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include "dll.hpp"
#include <map>

namespace dci::host
{
    boost::dll::shared_library& dll(const std::string path)
    {
        static std::map<std::string, boost::dll::shared_library> all;
        boost::dll::shared_library& sl = all[path];
        if(!sl.is_loaded())
            sl.load(path, boost::dll::load_mode::rtld_now | boost::dll::load_mode::rtld_local /*| boost::dll::load_mode::rtld_deepbind*/);
        return sl;
    }
}
