#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <sys/param.h>
#include <mbedtls/base64.h>

#include "net/http/server/http_server.h"
#include "net/http/server/esp32/register_uris.h"

#include "userio/status_json.h"
#include "config/config.h"
#include "cli/mutex.h"
#include "cli_app/cli_imp.h"
#include "misc/int_types.h"

#include "webapp/content.h"

bool check_access_allowed(httpd_req_t *req); //XXX move this to a header file

#define URI_WAPP_HTML "/"
#define URI_WAPP_CSS "/f/css/wapp.css"
#define URI_WAPP_JS "/f/js/wapp.js"
#define URI_WAPP_JS_MAP "/f/js/wapp.js.map"
#define URI_WAPP_CSS_MAP "/f/css/wapp.css.map"
//#define SERVE_BR

static const char *TAG="APP";

/*
 * Structure holding server handle
 * and internal socket fd in order
 * to use out of request send
 */
struct async_resp_arg {
    httpd_handle_t hd;
    int fd;
};

static fd_set ws_fds;
static int ws_nfds;
struct ws_send_arg {
  httpd_handle_t hd;
  char *json;
};

static void ws_send(void *arg) {
  struct ws_send_arg *a = arg;
  httpd_ws_frame_t ws_pkt = { .type = HTTPD_WS_TYPE_TEXT, .payload = (u8*)a->json, .len = strlen(a->json), .final = true};

  for (int fd = 0; fd < ws_nfds; ++fd) {
    if (!FD_ISSET(fd, &ws_fds))
      continue;
    esp_err_t res = httpd_ws_send_frame_async(a->hd, fd, &ws_pkt);
    if (res != ESP_OK) {
      FD_CLR(fd, &ws_fds);
      if (ws_nfds == fd + 1)
        ws_nfds = fd;
    }
  }
  free(a->json);
  free(a);
}

static esp_err_t ws_trigger_send(httpd_handle_t handle, const char *json) {
  struct ws_send_arg *arg = malloc(sizeof(struct ws_send_arg));
  arg->hd = handle;
  arg->json = strdup(json);
  return httpd_queue_work(handle, ws_send, arg);
}

void ws_send_json(const char *json) {
  ws_trigger_send(hts_server, json);
}

/*
 * async send function, which we put into the httpd work queue
 */
static void ws_async_send(void *arg)
{
  ESP_LOGI(TAG, "ws_async_send");
    static const char * data = "Async data";
    struct async_resp_arg *resp_arg = arg;
    httpd_handle_t hd = resp_arg->hd;
    int fd = resp_arg->fd;
    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = (uint8_t*)data;
    ws_pkt.len = strlen(data);
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;
    ws_pkt.final = true;

    httpd_ws_send_frame_async(hd, fd, &ws_pkt);
    free(resp_arg);
}

static esp_err_t trigger_async_send(httpd_handle_t handle, httpd_req_t *req)
{
  ESP_LOGI(TAG, "trigger_async_send");
    struct async_resp_arg *resp_arg = malloc(sizeof(struct async_resp_arg));
    resp_arg->hd = req->handle;
    resp_arg->fd = httpd_req_to_sockfd(req);
    return httpd_queue_work(handle, ws_async_send, resp_arg);
}

////////////////////////// URI handlers /////////////////////////////////
static esp_err_t handle_uri_cmd_json(httpd_req_t *req) {
  char buf[256];
  int ret, remaining = req->content_len;

  if (!check_access_allowed(req))
    return ESP_FAIL;

  if ((ret = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)))) <= 0) {
    return ESP_FAIL;
  }

  if (mutex_cliTake()) {
    hts_query(HQT_NONE, buf, ret); // parse and process received command

    httpd_resp_set_type(req, "application/json");
    if (sj_get_json()) {
      httpd_resp_sendstr(req, sj_get_json());
      sj_free_buffer();
    } else {
      httpd_resp_sendstr(req, "{}");
    }
    mutex_cliGive();
  }

  return ESP_OK;
}

#if 0
struct file_map { const char *uri, *type, *file, *file_gz, *file_br; u16 file_size, file_gz_size, file_br_size; };
const struct file_map uri_file_map[] = {
    { .uri = "/", .type = "text/html", .file_gz = build_wapp_html_gz, .file_gz_size = sizeof build_wapp_html_gz }, //
    { .uri = "/f/js/wapp.js", .type = "text/javascript", .file_gz = build_wapp_js_gz, .file_gz_size = sizeof build_wapp_js_gz  }, //
#ifdef SERVE_JS_MAP
    { .uri = "/f/js/wapp.js.map", .type = "text/javascript",  .file_br = build_wapp_js_map_br, .file_br_size = sizeof build_wapp_js_map_br }, //
#endif
    { .uri = "/f/css/wapp.css", .type = "text/css", .file_gz = build_wapp_css_gz, .file_gz_size = sizeof build_wapp_css_gz  }, //
    { .uri = "/f/cli/help/config", .file = cli_help_parmConfig }, //
    { .uri = "/f/cli/help/mcu", .file = cli_help_parmMcu }, //
    { .uri = "/f/cli/help/help", .file = cli_help_parmHelp }, //
    { .uri = "/f/cli/help/kvs", .file = cli_help_parmKvs }, //
    { .uri = "/f/cli/help/cmd", .file = cli_help_parmCmd }, //
    };


static esp_err_t handle_uri_get_file(httpd_req_t *req) {
  if (!check_access_allowed(req))
    return ESP_FAIL;

  for (int i = 0; i < sizeof(uri_file_map) / sizeof(uri_file_map[0]); ++i) {
    const struct file_map *fm = &uri_file_map[i];
    if (strcmp(req->uri, fm->uri) != 0)
      continue;
    const char *type = (fm->type) ? fm->type : "text/plain;charset=\"UTF-8\"";
    httpd_resp_set_type(req, type);

    if (fm->file_br) {
      httpd_resp_set_hdr(req, "content-encoding", "br");
      httpd_resp_send(req, fm->file_br, fm->file_br_size);
      return ESP_OK;
    }

    if (fm->file_gz) {
      httpd_resp_set_hdr(req, "content-encoding", "gzip");
      httpd_resp_send(req, fm->file_gz, fm->file_gz_size);
      return ESP_OK;
    }

    if (fm->file) {
      if (fm->file_size)
        httpd_resp_send(req, fm->file_gz, fm->file_size);
      else
        httpd_resp_sendstr(req, fm->file);
      return ESP_OK;
    }
  }

  return ESP_FAIL;
}

#else
struct file_map { const char *uri, *type, *file; u32 file_size; };

const struct file_map uri_file_map[] =  {
    { .uri = "/f/cli/help/config", .file = cli_help_parmConfig }, //
    { .uri = "/f/cli/help/mcu", .file = cli_help_parmMcu }, //
    { .uri = "/f/cli/help/help", .file = cli_help_parmHelp }, //
    { .uri = "/f/cli/help/kvs", .file = cli_help_parmKvs }, //
    { .uri = "/f/cli/help/cmd", .file = cli_help_parmCmd }, //
};


static esp_err_t respond_file(httpd_req_t *req, const struct file_map *fm, const char *content_encoding) {
  httpd_resp_set_type(req, fm->type ? fm->type : "text/plain;charset=\"UTF-8\"");
  if (content_encoding)
    httpd_resp_set_hdr(req, "content-encoding", content_encoding);
  if (fm->file_size)
    httpd_resp_send(req, fm->file, fm->file_size);
  else
    httpd_resp_sendstr(req, fm->file);
  return ESP_OK;
}

static esp_err_t handle_uri_get_file(httpd_req_t *req) {
  if (!check_access_allowed(req))
    return ESP_FAIL;

  struct file_map fm = {};
  const char *encoding = "gzip";

  if (strcmp(req->uri, URI_WAPP_HTML) == 0) {
    fm.file = build_wapp_html_gz;
    fm.file_size = build_wapp_html_gz_len;
    fm.type = "text/html";
  } else  if (strcmp(req->uri, URI_WAPP_CSS) == 0) {
    fm.file = build_wapp_css_gz;
    fm.file_size = build_wapp_css_gz_len;
    fm.type = "text/css";
  } else  if (strcmp(req->uri, URI_WAPP_JS) == 0) {
    fm.file = build_wapp_js_gz;
    fm.file_size = build_wapp_js_gz_len;
    fm.type = "text/javascript";
#ifdef URI_WAPP_JS_MAP
  } else  if (strcmp(req->uri, URI_WAPP_JS_MAP) == 0) {
    fm.type = "text/javascript";
#ifdef SERVE_BR
    fm.file = build_wapp_js_map_br;
    fm.file_size = build_wapp_js_map_br_len;
    encoding = "br";
#else
    fm.file = build_wapp_js_map_gz;
    fm.file_size = build_wapp_js_map_gz_len;
#endif
#endif
#ifdef URI_WAPP_CSS_MAP
  } else  if (strcmp(req->uri, URI_WAPP_CSS_MAP) == 0) {
    fm.type = "text/css";
#ifdef SERVE_BR
    fm.file = build_wapp_css_map_br;
    fm.file_size = build_wapp_css_map_br_len;
    encoding = "br";
#else
    fm.file = build_wapp_css_map_gz;
    fm.file_size = build_wapp_css_map_gz_len;
#endif
#endif
  } else {
    for (int i = 0; i < sizeof(uri_file_map) / sizeof(uri_file_map[0]); ++i) {
      if (strcmp(req->uri, uri_file_map[i].uri) == 0)
        return respond_file(req, &uri_file_map[i], "");
    }
    return ESP_FAIL;
  }

  return respond_file(req, &fm, encoding);

}

#endif


static esp_err_t handle_uri_ws(httpd_req_t *req) {
  int fd = httpd_req_to_sockfd(req);
  FD_SET(fd, &ws_fds);
  if (ws_nfds <= fd)
    ws_nfds = 1 + fd;
  ESP_LOGI(TAG, "handle /ws using fd: %d", fd);
  uint8_t buf[128] = { 0 };

  httpd_ws_frame_t ws_pkt = {.payload = buf };
  esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, (sizeof buf)-1);

  if (ret != ESP_OK) {
      ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
      return ret;
  }
  ESP_LOGI(TAG, "Got packet with message: %s", ws_pkt.payload);
  ESP_LOGI(TAG, "Packet type: %d", ws_pkt.type);

  if (ws_pkt.type == HTTPD_WS_TYPE_CLOSE) { // XXX: does nothing. remove this
    ESP_LOGI(TAG, "Close fd: %d", fd);
    FD_CLR(fd, &ws_fds);
  }


  if (mutex_cliTake()) {
    buf[ws_pkt.len] = '\0';
    hts_query0(HQT_NONE, (char*)buf); // parse and process received command

    ws_pkt.payload = (u8*)sj_get_json();
    ws_pkt.len = strlen((char*)ws_pkt.payload);
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;

    ret = httpd_ws_send_frame(req, &ws_pkt);
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", ret);
    }

    sj_free_buffer();
    mutex_cliGive();
  }
  return ret;
}

////////////////////////// URI definitions ////////////////////////////////
static const httpd_uri_t httpd_uris[] = {
    { .uri = "/cmd.json", .method = HTTP_POST, .handler = handle_uri_cmd_json },
    { .uri = "/f/*", .method = HTTP_GET, .handler = handle_uri_get_file },
    { .uri = "/", .method = HTTP_GET, .handler = handle_uri_get_file },
    { .uri = "/ws", .method = HTTP_GET, .handler = handle_uri_ws, .user_ctx = NULL, .is_websocket = true },
};


///////// public ///////////////
void hts_register_uri_handlers(httpd_handle_t server) {
  int i;

  ESP_LOGI(TAG, "Registering URI handlers");
  for (i = 0; i < sizeof(httpd_uris) / sizeof(httpd_uri_t); ++i) {
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &httpd_uris[i]));
  }

}
