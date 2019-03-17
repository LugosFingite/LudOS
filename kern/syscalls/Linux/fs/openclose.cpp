/*
openclose.cpp

Copyright (c) 20 Yann BOUCHER (yann)

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

#include "syscalls/Linux/syscalls.hpp"

#include "tasking/process.hpp"
#include "sys/fnctl.h"
#include "errno.h"
#include "utils/user_ptr.hpp"

#include "fs/fsutils.hpp"
#include "fs/vfs.hpp"
#include "fs/pipe.hpp"

#include "utils/logging.hpp"

int sys_open(user_ptr<const char> path, int flags, int mode)
{
    if (!path.check())
    {
        return -EFAULT;
    }

    auto result = vfs::user_find(path.get());
    if (result.target_node == nullptr)
    {
        return -result.error;
    }

    if (flags & O_TRUNC)
    {
        // TODO :
        warn("O_TRUNC unsupported\n");
        return -ENOSYS;
    }
    if (flags & O_CREAT)
    {
        // TODO :
        warn("O_CREAT unsupported\n");
        return -ENOSYS;
    }

    if (flags & O_EXCL)
    {
        return -EEXIST;
    }

    if (flags & O_DIRECTORY && result.target_node->type() != vfs::node::Directory)
    {
        return -ENOTDIR;
    }

    tasking::FDInfo info;
    info.node = result.target_node;
    info.cursor = 0;

    const auto perms = result.target_node->stat().perms;
    if ((flags & 0b11) == O_RDONLY || (flags & 0b11) == O_RDWR)
    {
        if (!Process::current().check_perms(perms, result.target_node->stat().uid,
                                            result.target_node->stat().gid, Process::AccessRequestPerm::ReadRequest))
        {
            return -EACCES;
        }
        info.read = true;
    }
    if ((flags & 0b11) == O_WRONLY || (flags & 0b11) == O_RDWR)
    {
        if (!Process::current().check_perms(perms, result.target_node->stat().uid,
                                            result.target_node->stat().gid, Process::AccessRequestPerm::WriteRequest))
        {
            return -EACCES;
        }
        info.write = true;
    }

    if (flags & O_APPEND)
    {
        info.append = true;
    }

    if (result.target_node->type() == vfs::node::FIFO)
    {
        info.node = vfs::pipe::open_fifo(result.target_node);
    }

    int fd = Process::current().add_fd(info);

    return fd;
}

int sys_close(unsigned int fd)
{
    if (!Process::current().get_fd(fd))
    {
        return -EBADFD;
    }

    Process::current().close_fd(fd);

    return EOK;
}
