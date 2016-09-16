// Copyright (c) 2015 Cesanta Software Limited
// All rights reserved
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#define __unix__
#include "mongoose.h"

static const char *s_http_port = ":80";
static const char *s_https_port = ":443";
static struct mg_serve_http_opts s_http_server_opts;

static void handle_call(struct mg_connection *nc, struct http_message *hm) {
  /* Send headers */
  mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");

  mg_send_http_chunk(nc, "It works!\r\n", 10);
  mg_send_http_chunk(nc, "", 0);
}

static void ev_handler(struct mg_connection *nc, int ev, void *p) {
  struct http_message *hm = (struct http_message *) p;
  if (ev == MG_EV_HTTP_REQUEST) {
    //mg_serve_http(nc, (struct http_message *) p, s_http_server_opts);
    handle_call(nc, p);
  }
}

int main(void) {
  struct mg_mgr mgr;
  struct mg_connection *nc;
  const char *port = NULL;

  mg_mgr_init(&mgr, NULL);
#ifdef ENABLE_SSL
  port = s_https_port;
  nc = mg_bind(&mgr, port, ev_handler);
  printf("Enabling SSL in Mongoose: %s\n", mg_set_ssl(nc, (char *)1u, NULL));
#else
  port = s_http_port;
  nc = mg_bind(&mgr, port, ev_handler);
#endif

  // Set up HTTP server parameters
  mg_set_protocol_http_websocket(nc);
  s_http_server_opts.document_root = "/";  // Serve current directory
  //s_http_server_opts.dav_document_root = "/";  // Allow access via WebDav
  s_http_server_opts.enable_directory_listing = "yes";

  printf("Starting web server on port %s\n", port);
  for (;;) {
    mg_mgr_poll(&mgr, 1000);
  }
  mg_mgr_free(&mgr);

  return 0;
}
