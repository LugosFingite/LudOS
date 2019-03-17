/*
fscommands.cpp

Copyright (c) 24 Yann BOUCHER (yann)

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

#include "fscommands.hpp"

#include <sys/fs.h>
#include <sys/fnctl.h>
#include <syscalls/syscall_list.hpp>
#include <errno.h>

#include "../shell.hpp"

#include "terminal/escape_code_macros.hpp"

void install_fs_commands(Shell &sh)
{
    sh.register_command(
    {"pwd", "print current directory",
     "Usage : pwd",
     [](const std::vector<std::string>&)
     {
         char buf[512];
         getcwd(buf, 512);
         puts(buf);
         return 0;
     }});

//    sh.register_command(
//    {"ls", "list current directory",
//     "Usage : ls",
//     [&sh](const std::vector<std::string>&)
//     {
//         auto vec = sh.pwd->readdir();
//         std::sort(vec.begin(), vec.end(), [](const auto& lhs, const auto& rhs)
//         { return lhs->name() == "." || lhs->name() == ".." ? false : lhs->name() < rhs->name(); });

//         for (const auto& entry : vec)
//         {
//             if (entry->is_link()) printf(ESC_FG(0, 118, 201));

//             printf("\t%s", entry->name().c_str());
//             if (entry->type() == vfs::node::Directory)
//             {
//                 printf("/");
//                 if (entry->is_link()) printf(ESC_POP_COLOR);
//             }
//             else
//             {
//                 if (entry->is_link()) printf(ESC_POP_COLOR);
//                 printf("\t: %s", human_readable_size(entry->size()).c_str());
//             }
//             printf("\n");
//         }
//         return 0;
//     }});

    sh.register_command(
    {"cd", "change directory",
     "Usage : cd <dir>",
     [&sh](const std::vector<std::string>& args)
     {
         std::string target = "/";
         if (!args.empty()) target = args[0];

         int ret = chdir(sh.get_path(target).c_str());

         if (ret < 0)
         {
             sh.error("cd error for path %s : %s\n", target.c_str(), strerror(errno));
             return -2;
         }

         return 0;
     }});

//    sh.register_command(
//    {"cat", "print file contents",
//     "Usage : cat <file> (offset) (size)",
//     [&sh](const std::vector<std::string>& args)
//     {
//         std::string path = "/";

//         if (!args.empty())
//         {
//             path = args[0];
//         }

//         auto node = vfs::find(sh.get_path(path)).value_or(nullptr);
//         if (!node)
//         {
//             sh.error("file not found : '%s'\n", path.c_str());
//             return -2;
//         }

//         size_t offset = 0;
//         size_t size = node->size() ?: 0x1000;

//         if (args.size() >= 2)
//         {
//             offset = std::stoul(args[1]);
//         }
//         if (args.size() >= 3)
//         {
//             size = std::stoul(args[2]);
//         }

//         auto result = node->read(offset, size);
//         if (!result)
//         {
//             sh.error("Error reading file %s : %s\n", args[0].c_str(), result.error().to_string());
//         }

//         auto data = std::move(result.value());
//         if (data.empty())
//         {
//             sh.error("%s returned empty\n", path.c_str());
//             return -3;
//         }
//         data.emplace_back('\0');

//         puts(reinterpret_cast<const char*>(data.data()));

//         return 0;
//     }});

//    sh.register_command(
//    {"tree", "print current fs tree",
//     "Usage : tree",
//     [&sh](const std::vector<std::string>&)
//     {
//         vfs::traverse(*sh.pwd);
//         return 0;
//     }});

//    sh.register_command(
//    {"lsblk", "list current drives",
//     "Usage : lsblk",
//     [](const std::vector<std::string>&)
//     {
//         for (Disk& disk : Disk::disks())
//         {
//             printf("%s : %s (%s) (%s)\n", disk.drive_name().c_str(), human_readable_size(disk.disk_size()).c_str(),
//             disk.read_only() ? "ro" : "rw", disk.caching_enabled() ? "cached" : "uncached");
//         }
//         return 0;
//     }});

//    sh.register_command(
//    {"df", "list current file systems",
//     "Usage : df",
//     [](const std::vector<std::string>&)
//     {
//         for (FileSystem& fs : FileSystem::fs_list())
//         {
//             printf("%s on %s : %s/%s (%d%%)\n", fs.type().c_str(), fs.disk().drive_name().c_str(),
//             human_readable_size(fs.total_size()-fs.free_size()).c_str(), human_readable_size(fs.total_size()).c_str(),
//             100-int((double)fs.free_size()/fs.total_size()*100.));
//         }
//         return 0;
//     }});

//    sh.register_command(
//    {"crc32", "print file crc32 checksum",
//     "Usage : crc32 <file>",
//     [&sh](const std::vector<std::string>& args)
//     {
//         std::string path = "/";

//         if (!args.empty())
//         {
//             path = args[0];
//         }

//         auto node = vfs::find(sh.get_path(path)).value_or(nullptr);
//         if (!node)
//         {
//             sh.error("file not found : '%s'\n", path.c_str());
//             return -2;
//         }

//         size_t crc32;
//         for (size_t i { 0 }; i < 600; ++i)
//         {
//             auto result = node->read();
//             if (!result)
//             {
//                 sh.error("Error reading file %s : %s\n", args[0].c_str(), result.error().to_string());
//             }

//             auto data = std::move(result.value());

//             if (i == 0) crc32 = crc(data.begin(), data.end());
//             else
//             {
//                 size_t temp = crc(data.begin(), data.end());
//                 if (temp != crc32)
//                 {
//                     printf("Error ! %d\n", i);
//                 }
//                 crc32 = temp;
//             }
//         }

//         return 0;
//     }});

//    sh.register_command(
//    {"dump", "dump file",
//     "Usage : 'dump <file> (offset) (size)'",
//     [&sh](const std::vector<std::string>& args)
//     {
//         std::string path = "/";

//         if (!args.empty())
//         {
//             path = args[0];
//         }

//         auto node = vfs::find(sh.get_path(path)).value_or(nullptr);
//         if (!node)
//         {
//             sh.error("file not found : '%s'\n", path.c_str());
//             return -2;
//         }

//         size_t offset = 0;

//         if (args.size() >= 2)
//         {
//             offset = std::stoul(args[1]);
//         }

//         size_t size = node->size() - offset;
//         if (args.size() >= 3)
//         {
//             size = std::stoul(args[2]);
//         }

//         auto result = node->read(offset, size);
//         if (!result)
//         {
//             sh.error("Error reading file %s : %s\n", args[0].c_str(), result.error().to_string());
//             return -3;
//         }

//         auto data  = std::move(result.value());

//         if (data.empty())
//         {
//             sh.error("Cannot read file : '%s'\n", path.c_str());
//             return -3;
//         }

//         dump(data.data(), data.size());

//         return 0;
//     }});

//    sh.register_command(
//    {"write", "write to file",
//     "Usage : 'write <file> <data> (offset)'",
//     [&sh](const std::vector<std::string>& args)
//     {
//         std::string path = "/";

//         if (args.size() < 2)
//         {
//             sh.error("'write' needs at least 2 arguments\n");
//             return -1;
//         }
//         path = args[0];

//         auto node = vfs::find(sh.get_path(path)).value_or(nullptr);
//         if (!node)
//         {
//             sh.error("file not found : '%s'\n", path.c_str());
//             return -2;
//         }

//         std::string data = args[1];

//         size_t offset = 0;
//         if (args.size() >= 3) offset = std::stoul(args[2]);

//         auto result = node->write(offset, {(uint8_t*)data.data(), (int)data.size()});
//         if (!result)
//         {
//             sh.error("Cannot write to file : '%s' (%s)\n", path.c_str(), result.error().to_string());
//             return -3;
//         }

//         return 0;
//     }});


//    sh.register_command(
//    {"ren", "rename file",
//     "Usage : 'ren <old> <new>'",
//     [&sh](const std::vector<std::string>& args)
//     {
//         std::string path = "/";

//         if (args.size() != 2)
//         {
//             sh.error("'ren' needs 2 arguments\n");
//             return -1;
//         }
//         path = args[0];

//         auto node = vfs::find(sh.get_path(path)).value_or(nullptr);
//         if (!node)
//         {
//             sh.error("file not found : '%s'\n", path.c_str());
//             return -2;
//         }

//         node->rename(args[1]);
//         // TODO :

//         return 0;
//     }});

//    sh.register_command(
//    {"sync", "flush disks cache",
//     "Usage : sync",
//     [](const std::vector<std::string>&)
//     {
//         kmsgbus.send(SyncDisksCache{});
//         return 0;
//     }});

//    // TODO : do this to others (le sh.get_path)
//    sh.register_command(
//    {"mkdir", "creates a directory",
//     "Usage : mkdir <name>",
//     [&sh](const std::vector<std::string>& args)
//     {
//         if (args.size() != 1)
//         {
//             sh.error("mkdir needs one argument !\n");
//             return -1;
//         }

//         std::shared_ptr<vfs::node> node = vfs::find(sh.get_path(parent_path(args[0]))).value_or(nullptr);
//         if (!node)
//         {
//             sh.error("Could not find path '%s'\n", sh.get_path(parent_path(args[0])).c_str());
//             return -3;
//         }

//         if (!node->create(filename(args[0]).to_string(), vfs::node::Directory))
//         {
//             sh.error("Could not create directory '%s'\n", filename(args[0].c_str()).c_str());
//             return -3;
//         }

//         return 0;
//     }});

//    sh.register_command(
//    {"touch", "creates a file",
//     "Usage : touch <name>",
//     [&sh](const std::vector<std::string>& args)
//     {
//         if (args.size() != 1)
//         {
//             sh.error("touch needs one argument !\n");
//             return -1;
//         }

//         if (!sh.pwd->create(args[0], vfs::node::File))
//         {
//             sh.error("Could not create file '%s'\n", args[0].c_str());
//             return -2;
//         }

//         return 0;
//     }});

//    sh.register_command(
//    {"resize", "resizes a file",
//     "Usage : resize <name> <size>",
//     [&sh](const std::vector<std::string>& args)
//     {
//         if (args.size() != 2)
//         {
//             sh.error("resize needs two arguments !\n");
//             return -1;
//         }

//         auto node = vfs::find(sh.get_path(args[0])).value_or(nullptr);
//         if (!node)
//         {
//             sh.error("Can't find '%s' !\n", args[0].c_str());
//             return -2;
//         }

//         if (!node->resize(std::stoul(args[1])))
//         {
//             sh.error("Could not resize file '%s' to length %d\n", args[0].c_str(), std::stoul(args[1]));
//             return -3;
//         }

//         return 0;
//     }});

//    // TODO : ln

//    sh.register_command(
//    {"rm", "removes a file",
//     "Usage : rm <name>",
//     [&sh](const std::vector<std::string>& args)
//     {
//         if (args.size() != 1)
//         {
//             sh.error("rm needs one argument !\n");
//             return -1;
//         }

//         auto node = vfs::find(sh.get_path(args[0])).value_or(nullptr);
//         if (!node)
//         {
//             sh.error("Can't find '%s' !\n", args[0].c_str());
//             return -2;
//         }

//         if (!sh.pwd->remove(node.get()))
//         {
//             sh.error("Can't remove '%s' !\n", args[0].c_str());
//             return -3;
//         }

//         return 0;
//     }});

//    sh.register_command(
//    {"mount", "mounts a file system",
//     "Usage : mount <disk> <target>",
//     [&sh](const std::vector<std::string>& args)
//     {
//         if (args.size() != 2)
//         {
//             sh.error("mount needs two arguments !\n");
//             return -1;
//         }

//         auto vfs_disk_node = vfs::find(sh.get_path(args[0])).value_or(nullptr);
//         if (!vfs_disk_node)
//         {
//             sh.error("Can't find disk file '%s' !\n", args[0].c_str());
//             return -2;
//         }
//         auto disk_node = dynamic_cast<devfs::disk_file*>(vfs_disk_node.get());
//         if (!disk_node)
//         {
//             sh.error("'%s' is not a disk file\n", args[0].c_str());
//             return -3;
//         }

//         auto target_node = vfs::find(sh.get_path(args[1])).value_or(nullptr);
//         if (!target_node)
//         {
//             sh.error("Can't find mount point '%s' !\n", args[1].c_str());
//             return -4;
//         }

//         auto fs = FileSystem::get_disk_fs(disk_node->disk());
//         if (!fs)
//         {
//             sh.error("'%s' doesn't contain a valid file system\n", args[0].c_str());
//             return -5;
//         }

//         if (!vfs::mount(fs->root(), target_node))
//         {
//             sh.error("Can't mount '%s' on '%s'\n", args[0].c_str(), args[1].c_str());
//             return -6;
//         }

//         return 0;
//     }});

//    sh.register_command(
//    {"umount", "unmounts a file system",
//     "Usage : umount <target>",
//     [&sh](const std::vector<std::string>& args)
//     {
//         if (args.size() != 1)
//         {
//             sh.error("umount needs one argument !\n");
//             return -1;
//         }

//         auto target_node = vfs::find(sh.get_path(args[0])).value_or(nullptr);
//         if (!target_node)
//         {
//             sh.error("Can't find mount point '%s' !\n", args[0].c_str());
//             return -2;
//         }

//         if (!vfs::umount(target_node))
//         {
//             sh.error("Can't unmount '%s' !\n", args[0].c_str());
//             return -3;
//         }

//         return 0;
//     }});

//    sh.register_command(
//    {"stat", "shows file info",
//     "Usage : stat <file>",
//     [&sh](const std::vector<std::string>& args)
//     {
//         if (args.size() != 1)
//         {
//             sh.error("stat needs one argument !\n");
//             return -1;
//         }

//         auto target_node = vfs::find(sh.get_path(args[0])).value_or(nullptr);
//         if (!target_node)
//         {
//             sh.error("Can't find '%s' !\n", args[0].c_str());
//             return -2;
//         }

//         auto stat = target_node->stat();

//         std::string perm_str(9, '-');
//         if (stat.perms & vfs::Permissions::UserRead) perm_str[0] = 'r';
//         if (stat.perms & vfs::Permissions::UserWrite) perm_str[1] = 'w';
//         if (stat.perms & vfs::Permissions::UserExec) perm_str[2] = 'x';
//         if (stat.perms & vfs::Permissions::GroupRead) perm_str[3] = 'r';
//         if (stat.perms & vfs::Permissions::GroupWrite) perm_str[4] = 'w';
//         if (stat.perms & vfs::Permissions::GroupExec) perm_str[5] = 'x';
//         if (stat.perms & vfs::Permissions::OtherRead) perm_str[6] = 'r';
//         if (stat.perms & vfs::Permissions::OtherWrite) perm_str[7] = 'w';
//         if (stat.perms & vfs::Permissions::OtherExec) perm_str[8] = 'x';

//         printf("Permissions : %s\n", perm_str.c_str());
//         printf("uid : %d    gid : %d\n", stat.uid, stat.gid);
//         printf("Flags : 0x%x\n", stat.flags);
//         printf("Creation time : %s\n", stat.creation_time ? Time::to_string(Time::to_local_time(Time::from_unix(stat.creation_time))) : "-");
//         printf("Access time : %s\n", stat.access_time ? Time::to_string(Time::to_local_time(Time::from_unix(stat.access_time))) : "-");
//         printf("Modification time : %s\n", stat.modification_time ? Time::to_string(Time::to_local_time(Time::from_unix(stat.modification_time))) : "-");

//         return 0;
//     }});
}

