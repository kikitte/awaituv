#include "common.h"
#include <awaittlsuv.h>

using namespace awaittlsuv;

awaitable_t<void> make_request(uv_timer_t* timer, tlsuv_http_t& clt)
{
  tlsuv_http_req_t* req = tlsuv_http_req(&clt, "POST", "/post");
  const char*       msg = "Hello World!";
  tlsuv_http_req_data(req, msg, strlen(msg), nullptr);
  tlsuv_http_resp_t *resp = co_await tlsuv_http_req_wait(req);
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

  tlsuv_http_init(loop, &clt, "https://httpbin.org");
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
  make_request(loop, 1);
  uv_run(loop, UV_RUN_DEFAULT);

  return 0;
}
