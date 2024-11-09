// Copyright (c) 2018-2023 NetFoundry Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * \file repeat-fetch.c
 * \brief demonstrates re-connecting usage of HTTP client
 */

#include "common.h"
#include <awaittlsuv.h>

using namespace awaittlsuv;

awaitable_t<void> make_request(uv_timer_t* timer, tlsuv_http_t& clt)
{
  tlsuv_http_req_t* req = tlsuv_http_req(&clt, "GET", "/api/timezone/EST");
  tlsuv_http_resp_t* resp = co_await tlsuv_http_req_wait(req);

  if (!resp_cb(resp, nullptr))
  {
    // co_await is required, which will make the coroutine finished in the next loop iteration
    // It ensures that we are done with the callback.
    co_await uv_timer_start(timer, 0);
    co_return;
  }

  http_resp_body_reader_t reader{ resp };
  while (true)
  {
    http_resp_body resp_body = co_await reader.read_next();
    bool           succ = body_cb(resp->req, resp_body.body, resp_body.len);
    if (!succ)
      break;
  }

  co_await uv_timer_start(timer, 0);

  printf("\n\n");
}

awaitable_t<void> make_request(uv_loop_t* loop, int count)
{
  uv_timer_t timer;
  uv_timer_init(loop, &timer);

  tlsuv_http_t clt;
  co_await uv_timer_start(&timer, 1000);

  tlsuv_http_init(loop, &clt, "https://worldtimeapi.org");
  tlsuv_http_connect_timeout(&clt, 1000);

  while (count > 0)
  {
    co_await make_request(&timer, clt);
    count -= 1;
  }

  co_await tlsuv_http_close(&clt);
  co_await uv_close(&timer);
}

int main(int argc, char** argv)
{
  uv_loop_t* loop = uv_default_loop();
//  tlsuv_set_debug(6, logger);
  make_request(loop, 2);
  uv_run(loop, UV_RUN_DEFAULT);

  return 0;
}
