/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this
 *file except in compliance with the License. You may obtain a copy of the
 *License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

/*! file
brief
author
date 2020
email: contact@openairinterface.org
*/

#ifndef __LOGGER_H
#define __LOGGER_H

#include <cstdarg>
#include <stdexcept>
#include <vector>

#define SPDLOG_LEVEL_NAMES                                                     \
  {"trace", "debug", "info ", "start", "warn ", "error", "off  "};

#define SPDLOG_ENABLE_SYSLOG
#include "spdlog/spdlog.h"

class LoggerException : public std::runtime_error {
 public:
  explicit LoggerException(const char* m) : std::runtime_error(m) {}
  explicit LoggerException(const std::string& m) : std::runtime_error(m) {}
};

class _Logger {
 public:
  _Logger(
      const char* category, std::vector<spdlog::sink_ptr>& sinks,
      const char* pattern);

  void trace(const char* format, ...);
  void trace(const std::string& format, ...);
  void debug(const char* format, ...);
  void debug(const std::string& format, ...);
  void info(const char* format, ...);
  void info(const std::string& format, ...);
  void startup(const char* format, ...);
  void startup(const std::string& format, ...);
  void warn(const char* format, ...);
  void warn(const std::string& format, ...);
  void error(const char* format, ...);
  void error(const std::string& format, ...);

 private:
  _Logger();
  enum _LogType { _ltTrace, _ltDebug, _ltInfo, _ltStartup, _ltWarn, _ltError };

  void log(_LogType lt, const char* format, va_list& args);
  spdlog::logger m_log;
};

class Logger {
 public:
  static void init(
      const char* app, const bool log_stdout, const bool log_rot_file) {
    singleton()._init(app, log_stdout, log_rot_file);
  }
  static void init(
      const std::string& app, const bool log_stdout, const bool log_rot_file) {
    init(app.c_str(), log_stdout, log_rot_file);
  }

  static _Logger& config() { return *singleton().m_config; }
  static _Logger& system() { return *singleton().m_system; }
  static _Logger& ausf_app() { return *singleton().m_ausf_app; }
  static _Logger& ausf_nrf() { return *singleton().m_ausf_nrf; }
  static _Logger& ausf_server() { return *singleton().m_ausf_server; }

 private:
  static Logger* m_singleton;
  static Logger& singleton() {
    if (!m_singleton) m_singleton = new Logger();
    return *m_singleton;
  }

  Logger() {}
  ~Logger() {}

  void _init(const char* app, const bool log_stdout, const bool log_rot_file);

  std::vector<spdlog::sink_ptr> m_sinks;

  std::string m_pattern;

  _Logger* m_config;
  _Logger* m_system;
  _Logger* m_ausf_app;
  _Logger* m_ausf_nrf;
  _Logger* m_ausf_server;
};

#endif
