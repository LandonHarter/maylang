enum RuntimeFlag {
    FLAG_DEBUG = 1 << 0
};

struct Runtime {
    unsigned int flags;
};

void set_global_runtime(struct Runtime* runtime);
struct Runtime* get_global_runtime(void);
void enable_runtime_flag(char* flag, struct Runtime* runtime);
int flag_enabled(enum RuntimeFlag rf, struct Runtime* runtime);
