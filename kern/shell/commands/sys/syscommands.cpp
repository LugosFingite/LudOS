/*
syscommands.cpp

Copyright (c) 23 Yann BOUCHER (yann)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "syscommands.hpp"

#include <kstring/kstring_view.hpp>

#include "utils/memutils.hpp"

#include "utils/kmsgbus.hpp"

#include "shell/shell.hpp"
#include "mem/meminfo.hpp"
#include "power/powermanagement.hpp"
#include "time/time.hpp"
#include "drivers/pci/pci.hpp"
#include "drivers/pci/pci_vendors.hpp"
#include "drivers/driver.hpp"

void install_sys_commands(Shell &sh)
{
    sh.register_command(
    {"meminfo", "Print memory info",
     "Usage : 'meminfo'",
     [](const std::vector<kpp::string>&)
     {
         kprintf("Total memory : %s\n", human_readable_size(MemoryInfo::total()).c_str());
         kprintf("Free memory : %s\n", human_readable_size(MemoryInfo::free()).c_str());
         kprintf("Allocated memory : %s\n", human_readable_size(MemoryInfo::allocated()).c_str());
         kprintf("Used memory : %s\n", human_readable_size(MemoryInfo::used()).c_str());
         kprintf("Maximal Used memory : %s\n", human_readable_size(MemoryInfo::max_usage()).c_str());
         return 0;
     }});

    sh.register_command(
    {"alloc_dump", "Print malloc info",
     "Usage : 'alloc_dump'",
     [](const std::vector<kpp::string>&)
     {
         liballoc_dump();
         return 0;
     }});

    sh.register_command(
    {"halt", "stops computer",
     "Usage : 'halt'",
     [](const std::vector<kpp::string>&)
     {
         kmsgbus.send(ShutdownMessage{});
         return 0;
     }});

    sh.register_command(
    {"reboot", "reboots computer",
     "Usage : 'reboot'",
     [](const std::vector<kpp::string>&)
     {
         kmsgbus.send(ResetMessage{});
         return 0;
     }});

    sh.register_command(
    {"date", "prints current date",
     "Usage : 'date'",
     [](const std::vector<kpp::string>&)
     {
         auto date = Time::to_local_time(Time::get_time_of_day());
         kprintf("%s\n", Time::to_string(date));
         return 0;
     }});

    sh.register_command(
    {"uptime", "prints uptime",
     "Usage : 'uptime'",
     [](const std::vector<kpp::string>&)
     {
         kprintf("Uptime : %f sec\n", Time::uptime());
         return 0;
     }});

    sh.register_command(
    {"lspci", "list pci devices",
     "Usage : 'lspci'",
     [](const std::vector<kpp::string>&)
     {
         for (const auto& dev : pci::devices)
         {
             kprintf("%x:%x.%x:\n", dev.bus, dev.slot, dev.func);
             kprintf("   Vendor : '%s' (0x%x)\n", pci::vendor_string(dev.vendorID).c_str(), dev.vendorID);
             kprintf("   Device : '%s' (0x%x)\n", pci::dev_string(dev.vendorID, dev.deviceID).c_str(), dev.deviceID);
             kprintf("   Class : '%s' (0x%x:0x%x:0x%x)\n", pci::class_code_string(dev.classCode, dev.subclass, dev.progIF).c_str(),
                                                           dev.classCode, dev.subclass, dev.progIF);
         }
         return 0;
     }});  

    sh.register_command(
    {"lsdrv", "list active drivers",
     "Usage : 'lsdrv'",
     [](const std::vector<kpp::string>&)
     {
         for (Driver& driver : Driver::list())
         {
             kprintf("\t%s (%s)\n", driver.driver_name().c_str(), DriverType::to_string(driver.type()));
         }
         return 0;
     }});

    sh.register_command(
    {"epoch", "print current unix time",
     "Usage : 'epoch'",
     [](const std::vector<kpp::string>&)
     {
         kprintf("%d\n", Time::epoch());
         return 0;
     }});
}
