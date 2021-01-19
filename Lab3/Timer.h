#ifndef LAB3_TIMER_H
#define LAB3_TIMER_H

#include <chrono>

class Timer {
public:
    Timer() {
        run();
    }

    void run() {
        start = std::chrono::high_resolution_clock::now();
    }

    void stop() {
        std::chrono::time_point<std::chrono::high_resolution_clock> finish = std::chrono::high_resolution_clock::now();
        lastElapsedNanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();
    }

    [[nodiscard]] uint64_t elapsedNanoseconds() const {
        return lastElapsedNanoseconds;
    }

    [[nodiscard]] uint64_t elapsedMicroseconds() const {
        return lastElapsedNanoseconds * 0.001;
    }

    [[nodiscard]] uint64_t elapsedMilliseconds() const {
        return lastElapsedNanoseconds * 0.000001;
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    uint64_t lastElapsedNanoseconds = 0;
};

#endif //LAB3_TIMER_H
