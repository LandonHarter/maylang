#include "cJSON.h"
#include "types.h"
#include "env.h"
#include "error.h"
#include "module.h"
#include "lib/io.h"
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "cJSON.h"

struct Buffer {
    char *data;
    size_t size;
};

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t total = size * nmemb;
    struct Buffer *buf = (struct Buffer *)userp;
    buf->data = realloc(buf->data, buf->size + total + 1);
    memcpy(buf->data + buf->size, contents, total);
    buf->size += total;
    buf->data[buf->size] = '\0';
    return total;
}

struct MayValue* cjson_to_mayvalue(cJSON *item) {
    if (!item) return NULL;
    struct MayValue *val = malloc(sizeof(struct MayValue));

    if (cJSON_IsString(item)) {
        val->type = MAY_STRING;
        val->as.string.val = strdup(item->valuestring);
        val->as.string.len = strlen(item->valuestring);
    }
    else if (cJSON_IsNumber(item)) {
        val->type = MAY_FLOAT;
        val->as.floating = item->valuedouble;
    }
    else if (cJSON_IsArray(item)) {
        val->type = MAY_ARRAY;
        int count = cJSON_GetArraySize(item);
        val->as.array.length = count;
        val->as.array.elements = malloc(sizeof(struct MayValue*) * count);

        cJSON *child = item->child;
        for (int i = 0; i < count; i++) {
            val->as.array.elements[i] = cjson_to_mayvalue(child);
            child = child->next;
        }
    }
    else if (cJSON_IsObject(item)) {
        val->type = MAY_OBJECT;
        int count = cJSON_GetArraySize(item);
        val->as.object.num_values = count;
        val->as.object.names = malloc(sizeof(char*) * count);
        val->as.object.values = malloc(sizeof(struct MayValue*) * count);

        cJSON *child = item->child;
        for (int i = 0; i < count; i++) {
            val->as.object.names[i] = strdup(child->string);
            val->as.object.values[i] = cjson_to_mayvalue(child);
            child = child->next;
        }
    }
    else if (cJSON_IsBool(item)) {
        val->type = MAY_FLOAT;
        val->as.floating = cJSON_IsTrue(item) ? 1 : 0;
    }

    return val;
}

struct MayValue* api_curl(struct MayValue** args, int arg_count) {
    char* url = args[0]->as.string.val;
    struct MayValue* params = args[1];

    CURL *curl = curl_easy_init();
    struct Buffer response_buffer = {NULL, 0};
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_buffer);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl error: %s\n", curl_easy_strerror(res));
        }

        cJSON* json = cJSON_Parse(response_buffer.data);
        struct MayValue* ret;
        if (json) {
            ret = cjson_to_mayvalue(json);
            cJSON_Delete(json);
        } else {
            ret = malloc(sizeof(struct MayValue));
            ret->type = MAY_STRING;
            ret->as.string.val = response_buffer.data;
            ret->as.string.len = response_buffer.size;
        }
        curl_easy_cleanup(curl);
        return ret;
    }

    return NULL;
}

void load_api_env(struct Env* env) {
    struct MayValue* testval = malloc(sizeof(struct MayValue));
    testval->type = MAY_OBJECT;

    char** names = malloc(sizeof(char*));
    names[0] = strdup("field");
    testval->as.object.names = names;

    struct MayValue* testfield = malloc(sizeof(struct MayValue));
    testfield->type = MAY_STRING;
    testfield->as.string.val = "my secret message";
    testfield->as.string.len = 17;

    struct MayValue** values = malloc(sizeof(struct MayValue*));
    values[0] = testfield;
    testval->as.object.values = values;

    testval->as.object.num_values = 1;
    register_builtin_var(env, "myobj", testval);

    enum MayValueType* atcurl = malloc(sizeof(enum MayValueType) * 2);
    atcurl[0] = MAY_STRING;
    atcurl[1] = MAY_OBJECT;

    register_builtin(env, "curl", api_curl, 2, atcurl);
}

struct Module* load_api_module() {
    struct Module* mod = malloc(sizeof(struct Module));
    mod->name = "api";
    mod->loader = load_api_env;
    return mod;
}
