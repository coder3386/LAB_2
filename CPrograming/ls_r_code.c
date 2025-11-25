#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

void list_dir(const char *path, int level) {
    DIR *dir;
    struct dirent *entry;
    struct stat st;
    char fullpath[1024];

    if ((dir = opendir(path)) == NULL) {
        perror("opendir");
        return;
    }

    printf("%*s%s:\n", level * 2, "", path);

    while ((entry = readdir(dir)) != NULL) {
        // "." 및 ".." 디렉토리는 무시
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        if (stat(fullpath, &st) < 0) {
            perror("stat");
            continue;
        }

        printf("%*s%s\n", level * 2, "", entry->d_name);

        // 디렉토리이면 재귀 호출
        if (S_ISDIR(st.st_mode)) {
            list_dir(fullpath, level + 1);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    const char *start_dir;

    if (argc < 2) {
        start_dir = ".";  // 인자가 없으면 현재 디렉토리
    } else {
        start_dir = argv[1];
    }

    list_dir(start_dir, 0);

    return 0;
}

