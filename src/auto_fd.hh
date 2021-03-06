/**
 * Copyright (c) 2007-2012, Timothy Stack
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * * Neither the name of Timothy Stack nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @file auto_fd.hh
 */

#ifndef __auto_fd_hh
#define __auto_fd_hh

#include <assert.h>
#include <unistd.h>
#include <sys/select.h>

#include <new>
#include <exception>

/**
 * Resource management class for file descriptors.
 *
 * @see auto_ptr
 */
class auto_fd {
public:

    /**
     * Wrapper for the posix pipe(2) function that stores the file descriptor
     * results in an auto_fd array.
     *
     * @param af An array of atleast two auto_fd elements, where the first
     * contains the reader end of the pipe and the second contains the writer.
     * @return The result of the pipe(2) function.
     */
    static int pipe(auto_fd *af)
    {
        int retval, fd[2];

        assert(fd != NULL);

        if ((retval = ::pipe(fd)) == 0) {
            af[0] = fd[0];
            af[1] = fd[1];
        }

        return retval;
    };

    /**
     * Construct an auto_fd to manage the given file descriptor.
     *
     * @param fd The file descriptor to be managed.
     */
    auto_fd(int fd = -1)
        : af_fd(fd)
    {
        assert(fd >= -1);
        assert(fd < (int)FD_SETSIZE);
    };

    /**
     * Non-const copy constructor.  Management of the file descriptor will be
     * transferred from the source to this object and the source will be
     * cleared.
     *
     * @param af The source of the file descriptor.
     */
    auto_fd(auto_fd & af)
        : af_fd(af.release()) { };

    /**
     * Const copy constructor.  The file descriptor from the source will be
     * dup(2)'d and the new descriptor stored in this object.
     *
     * @param af The source of the file descriptor.
     */
    auto_fd(const auto_fd &af)
        : af_fd(-1)
    {
        if (af.af_fd != -1 && (this->af_fd = dup(af.af_fd)) == -1) {
            throw std::bad_alloc();
        }
    };

    /**
     * Destructor that will close the file descriptor managed by this object.
     */
    ~auto_fd()
    {
        this->reset();
    };

    /** @return The file descriptor as a pain integer. */
    operator int(void) const { return this->af_fd;  };

    /**
     * Replace the current descriptor with the given one.  The current
     * descriptor will be closed.
     *
     * @param fd The file descriptor to store in this object.
     * @return *this
     */
    auto_fd &operator =(int fd)
    {
        assert(fd >= -1);
        assert(fd < (int)FD_SETSIZE);

        this->reset(fd);
        return *this;
    };

    /**
     * Transfer management of the given file descriptor to this object.
     *
     * @param af The old manager of the file descriptor.
     * @return *this
     */
    auto_fd &operator =(auto_fd & af)
    {
        this->reset(af.release());
        return *this;
    };

    /**
     * Return a pointer that can be passed to functions that require an out
     * parameter for file descriptors (e.g. openpty).
     *
     * @return A pointer to the internal integer.
     */
    int *out(void)
    {
        this->reset();
        return &this->af_fd;
    };

    /**
     * Stop managing the file descriptor in this object and return its value.
     *
     * @return The file descriptor.
     */
    int release(void)
    {
        int retval = this->af_fd;

        this->af_fd = -1;
        return retval;
    };

    /**
     * @return The file descriptor.
     */
    int get(void) const
    {
        return this->af_fd;
    };

    /**
     * Closes the current file descriptor and replaces its value with the given
     * one.
     *
     * @param fd The new file descriptor to be managed.
     */
    void reset(int fd = -1)
    {
        assert(fd >= -1);
        assert(fd < (int)FD_SETSIZE);

        if (this->af_fd != fd) {
            if (this->af_fd != -1) {
                close(this->af_fd);
            }
            this->af_fd = fd;
        }
    };

private:
    int af_fd;  /*< The managed file descriptor. */
};
#endif
