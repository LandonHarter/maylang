#include "lib/http.h"
#include "types.h"
#include "env.h"
#include "error.h"
#include "module.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

struct MayValue* http_create_server(struct MayValue** args, int arg_count) {
    int port = (int)args[0]->as.floating;

    int server_fd;
    struct sockaddr_in server_addr;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        throw_runtime_error("Failed to create http server");
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        throw_runtime_error("Failed to bind http server");
    }

    if (listen(server_fd, 10) < 0) {
        throw_runtime_error("Failed to listen on http server");
    }

    struct MayValue* result = malloc(sizeof(struct MayValue));
    result->type = MAY_INT;
    result->inferred = MAY_INT;
    result->as.integer = server_fd;
    return result;
}

struct MayValue* http_accept_client(struct MayValue** args, int arg_count) {
    int server_fd = args[0]->as.integer;

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);

    if (client_fd < 0) {
        throw_runtime_error("Failed to accept http client");
    }

    struct MayValue* result = malloc(sizeof(struct MayValue));
    result->type = MAY_INT;
    result->inferred = MAY_INT;
    result->as.integer = client_fd;
    return result;
}

struct MayValue* http_read_request(struct MayValue** args, int arg_count) {
    int client_fd = args[0]->as.integer;

    char buffer[4096];
    int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        throw_runtime_error("Failed to read http request");
    }
    buffer[bytes_read] = '\0';

    char method[16] = {0};
    char path[256] = {0};
    sscanf(buffer, "%15s %255s", method, path);

    char** names = malloc(sizeof(char*) * 3);
    struct MayValue** values = malloc(sizeof(struct MayValue*) * 3);

    names[0] = strdup("method");
    values[0] = malloc(sizeof(struct MayValue));
    values[0]->type = MAY_STRING;
    values[0]->inferred = MAY_STRING;
    values[0]->as.string.val = strdup(method);
    values[0]->as.string.len = strlen(method);

    names[1] = strdup("path");
    values[1] = malloc(sizeof(struct MayValue));
    values[1]->type = MAY_STRING;
    values[1]->inferred = MAY_STRING;
    values[1]->as.string.val = strdup(path);
    values[1]->as.string.len = strlen(path);

    names[2] = strdup("body");
    values[2] = malloc(sizeof(struct MayValue));
    values[2]->type = MAY_STRING;
    values[2]->inferred = MAY_STRING;
    char* body_start = strstr(buffer, "\r\n\r\n");
    if (body_start) {
        body_start += 4;
        values[2]->as.string.val = strdup(body_start);
        values[2]->as.string.len = strlen(body_start);
    } else {
        values[2]->as.string.val = strdup("");
        values[2]->as.string.len = 0;
    }

    struct MayValue* result = malloc(sizeof(struct MayValue));
    result->type = MAY_OBJECT;
    result->inferred = MAY_OBJECT;
    result->as.object.names = names;
    result->as.object.values = values;
    result->as.object.num_values = 3;
    return result;
}

struct MayValue* http_send_html(struct MayValue** args, int arg_count) {
    int client_fd = args[0]->as.integer;
    char* html = args[1]->as.string.val;
    int html_len = args[1]->as.string.len;

    char header[512];
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n", html_len);

    write(client_fd, header, header_len);
    write(client_fd, html, html_len);
    close(client_fd);

    return NULL;
}

struct MayValue* http_send_response(struct MayValue** args, int arg_count) {
    int client_fd = args[0]->as.integer;
    int status = (int)args[1]->as.floating;
    char* body = args[2]->as.string.val;
    int body_len = args[2]->as.string.len;

    const char* status_text = "OK";
    if (status == 404) status_text = "Not Found";
    else if (status == 500) status_text = "Internal Server Error";
    else if (status == 301) status_text = "Moved Permanently";
    else if (status == 400) status_text = "Bad Request";
    else if (status == 403) status_text = "Forbidden";

    char header[512];
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n", status, status_text, body_len);

    write(client_fd, header, header_len);
    write(client_fd, body, body_len);
    close(client_fd);

    return NULL;
}

struct MayValue* http_close_client(struct MayValue** args, int arg_count) {
    int client_fd = args[0]->as.integer;
    close(client_fd);
    return NULL;
}

void load_http_env(struct Env* env) {
    enum MayValueType* at_port = malloc(sizeof(enum MayValueType) * 1);
    at_port[0] = MAY_FLOAT;
    register_builtin(env, "create_server", http_create_server, 1, at_port);

    enum MayValueType* at_fd = malloc(sizeof(enum MayValueType) * 1);
    at_fd[0] = MAY_INT;
    register_builtin(env, "accept_client", http_accept_client, 1, at_fd);

    enum MayValueType* at_fd2 = malloc(sizeof(enum MayValueType) * 1);
    at_fd2[0] = MAY_INT;
    register_builtin(env, "read_request", http_read_request, 1, at_fd2);

    enum MayValueType* at_send_html = malloc(sizeof(enum MayValueType) * 2);
    at_send_html[0] = MAY_INT;
    at_send_html[1] = MAY_STRING;
    register_builtin(env, "send_html", http_send_html, 2, at_send_html);

    enum MayValueType* at_send_resp = malloc(sizeof(enum MayValueType) * 3);
    at_send_resp[0] = MAY_INT;
    at_send_resp[1] = MAY_FLOAT;
    at_send_resp[2] = MAY_STRING;
    register_builtin(env, "send_response", http_send_response, 3, at_send_resp);

    enum MayValueType* at_close = malloc(sizeof(enum MayValueType) * 1);
    at_close[0] = MAY_INT;
    register_builtin(env, "close_client", http_close_client, 1, at_close);
}

struct Module* load_http_module() {
    struct Module* mod = malloc(sizeof(struct Module));
    mod->name = "http";
    mod->loader = load_http_env;
    return mod;
}
