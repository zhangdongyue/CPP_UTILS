//=====================================================
// Copyright(c) 2015-2017 ZIPPY.ZDY All Rights Reserved.
//=====================================================
#include "utils/redis_storage_policy.h"

// @Author Dongyue.Zhang
// @Mailto zhangdy1986(at)gmail.com 
// @Date   2016-09-24

namespace utils {

thread_local redisContext* RedisStoragePolicy::_redis_ctx = nullptr;

bool RedisStoragePolicy::Init(std::string const& host, int port,
       std::string const& password) {
  _host = host;
  _port = port;
  _password = password;

  return true;
}

int RedisStoragePolicy::Connect() {
  if (_redis_ctx)
    return 0;
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = FLAGS_io_timeout * 1000;
  { // normalized
    long a = tv.tv_usec / 1000000;
    long b = tv.tv_usec % 1000000;
    tv.tv_sec += a;
    tv.tv_usec = b;
  }  // namespace

  _redis_ctx = redisConnect(_host.c_str(), _port);
  if (!_redis_ctx || _redis_ctx->err != 0) {
    Close();
    std::stringstream err_msg;
    err_msg << _host << ":" << _port <<" CONNECT FAILED:"
      << (_redis_ctx ? _redis_ctx->errstr : "UNKNOWN");
    throw std::runtime_error(err_msg.str());
  }

  redisSetTimeout(_redis_ctx, tv);

  if (!_password.empty()) {
    auto ret = redisCommand(_redis_ctx, "AUTH %s", _password.c_str());
    freeReplyObject(ret);
  }

  return 0;
}

int RedisStoragePolicy::Close() {
  if (_redis_ctx) {
    redisFree(_redis_ctx);
    _redis_ctx = nullptr;
  }
  return 0;
}

RedisStoragePolicy::ReturnType RedisStoragePolicy::Get(HandlerType& cmd) {
  std::vector<const char*> cmd_argv;
  std::vector<size_t> cmd_argv_size;
  cmd_argv.reserve(cmd.size());
  cmd_argv_size.reserve(cmd.size());

  for (const std::string& cmd_com : cmd) {
    cmd_argv.push_back(cmd_com.data());
    cmd_argv_size.push_back(cmd_com.size());
  }

  auto redis_reply = static_cast<redisReply*>(
      redisCommandArgv(_redis_ctx, cmd_argv.size(),
        cmd_argv.data(), cmd_argv_size.data()) );
  if (!redis_reply || redis_reply->type == REDIS_REPLY_ERROR) {
    std::stringstream err_msg;
    err_msg << "REDIS GET ERROR: errno:" << _redis_ctx->err
      << " errstr:" << _redis_ctx->errstr;

    if (redis_reply) {
      err_msg << " redis_reply:" << redis_reply->str;
    }

    throw std::runtime_error(err_msg.str());
  }

  return ReturnType{redis_reply, freeReplyObject};
}

}  // namespace utils

/* vim :set ts=2 sts=2 sw=2 tw=80 et */

