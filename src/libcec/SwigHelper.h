#pragma once
/*
 * This file is part of the libCEC(R) library.
 *
 * libCEC(R) is Copyright (C) 2011-2015 Pulse-Eight Limited.  All rights reserved.
 * libCEC(R) is an original work, containing original code.
 *
 * libCEC(R) is a trademark of Pulse-Eight Limited.
 *
 * This program is dual-licensed; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 *
 *
 * Alternatively, you can license this library under a commercial license,
 * please contact Pulse-Eight Licensing for more information.
 *
 * For more information contact:
 * Pulse-Eight Licensing       <license@pulse-eight.com>
 *     http://www.pulse-eight.com/
 *     http://www.pulse-eight.net/
 */

#define SWIG_FILE_WITH_INIT
#define LIBCEC_SWIG_EXPORTS

/** XXX python does a #define PLATFORM, which causes a collision with our PLATFORM namespace */
#ifdef PLATFORM
#define _platform_tmp PLATFORM
#undef PLATFORM
#endif

#include "cectypes.h"
#include "cec.h"
#include "CECTypeUtils.h"
#include "platform/threads/mutex.h"

/** XXX python does a #define PLATFORM, which causes a collision with our PLATFORM namespace */
#ifdef _platform_tmp
#define PLATFORM _platform_tmp
#undef _platform_tmp
#endif

namespace CEC
{
  enum libcecSwigCallback {
    PYTHON_CB_LOG_MESSAGE,
    PYTHON_CB_KEY_PRESS,
    PYTHON_CB_COMMAND,
    PYTHON_CB_ALERT,
    PYTHON_CB_MENU_STATE,
    PYTHON_CB_SOURCE_ACTIVATED,
    NB_PYTHON_CB,
  };

  class CCecPythonCallbacks
  {
  public:
    /**
     * Create a new python callbacks instance, and set callbacks in the configuration
     * @param config    the configuration to set the callbacks in
     */
    CCecPythonCallbacks(libcec_configuration* config) :
      m_configuration(config)
    {
      assert(m_configuration);

      config->callbacks = new ICECCallbacks;
      if (!config->callbacks)
        throw std::bad_alloc();

      for (size_t ptr = 0; ptr < NB_PYTHON_CB; ++ptr)
        m_callbacks[ptr] = NULL;

      m_configuration->callbacks->CBCecLogMessage       = CBCecLogMessage;
      m_configuration->callbacks->CBCecKeyPress         = CBCecKeyPress;
      m_configuration->callbacks->CBCecCommand          = CBCecCommand;
      m_configuration->callbacks->CBCecMenuStateChanged = CBCecMenuStateChanged;
      m_configuration->callbacks->CBCecSourceActivated  = CBCecSourceActivated;
    }

    /**
     * Unreferences all python callbacks, and deletes the callbacks
     */
    virtual ~CCecPythonCallbacks(void)
    {
      for (size_t ptr = 0; ptr < NB_PYTHON_CB; ++ptr)
        if (m_callbacks[ptr])
          Py_XDECREF(m_callbacks[ptr]);
      delete m_configuration->callbacks;
      m_configuration->callbacks = NULL;
    }

    /**
     * Call a python callback (if set)
     * @param callback  the callback to call
     * @param arglist   the arguments to pass to the callback
     * @return 1 when processed, 0 otherwise
     */
    int CallPythonCallback(enum libcecSwigCallback callback, PyObject* arglist)
    {
      assert(callback < NB_PYTHON_CB);
      assert(arglist);

      if (!m_callbacks[callback])
        return 0;

      PyObject* result = NULL;
      if (m_callbacks[callback] && arglist)
      {
        /** call the callback */
        result = PyEval_CallObject(m_callbacks[callback], arglist);

        /** unref the argument and result */
        Py_DECREF(arglist);
        if (result)
          Py_XDECREF(result);
      }

      return 1;
    }

    /**
     * Set a python callable as callback
     * @param cb      the callback to set
     * @param pyfunc  the python callable to call
     */
    void SetCallback(size_t cb, PyObject* pyfunc)
    {
      assert(cb < NB_PYTHON_CB);
      assert(PyCallable_Check(pyfunc));

      /** unref previous callback (if any) */
      if (m_callbacks[cb])
        Py_XDECREF(m_callbacks[cb]);

      /** set the new callback */
      m_callbacks[cb] = pyfunc;
      Py_XINCREF(pyfunc);
    }

  private:
    static inline int CallPythonCallback(void* param, enum libcecSwigCallback callback, PyObject* arglist)
    {
      CCecPythonCallbacks* pCallbacks = static_cast<CCecPythonCallbacks*>(param);
      return pCallbacks ?
        pCallbacks->CallPythonCallback(callback, arglist) :
        0;
    }

    static int CBCecLogMessage(void* param, const CEC::cec_log_message message)
    {
      PyGILState_STATE gstate = PyGILState_Ensure();
      PyObject* arglist = Py_BuildValue("(I,I,s)", message.level, (long)message.time, message.message);
      int retval = CallPythonCallback(param, PYTHON_CB_LOG_MESSAGE, arglist);
      PyGILState_Release(gstate);
      return retval;
    }

    static int CBCecKeyPress(void* param, const CEC::cec_keypress key)
    {
      PyGILState_STATE gstate = PyGILState_Ensure();
      int retval = CallPythonCallback(param, PYTHON_CB_KEY_PRESS,
                                      Py_BuildValue("(I,I)", (long)key.keycode, (long)key.duration));
      PyGILState_Release(gstate);
      return retval;
    }

    static int CBCecCommand(void* param, const CEC::cec_command command)
    {
      PyGILState_STATE gstate = PyGILState_Ensure();
      int retval = CallPythonCallback(param, PYTHON_CB_COMMAND,
                                      Py_BuildValue("(s)", CEC::CCECTypeUtils::ToString(command).c_str()));
      PyGILState_Release(gstate);
      return retval;
    }

    static int CBCecMenuStateChanged(void* param, const CEC::cec_menu_state state)
    {
      PyGILState_STATE gstate = PyGILState_Ensure();
      int retval = CallPythonCallback(param, PYTHON_CB_MENU_STATE,
                                      Py_BuildValue("(i)", state));
      PyGILState_Release(gstate);
      return retval;
    }

    static void CBCecSourceActivated(void* param, const CEC::cec_logical_address logicalAddress, const uint8_t activated)
    {
      PyGILState_STATE gstate = PyGILState_Ensure();
      CallPythonCallback(param, PYTHON_CB_SOURCE_ACTIVATED,
                         Py_BuildValue("(I,i)", logicalAddress, activated ? 1 : 0));
      PyGILState_Release(gstate);
    }

    PyObject*             m_callbacks[NB_PYTHON_CB];
    libcec_configuration* m_configuration;
  };

  static CCecPythonCallbacks* _GetCallbacks(CEC::libcec_configuration* self)
  {
    /** allocate callbacks struct and python callbacks if needed */
    if (!self->callbackParam)
      self->callbackParam = new CCecPythonCallbacks(self);

    return static_cast<CCecPythonCallbacks*>(self->callbackParam);
  }
}

static void _SetCallback(CEC::libcec_configuration* self, size_t cb, PyObject* pyfunc)
{
  assert(self);
  CEC::CCecPythonCallbacks* pCallbacks = CEC::_GetCallbacks(self);
  if (pCallbacks)
    pCallbacks->SetCallback(cb, pyfunc);
  else
    printf("ERROR: cannot set callback to %p: out of memory\n", pyfunc);
}

void _ClearCallbacks(CEC::libcec_configuration* self)
{
  CEC::CCecPythonCallbacks* pCallbacks = static_cast<CEC::CCecPythonCallbacks*>(self->callbackParam);
  if (pCallbacks)
    delete pCallbacks;
  self->callbackParam = NULL;
}

