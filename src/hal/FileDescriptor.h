#pragma once

#include <fcntl.h>
#include <unistd.h>

class FileDescriptor
{
private:
    int m_fd{-1};

public:
    FileDescriptor() = default;

    explicit FileDescriptor(const char* path, int flags)
        : m_fd{::open(path, flags)}
    {
    }

    ~FileDescriptor()
    {
        if (m_fd >= 0)
        {
            ::close(m_fd);
            m_fd = -1;
        }
    }

    FileDescriptor(const FileDescriptor&) = delete;
    FileDescriptor& operator=(const FileDescriptor&) = delete;

    FileDescriptor(FileDescriptor&& other) noexcept
        : m_fd{other.m_fd}
    {
        other.m_fd = -1;
    }

    FileDescriptor& operator=(FileDescriptor&& other) noexcept
    {
        if (this != &other)
        {
            if (m_fd >= 0)
                ::close(m_fd);
            m_fd = other.m_fd;
            other.m_fd = -1;
        }
        return *this;
    }

    [[nodiscard]] bool isValid() const noexcept
    {
        return m_fd >= 0;
    }
    [[nodiscard]] explicit operator bool() const noexcept
    {
        return isValid();
    }
    [[nodiscard]] int get() const noexcept
    {
        return m_fd;
    }

    ssize_t read(void* buf, size_t count) const
    {
        return ::read(m_fd, buf, count);
    }

    ssize_t write(const void* buf, size_t count) const
    {
        return ::write(m_fd, buf, count);
    }
};