/*====================================================
 * Copyright (c) 2015-2016 ZIPPY.Z All Rights Reserved.
 *====================================================*/
#ifndef SRC_UTILS_LOGGER_H_
#define SRC_UTILS_LOGGER_H_

#include "utils/logging.h"
#include "gflags/gflags.h"

// @Author dongyue.zhang(zhangdy1986(at)gmail.com)
// @Brief Log print, thread safe, TODO(zdy): no buffering

DEFINE_string(binlog_prefix, "./mylog_", "log prefix");

using ::utils::Logger;
using ::utils::FileLogPolicy;
using ::utils::SeverityType;
static Logger<FileLogPolicy> log_inst(FLAGS_binlog_prefix);

#define LOGGER       log_inst.Print<SeverityType::debug>
#define LOGGER_ERR   log_inst.Print<SeverityType::error>
#define LOGGER_WARN  log_inst.Print<SeverityType::warning>
#define LOGGER_BIN   log_inst.PrintBinary

#ifdef RANK_LOG_DEBUG
#define ELOGGER      log_inst.Print<SeverityType::debug>
#define ELOGGER_ERR  log_inst.Print<SeverityType::error>
#define ELOGGER_WARN log_inst.Print<SeverityType::warning>
#define ELOGGER_BIN  log_inst.PrintBinary
#endif

#endif  // SRC_UTILS_LOGGER_H_

/* vim: set ts=2 sts=2 sw=2 tw=80 et */
