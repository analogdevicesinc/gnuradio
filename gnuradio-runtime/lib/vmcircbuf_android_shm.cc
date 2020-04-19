/* -*- c++ -*- */
/*
 * Copyright 2003,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "vmcircbuf_android_shm.h"
#include <gnuradio/logger.h>
#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdexcept>
#include "pagesize.h"
#include <errno.h>
#include <stdio.h>
#include <android/sharedmem.h>
#include <android/sharedmem_jni.h>
#include <sys/mman.h>

#define MAX_SYSV_SHM_ATTEMPTS 3

namespace gr {

vmcircbuf_android_shm::vmcircbuf_android_shm(int size) : gr::vmcircbuf(size)
{
    gr::thread::scoped_lock guard(s_vm_mutex);

    int pagesize = gr::pagesize();

    if (size <= 0 || (size % pagesize) != 0) {
        GR_DEBUG("android_shm", "invalid size requested");
        throw std::runtime_error("gr::vmcircbuf_android_shm");
    }

    // Attempt to allocate buffers (handle bad_alloc errors)
    int attempts_remain(MAX_SYSV_SHM_ATTEMPTS);
    while (attempts_remain-- > 0) {

        d_shmem_guard = ASharedMemory_create(0, pagesize);
        if(d_shmem_guard < 0) {
            GR_DEBUG("android_shm", "cannot memory segment guard, error " << d_shmem_guard);
            continue;
        }

        d_shmem_buf = ASharedMemory_create(0, size);
        if(d_shmem_buf < 0) {
            close(d_shmem_guard);
            GR_DEBUG("android_shm", "cannot create memory segment buf, error " << d_shmem_buf);
            continue;
        }

        int shmem_probe = ASharedMemory_create(0, 2*size + 2*pagesize);
        if(shmem_probe < 0) {
            close(d_shmem_guard);
            close(d_shmem_buf);
            GR_DEBUG("android_shm", "cannot create memory segment probe, error " << shmem_probe);
            continue;
        }

        char* probe = (char*)mmap(NULL, 2*size + 2*pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, shmem_probe, 0);
        if(probe == MAP_FAILED) {
            close(d_shmem_guard);
            close(d_shmem_buf);
            close(shmem_probe);
            GR_DEBUG("android_shm", "cannot mmap memory segment probe, error " << probe);
            continue;
        }
        munmap(probe, 2*size + 2*pagesize);

        char* buf = (char*)mmap(probe, pagesize, PROT_READ, MAP_SHARED, d_shmem_guard, 0);

        close(shmem_probe);

        if(buf != probe) {
            GR_LOG_DEBUG("android_shm", "pre-guard at wrong location");
            munmap(buf, pagesize);
            close(d_shmem_buf);
            close(d_shmem_guard);
            continue;
        }

        buf = (char*)mmap(probe+pagesize, size, PROT_READ | PROT_WRITE, MAP_SHARED, d_shmem_buf, 0);

        if(buf != probe + pagesize) {
            GR_LOG_DEBUG("android_shm", "first buf at wrong location");
            munmap(probe, pagesize);
            munmap(buf, size);
            close(d_shmem_buf);
            close(d_shmem_guard);
            continue;
        }

        buf = (char*)mmap(probe+pagesize+size, size, PROT_READ | PROT_WRITE, MAP_SHARED, d_shmem_buf, 0);

        if(buf != probe + pagesize + size) {
            GR_LOG_DEBUG("android_shm", "second buf at wrong location");
            munmap(probe, pagesize);
            munmap(probe+pagesize, size);
            munmap(buf, size);
            close(d_shmem_buf);
            close(d_shmem_guard);
            continue;
        }


        buf = (char*)mmap(probe+pagesize+2*size, pagesize, PROT_READ, MAP_SHARED, d_shmem_guard, 0);

        if(buf != probe + pagesize + 2*size) {
            GR_LOG_DEBUG("android_shm", "post-guard at wrong location ");
            munmap(probe, pagesize);
            munmap(probe+pagesize, size);
            munmap(probe+2*pagesize, size);
            munmap(buf, pagesize);
            close(d_shmem_buf);
            close(d_shmem_guard);
            continue;
        }

        d_base = probe + pagesize;

        break;
    }


    if (attempts_remain < 0) {
        throw std::runtime_error("gr::vmcircbuf_android_shm");
    }

    // GR_DEBUG("android_shm", "successfully created buffer");
}

vmcircbuf_android_shm::~vmcircbuf_android_shm()
{
    gr::thread::scoped_lock guard(s_vm_mutex);
    int pagesize = gr::pagesize();
    munmap(d_base-pagesize, pagesize);
    munmap(d_base, d_size);
    munmap(d_base+d_size, d_size);
    munmap(d_base+2*d_size, pagesize);
    close(d_shmem_guard);
    close(d_shmem_buf);
}

// ----------------------------------------------------------------
//			The factory interface
// ----------------------------------------------------------------

gr::vmcircbuf_factory* vmcircbuf_android_shm_factory::s_the_factory = 0;

gr::vmcircbuf_factory* vmcircbuf_android_shm_factory::singleton()
{
    if (s_the_factory)
        return s_the_factory;

    s_the_factory = new gr::vmcircbuf_android_shm_factory();
    return s_the_factory;
}

int vmcircbuf_android_shm_factory::granularity() { return gr::pagesize(); }

gr::vmcircbuf* vmcircbuf_android_shm_factory::make(int size)
{
    try {
        return new vmcircbuf_android_shm(size);
    } catch (...) {
        GR_DEBUG("android_shm", "exception while creating circ buffer");
        return 0;
    }
}

} /* namespace gr */
