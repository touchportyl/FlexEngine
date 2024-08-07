#pragma once

#include <chrono>

namespace FlexEngine
{

  class FramerateController
  {
    std::chrono::time_point<std::chrono::high_resolution_clock> m_last_time = std::chrono::high_resolution_clock::now();
    float m_delta_time = 0.0f;
    float m_frame_time_accumulator = 0.0f;
    unsigned int m_fps = 0;
    unsigned int m_frame_counter = 0;
    unsigned int m_target_fps = 60;

  public:
    void BeginFrame();
    void EndFrame();

    #pragma region Getter/Setter Functions

    float GetDeltaTime() const;
    unsigned int GetFPS() const;
    void SetTargetFPS(unsigned int fps = 0);

    #pragma endregion
  };

}