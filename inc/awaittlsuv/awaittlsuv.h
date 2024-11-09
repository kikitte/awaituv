#pragma once

#include <awaituv.h>
#include <tlsuv/http.h>
#include <tlsuv/tlsuv.h>

namespace awaittlsuv
{
using namespace awaituv;

inline auto& tlsuv_http_req_wait(awaitable_state<tlsuv_http_resp_t*>& awaitable, tlsuv_http_req_t* req)
{
  req->data = &awaitable;
  req->resp_cb = [](tlsuv_http_resp_t* resp, void* data) {
    // resp_cb should be called only once in order to avoid access the freed memory to the coroutine frame.
    resp->req->data = nullptr;
    resp->req->resp_cb = nullptr;

    auto awaitable = static_cast<awaitable_state<tlsuv_http_resp_t*>*>(data);
    awaitable->set_value(resp);
  };
  return awaitable;
}

inline awaitable_t<tlsuv_http_resp_t*> tlsuv_http_req_wait(tlsuv_http_req_t* req)
{
  awaitable_state<tlsuv_http_resp_t*> state;
  co_return co_await tlsuv_http_req_wait(state, req);
}

inline tlsuv_http_req_t* tlsuv_http_req(tlsuv_http_t* clt, const char* method, const char* path)
{
  return tlsuv_http_req(clt, method, path, nullptr, nullptr);
}

inline auto& tlsuv_http_close(awaitable_state<int>& awaitable, tlsuv_http_t* clt)
{
  clt->data = &awaitable;

  int ret = tlsuv_http_close(clt, [](tlsuv_http_t* clt) {
    auto awaitable = reinterpret_cast<awaitable_state<int>*>(clt->data);

    // close_cb should be called only once
    clt->data = nullptr;
    clt->close_cb = nullptr;

    awaitable->set_value(0);
  });

  if (ret != 0)
    awaitable.set_value(ret);

  return awaitable;
}

inline awaitable_t<int> tlsuv_http_close(tlsuv_http_t* clt)
{
  awaitable_state<int> state;
  co_return co_await tlsuv_http_close(state, clt);
}

struct http_resp_body
{
  char*   body{ nullptr };
  ssize_t len{ 0 };
};

/**
 * Http response body reader.
 *
 * Note: In order to avoid data missing, there must no any co_await between co_await tlsuv_http_req and co_await
 * reader.read_next()
 *
 * Usage:
 *     tlsuv_http_req_t* req = tlsuv_http_req(&clt, "GET", "/api/timezone/EST");
 *     // some configuration to req
 *      // some synchronization operations...
 *
 *     tlsuv_http_resp_t* resp = co_await tlsuv_http_req_wait(req);
 *     // some synchronization operations...
 *
 *     http_resp_body_reader_t reader(resp);
 *     // some synchronization operations...
 *
 *     while (true)
 *     {
 *         http_resp_body body = co_await reader.read_next()
 *         // processing with body but synchronized...
 *
 *         if (body.len == UV_EOF)
 *             break;
 *     }
 */
class http_resp_body_reader_t
{
  void add_body(char* body, ssize_t len)
  {
    http_resp_body info{ body, len };
    if (waiting != nullptr)
    {
      auto p = waiting;
      waiting = nullptr;
      p->set_value(info);
    }
  }

  awaitable_state<http_resp_body>* waiting = nullptr;

public:
  http_resp_body_reader_t(tlsuv_http_resp_t* resp)
  {
    resp->req->data = this;
    resp->body_cb = [](tlsuv_http_req_t* req, char* body, ssize_t len) {
      auto reader = reinterpret_cast<http_resp_body_reader_t*>(req->data);
      reader->add_body(body, len);
    };
  }

  awaitable_t<http_resp_body> read_next()
  {
    assert(waiting == nullptr);

    awaitable_state<http_resp_body> state;
    waiting = &state;

    co_return co_await state;
  }
};


} // namespace awaittlsuv