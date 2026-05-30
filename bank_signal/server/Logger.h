#pragma once

#include <string>
#include <deque>
#include <sstream>
#include <mutex>

class LogStorage {
public:
    void push(std::string message)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        mMessages.push_back(std::move(message));
    }

    std::deque<std::string> snapshot() const
    {
        std::lock_guard<std::mutex> lock(mMutex);
        return mMessages;
    }

private:
    mutable std::mutex mMutex;
    std::deque<std::string> mMessages;
};

class LogLine {
public:
    explicit LogLine(LogStorage& storage)
        : mStorage(storage)
    {
    }

    ~LogLine()
    {
        mStorage.push(mStream.str());
    }

    template<typename T>
    LogLine& operator<<(const T& value)
    {
        mStream << value;
        return *this;
    }

private:
    LogStorage& mStorage;
    std::ostringstream mStream;
};
class Logger {
public:
    LogLine operator()()
    {
        return LogLine(mStorage);
    }

    std::deque<std::string> messages() const
    {
        return mStorage.snapshot();
    }

private:
    LogStorage mStorage;
};