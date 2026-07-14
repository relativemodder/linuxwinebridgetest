#include <windows.h>
#include <wine/debug.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

WINE_DEFAULT_DEBUG_CHANNEL(nativelib);

#define BRIDGE_SOCK_PATH "/tmp/gdbridge.sock"
#define BRIDGE_BIN       "rust-bridge"
#define PATH_BUF_SIZE    4096
#define CMD_BUF_SIZE     4096

static HMODULE s_hModule = NULL;

BOOL WINAPI DllMain(HMODULE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            s_hModule = hinstDLL;
            TRACE("DLL_PROCESS_ATTACH\n");
            break;
        case DLL_PROCESS_DETACH:
            TRACE("DLL_PROCESS_DETACH\n");
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            break;
    }
    return TRUE;
}

static int get_dll_dir(char *out, size_t size)
{
    WCHAR dll_path_w[PATH_BUF_SIZE];
    GetModuleFileNameW(s_hModule, dll_path_w, sizeof(dll_path_w) / sizeof(WCHAR));

    char *unix_path = wine_get_unix_file_name(dll_path_w);
    if (!unix_path) return 0;

    snprintf(out, size, "%s", unix_path);

    char *sep = strrchr(out, '/');
    if (sep) *sep = '\0';

    return 1;
}

/* game_pid is passed as argv[1] so the child can watch for parent death */
static int spawn_detached(const char *path, const char *game_pid)
{
    pid_t pid = fork();
    if (pid < 0) return -1;

    if (pid == 0) {
        pid_t inner = fork();
        if (inner == 0) {
            execl(path, BRIDGE_BIN, game_pid, NULL);
            _exit(1);
        }
        _exit(0);
    }

    int status;
    waitpid(pid, &status, 0);
    return 0;
}

int WINAPI NativeStartBridge(void)
{
    char dir[PATH_BUF_SIZE];
    if (!get_dll_dir(dir, sizeof(dir)))
        return 1;

    char path[PATH_BUF_SIZE];
    snprintf(path, sizeof(path), "%s/%s", dir, BRIDGE_BIN);
    chmod(path, 0755);

    char game_pid[32];
    snprintf(game_pid, sizeof(game_pid), "%d", (int)getpid());

    return spawn_detached(path, game_pid);
}

int WINAPI NativeSendCommand(const char *cmd, char *resp_buf, int resp_size)
{
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) return 1;

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", BRIDGE_SOCK_PATH);

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        { close(fd); return 2; }

    char line[CMD_BUF_SIZE];
    snprintf(line, sizeof(line), "%s\n", cmd);
    if (write(fd, line, strlen(line)) < 0)
        { close(fd); return 3; }

    int n = read(fd, resp_buf, resp_size - 1);
    if (n < 0) { close(fd); return 4; }

    resp_buf[n] = '\0';
    char *nl = strchr(resp_buf, '\n');
    if (nl) *nl = '\0';

    close(fd);
    return 0;
}
