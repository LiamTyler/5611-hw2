#ifndef SRC_INCLUDE_FPS_COUNTER_H_
#define SRC_INCLUDE_FPS_COUNTER_H_

#include <functional>

class FPSCounter {
    public:
        FPSCounter();
        ~FPSCounter();
        void Init();
        void StartFrame(float dt);
        void EndFrame(void* data);
        float GetDT() { return time_ - prevTime_; }
        void CallBack(std::function<void(void*)> func) { callback_ = func; }

    protected:
        std::function<void(void*)> callback_;
        float time_;
        float prevTime_;
        float fpsTime_;
        unsigned int frameCounter_;
};

#endif  // SRC_INCLUDE_FPS_COUNTER_H_
