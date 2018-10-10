/*
tar.cpp

Copyright (c) 05 Yann BOUCHER (yann)

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

#include "tar.hpp"

#include <stdlib.h>

#include "fs/fsutils.hpp"
#include "fs/vfs.hpp"

#include "fs/pathutils.hpp"
#include "utils/memutils.hpp"

#define TMAGIC   "ustar"        /* ustar and a null */
#define TMAGLEN  6
#define TVERSION "00"           /* 00 and no null */
#define TVERSLEN 2

/* Values used in typeflag field.  */
#define REGTYPE  '0'            /* regular file */
#define AREGTYPE '\0'           /* regular file */
#define LNKTYPE  '1'            /* link */
#define SYMTYPE  '2'            /* reserved */
#define CHRTYPE  '3'            /* character special */
#define BLKTYPE  '4'            /* block special */
#define DIRTYPE  '5'            /* directory */
#define FIFOTYPE '6'            /* FIFO special */
#define CONTTYPE '7'            /* reserved */

#define XHDTYPE  'x'            /* Extended header referring to the
    next file in the archive */
#define XGLTYPE  'g'            /* Global extended header */

/* Bits used in the mode field, values in octal.  */
#define TSUID    04000          /* set UID on execution */
#define TSGID    02000          /* set GID on execution */
#define TSVTX    01000          /* reserved */
/* file permissions */
#define TUREAD   00400          /* read by owner */
#define TUWRITE  00200          /* write by owner */
#define TUEXEC   00100          /* execute/search by owner */
#define TGREAD   00040          /* read by group */
#define TGWRITE  00020          /* write by group */
#define TGEXEC   00010          /* execute/search by group */
#define TOREAD   00004          /* read by other */
#define TOWRITE  00002          /* write by other */
#define TOEXEC   00001          /* execute/search by other */

namespace tar
{

TarFS::TarFS(Disk &disk)
    : FSImpl<tar::TarFS>(disk)
{
    auto result = disk.read();
    if (!result)
    {
        m_root_dir = nullptr;
        err("Cannot load tar fs on disk %s : %s\n", disk.drive_name().c_str(), result.error().to_string());
    }
    else
    {
        m_file = std::move(result.value());

        m_root_dir = std::make_shared<tar_node>(*this, nullptr);
        m_root_dir->m_type = vfs::node::Directory;
        m_root_dir->m_name = "";
        m_root_dir->m_data_addr = m_file.data() + sizeof(Header);
        m_root_dir->m_size = m_file.size();

        auto nodes = list_nodes();
        prune_directories_names(nodes);
        attach_parents(nodes);
    }
}

bool TarFS::accept(const Disk &disk)
{
    auto result = disk.read(0, sizeof(Header));
    if (!result) return false;

    Header hdr = *(Header*)result->data();

    return strncmp(hdr.magic, TMAGIC, 5) == 0;
}

size_t TarFS::total_size() const
{
    return root()->size();
}

std::shared_ptr<tar_node> TarFS::read_header(const Header *hdr) const
{
    if (hdr->name[0] == '\0')
    {
        return nullptr;
    }
    if (!check_sum(hdr))
    {
        warn("Bad TAR chksum\n");
        return nullptr;
    }

    auto node = std::make_shared<tar_node>(*this, m_root_dir.get());

    switch (hdr->typeflag)
    {
        case REGTYPE:
        case AREGTYPE :
            node->m_type = vfs::node::File;
            break;
        case DIRTYPE:
            node->m_type = vfs::node::Directory;
            break;
        case LNKTYPE:
        case SYMTYPE:
            node->m_type = vfs::node::SymLink;
            node->m_link_target = kpp::string(hdr->linkname, 101);
            node->m_link_target.back() = '\0';
            node->m_link_target = trim_zstr(node->m_link_target);
            break;
        case CHRTYPE:
        case BLKTYPE:
        case FIFOTYPE:
        case CONTTYPE:
            return nullptr;
    }

    node->m_data_addr = reinterpret_cast<const uint8_t*>(hdr) + sizeof(Header);
    node->m_size = read_number(hdr->size);
    node->m_stat.perms = read_number(hdr->mode);
    node->m_stat.uid = read_number(hdr->uid);
    node->m_stat.gid = read_number(hdr->gid);
    node->m_stat.creation_time = node->m_stat.modification_time = read_number(hdr->mtime);
    node->m_stat.access_time = 0;
    node->m_name = kpp::string(hdr->name, 101); node->m_name.back() = '\0';
    node->m_name = trim_zstr(node->m_name);

    return node;
}

std::vector<std::shared_ptr<tar_node> > TarFS::read_dir(const uint8_t *addr, size_t size) const
{
    std::vector<std::shared_ptr<tar_node>> nodes;

    const uint8_t* ptr = addr;

    while (ptr < addr + size)
    {
        const Header* hdr = reinterpret_cast<const Header*>(ptr);
        auto node = read_header(hdr);

        if (node)
        {
            nodes.emplace_back(node);
        }
        else
        {
            return nodes;
        }

        size_t jump = read_number(hdr->size);
        if (jump % sizeof(Header))
        {
            jump += sizeof(Header) - (jump % sizeof(Header));
        }
        ptr += sizeof(Header) + jump;

        // Handle end-of-file 512 bytes blocks
        if (!read_header(reinterpret_cast<const Header*>(ptr)))
        {
            ptr += sizeof(Header);
        }
    }

    return nodes;
}

bool TarFS::check_sum(const TarFS::Header *hdr) const
{
    size_t sum { 0 };
    Header hdr_copy = *hdr;
    strncpy(hdr_copy.chksum, "        ", 8);

    for (size_t i { 0 }; i < sizeof(Header); ++i)
    {
        sum += reinterpret_cast<const uint8_t*>(&hdr_copy)[i];
    }
    return read_number(hdr->chksum) == sum;
}

std::vector<std::shared_ptr<tar_node> > TarFS::list_nodes()
{
    return read_dir(m_file.data() + sizeof(Header), m_file.size());
}

void TarFS::prune_directories_names(std::vector<std::shared_ptr<tar_node>> dirs)
{
    for (const auto& node : dirs)
    {
        if (node->m_type == vfs::node::Directory)
        {
            auto list = path_list(node->m_name);
            list.erase(list.begin()); // remove first directory which is archive name

            node->m_name = join(list, "/");
        }
    }
}

void TarFS::attach_parents(std::vector<std::shared_ptr<tar_node>> nodes)
{
    for (auto node : nodes)
    {
        auto list = path_list(parent_path(node->m_name));
        if (!list.empty())
        {
            auto parent_name = list.back();

            auto parent = std::find_if(nodes.begin(), nodes.end(), [&parent_name](std::shared_ptr<tar_node> node)
            {
                    return node->m_type == vfs::node::Directory && path_list(node->m_name).back() == parent_name;
        });

            if (parent != nodes.end())
            {
                node->m_parent = parent->get();
                (*parent)->m_children.emplace_back(node);
            }
            else
            {
                node->m_parent = m_root_dir.get();
                m_root_dir->m_children.emplace_back(node);

                if (node->m_type == vfs::node::Directory)
                {
                    warn("Tar Directory '%s' has no parent\n", node->name().c_str());
                }
            }
        }
        else
        {
            node->m_parent = m_root_dir.get();
            m_root_dir->m_children.emplace_back(node);
        }
    }
}

[[nodiscard]] kpp::expected<size_t, vfs::FSError> tar_node::read_impl(size_t offset, gsl::span<uint8_t> data) const
{
    if (!m_link_target.empty())
    {
        auto result = vfs::find(m_parent->path() + m_link_target);
        if (!result) return kpp::make_unexpected(vfs::FSError{vfs::FSError::InvalidLink});
        return result.value()->read(offset, data);
    }

    size_t amnt = std::min<long>(data.size(), this->size());
    std::copy(m_data_addr + offset, m_data_addr + offset + amnt, data.begin());
    return amnt;
}

std::vector<std::shared_ptr<vfs::node> > tar_node::readdir_impl()
{
    if (!m_link_target.empty())
    {
        auto result = vfs::find(m_parent->path() + m_link_target);
        if (!result) return {}; // TODO
        return result.value()->readdir();
    }

    std::vector<std::shared_ptr<vfs::node> > vec;
    for (auto tar_node : m_children) vec.emplace_back(tar_node);

    return vec;
}

size_t tar_node::size() const
{
    if (!m_link_target.empty())
    {
        auto result = vfs::find(m_parent->path() + m_link_target);
        if (!result) return 0; // TODO
        return result.value()->size();
    }

    return m_size;
}

vfs::node::Type tar_node::type() const
{
    if (!m_link_target.empty())
    {
        auto result = vfs::find(m_parent->path() + m_link_target);
        if (!result) return vfs::node::Unknown; // TODO
        return result.value()->type();
    }

    return m_type;
}

bool tar_node::is_link() const
{
    return !m_link_target.empty();
}

kpp::string tar_node::name() const
{
    if (tokenize(m_name, "/").empty())
    {
        // TODO : investigate this ! !?
        return "";
    }
    return filename(m_name).to_string();
}

ADD_FS(TarFS)

}
