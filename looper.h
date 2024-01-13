#pragma once

#include "Arduino.h";

namespace synthux {

class Looper {
  public:
    void Init(float *buf, size_t length) {
      _buffer = buf;
      _buffer_length = length;
      // Reset buffer contents to zero
      memset(_buffer, 0, sizeof(float) * _buffer_length);
    }
  

    float Process(float in) {
      _buffer[_rec_head] = in;
      _rec_head++;
      _rec_head %= _buffer_length;

      // if (_is_empty) {
      //   _is_empty = false;
      //   return 0;
      // }
      // Playback from the buffer

      _play_head = (_rec_head - _delaySamples) % _buffer_length;
      float output = _buffer[_play_head] + in;
      return output;
    

    }

  private:
    static const size_t kFadeLength = 600;
    static const size_t kMinLoopLength = 2 * kFadeLength;

    float* _buffer;

    size_t _buffer_length = 0;

    size_t _play_head = 0;
    size_t _rec_head  = 0;

    static const uint32_t _delaySec = 6;
    static const uint32_t _kSampleRate = 48000;
    static const size_t _delaySamples = _delaySec * _kSampleRate;

    bool _is_empty  = true;
};
};