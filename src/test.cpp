/* This file is part of the the dci project. Copyright (C) 2013-2021 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <dci/host/test.hpp>

namespace dci::host
{
    extern TestStage g_testStage;
    extern Manager* g_testManager;

    TestStage g_testStage = TestStage::null;
    Manager* g_testManager = nullptr;

    TestStage testStage()
    {
        return g_testStage;
    }

    Manager* testManager()
    {
        return g_testManager;
    }

}
