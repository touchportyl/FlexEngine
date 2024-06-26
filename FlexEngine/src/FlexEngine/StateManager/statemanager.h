﻿#pragma once

#include "flx_api.h"

#include "state.h"

#include <memory>

// Uses macros to easily create StateManager classes.
// These macros can be used in other files as well.
#pragma region Macros

#pragma region Declaration
// Creates a StateManager class with the given name.
// Usage: FLX_STATEMANAGER_REGISTER_DECL(Application);
// This will create a class called ApplicationStateManager.
// Remember to run the FLX_STATEMANAGER_REGISTER_IMPL macro in a cpp file.
#define FLX_STATEMANAGER_REGISTER_DECL(NAME)                                     \
  class __FLX_API NAME##StateManager                                             \
  {                                                                              \
    static std::unique_ptr<State> m_state_current;                               \
                                                                                 \
  public:                                                                        \
    /* Usage example : NAME##StateManager::SetState<GameState>(); */             \
    template <typename T>                                                        \
    static void SetState()                                                       \
    {                                                                            \
      SetState(std::make_unique<T>());                                           \
    }                                                                            \
                                                                                 \
    /* For best practice, use the SetState template method instead               \
     * Usage: NAME##StateManager::SetState(std::make_unique<GameState>());       \
     */                                                                          \
    static void SetState(std::unique_ptr<State> state);                          \
                                                                                 \
    static void Restart();                                                       \
    static void Update();                                                        \
    static void Exit();                                                          \
  }
#pragma endregion

#pragma region Implementation
// Implementation of the StateManager class.
// Usage: FLX_STATEMANAGER_REGISTER_IMPL(Application);
// This will create the implementation of the ApplicationStateManager class.
// This implementation keeps everything in scope, do not rely on restart to fully reset the state.
// That is the responsibility of the state itself
#define FLX_STATEMANAGER_REGISTER_IMPL(NAME)                            \
  void NAME##StateManager::SetState(std::unique_ptr<State> state)       \
  {                                                                     \
    /* no state */                                                      \
    if (!state) return;                                                 \
                                                                        \
    /* same state */                                                    \
    if (state == m_state_current) return;                               \
                                                                        \
    /* set the next state */                                            \
    Exit();                                                             \
    m_state_current.swap(state);                                        \
    m_state_current->OnEnter();                                         \
  }                                                                     \
                                                                        \
  void NAME##StateManager::Update()                                     \
  {                                                                     \
    /* no state */                                                      \
    if (!m_state_current) return;                                       \
                                                                        \
    m_state_current->Update();                                          \
  }                                                                     \
                                                                        \
  void NAME##StateManager::Restart()                                    \
  {                                                                     \
    /* no state */                                                      \
    if (!m_state_current) return;                                       \
                                                                        \
    /* EXTREMELY IMPORTANT NOTE!                                        \
     * this implementation keeps everything in scope                    \
     * do not rely on restart to fully reset the state                  \
     * that is the responsibility of the state itself                   \
     */                                                                 \
    m_state_current->OnExit();                                          \
    m_state_current->OnEnter();                                         \
  }                                                                     \
                                                                        \
  void NAME##StateManager::Exit()                                       \
  {                                                                     \
    if (m_state_current) m_state_current->OnExit();                     \
    m_state_current.reset();                                            \
  }                                                                     \
  /* static member initialization */                                    \
  std::unique_ptr<State> NAME##StateManager::m_state_current = nullptr

#pragma endregion

#pragma endregion

namespace FlexEngine
{

  FLX_STATEMANAGER_REGISTER_DECL(Application);

}
