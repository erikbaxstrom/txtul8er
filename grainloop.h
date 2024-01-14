#pragma once

class GrainLoop {
    public:
        void Init(uint8_t *buf, size_t length, uint32_t interval){
            _buffer = buf;
            _buffer_length = length;
            _interval = interval;
            memset(_buffer, 0, sizeof(uint8_t) * _buffer_length);
        }

        void Process() {
            //do something
        }

    private:
        float* _buffer;
        size_t _buffer_length = 0;
        unsigned long gCurrentMillis;
        unsigned long gPreviousMillis = 0;
        uint32_t _interval
}