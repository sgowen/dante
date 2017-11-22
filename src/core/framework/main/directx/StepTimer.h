//
// StepTimer.h - A simple timer that provides elapsed time information
//

#pragma once

#include <exception>
#include <stdint.h>

namespace DX
{
    // Helper class for animation and simulation timing.
    class StepTimer
    {
    public:
        StepTimer() : _elapsedTicks(0), _frameCount(0)
        {
            if (!QueryPerformanceFrequency(&_qpcFrequency))
            {
                throw std::exception( "QueryPerformanceFrequency" );
            }
            
            if (!QueryPerformanceCounter(&_qpcLastTime))
            {
                throw std::exception( "QueryPerformanceCounter" );
            }
            
            // Initialize max delta to 1/10 of a second.
            _qpcMaxDelta = _qpcFrequency.QuadPart / 10;
        }
        
        // Get elapsed time since the previous Update call.
        double GetElapsedSeconds() const
        {
            return TicksToSeconds(_elapsedTicks);
        }
        
        // Get total number of updates since start of the program.
        uint32_t GetFrameCount() const
        {
            return _frameCount;
        }
        
        // Integer format represents time using 10,000,000 ticks per second.
        static const uint64_t TicksPerSecond = 10000000;
        
        // After an intentional timing discontinuity (for instance a blocking IO operation)
        // call this to avoid having the fixed timestep logic attempt a set of catch-up
        // Update calls.
        
        void ResetElapsedTime()
        {
            if (!QueryPerformanceCounter(&_qpcLastTime))
            {
                throw std::exception("QueryPerformanceCounter");
            }
        }
        
        // Update timer state, calling the specified Update function the appropriate number of times.
        template<typename TUpdate>
        void Tick(const TUpdate& update)
        {
            // Query the current time.
            LARGE_INTEGER currentTime;
            
            if (!QueryPerformanceCounter(&currentTime))
            {
                throw std::exception( "QueryPerformanceCounter" );
            }
            
            uint64_t timeDelta = currentTime.QuadPart - _qpcLastTime.QuadPart;
            
            _qpcLastTime = currentTime;
            
            // Clamp excessively large time deltas (e.g. after paused in the debugger).
            if (timeDelta > _qpcMaxDelta)
            {
                timeDelta = _qpcMaxDelta;
            }
            
            // Convert QPC units into a canonical tick format. This cannot overflow due to the previous clamp.
            timeDelta *= TicksPerSecond;
            timeDelta /= _qpcFrequency.QuadPart;
            
            uint32_t lastFrameCount = _frameCount;
            
            // Variable timestep update logic.
            _elapsedTicks = timeDelta;
            _frameCount++;
            
            update();
        }
        
    private:
        // Source timing data uses QPC units.
        LARGE_INTEGER _qpcFrequency;
        LARGE_INTEGER _qpcLastTime;
        uint64_t _qpcMaxDelta;
        
        // Derived timing data uses a canonical tick format.
        uint64_t _elapsedTicks;
        
        // Members for tracking the framerate.
        uint32_t _frameCount;
        
        static double TicksToSeconds(uint64_t ticks)
        {
            return static_cast<double>(ticks) / TicksPerSecond;
        }
        
        static uint64_t SecondsToTicks(double seconds)
        {
            return static_cast<uint64_t>(seconds * TicksPerSecond);
        }
    };
}
