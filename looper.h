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
      // Setup the playheads
      for(int i = 0; i < _head_count; i++){
        // _delaySamples[i] = (i + 1) * _buffer_length / (_head_count + 1); 
        _play_head[i] = _rec_head + (i + 1) * _buffer_length / (_head_count + 1); 
      }
    }
  

    float Process(float in) {
      // Record to the buffer
      _buffer[_rec_head] = in;
      _rec_head++;
      _rec_head %= _buffer_length;

      // Playback from the buffer
      // float _output[_head_count] = {0};
      _output = 0;
      for(int i = 0; i < _head_count; i++){
        // _play_head[i] = (_rec_head - _delaySamples[i]) % _buffer_length;
        _output += _buffer[_play_head[i]];
        _play_head[i]++;
        _play_head[i] %= _buffer_length;
        // _output[i] = _buffer[_play_head[i]] + in;}
        }
      // _output = _output * sqrt(_head_count) + in;
      _output = _output * sqrt(_head_count);
      return _output;

    }

  private:
    static const size_t kFadeLength = 600;
    static const size_t kMinLoopLength = 2 * kFadeLength;

    float* _buffer;

    size_t _buffer_length = 0;

    

    static const int _head_count = 4;
    size_t _play_head[_head_count] = {0}; 
    size_t _rec_head  = 0;

    float _output = 0;
    float _mix = sqrt(_head_count);
    // static const uint32_t _delaySec[_head_count] = {0};
    
    // static const uint32_t _kSampleRate = 48000;
    // size_t _delaySamples[_head_count] = {0};

    bool _is_empty  = true;
};
};