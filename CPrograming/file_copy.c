#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define SHM_SIZE 1024  // 공유 메모리 크기

// 공유 메모리 내에서 사용할 구조체
typedef struct {
    int data_ready; // 1: 데이터 있음(읽기 가능), 0: 비어있음(쓰기 가능)
    int eof;        // 파일 끝 도달 여부
    int size;       // 실제 읽은 바이트 수
    char buffer[SHM_SIZE - sizeof(int)*3]; 
} shm_data_t;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "사용법: %s <원본 파일> <대상 파일>\n", argv[0]);
        exit(1);
    }

    // 공유 메모리 생성
    key_t key = ftok(".", 'S');
    int shmid = shmget(key, sizeof(shm_data_t), 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget 실패");
        exit(1);
    }

    // 공유 메모리 연결
    shm_data_t *shm = (shm_data_t *)shmat(shmid, NULL, 0);
    if (shm == (void *)-1) {
        perror("shmat 실패");
        exit(1);
    }

    // 초기화
    shm->data_ready = 0;
    shm->eof = 0;

    pid_t pid = fork();

    if (pid > 0) {
        // [부모] Source 파일 읽기 -> 공유 메모리 쓰기
        FILE *src = fopen(argv[1], "rb");
        if (!src) { perror("원본 파일 열기 실패"); exit(1); }

        while (1) {
            // 자식이 데이터를 읽어갈 때까지 대기 (Busy waiting)
            while (shm->data_ready == 1);

            int n = fread(shm->buffer, 1, sizeof(shm->buffer), src);
            shm->size = n;
            
            if (n > 0) {
                shm->data_ready = 1; // 데이터 준비 완료
            }

            if (feof(src)) {
                shm->eof = 1;
                shm->data_ready = 1; // EOF 신호 전송
                break;
            }
        }
        fclose(src);
        wait(NULL); // 자식 종료 대기

        // 공유 메모리 해제
        shmdt(shm);
        shmctl(shmid, IPC_RMID, NULL);
        printf("복사 완료.\n");

    } else if (pid == 0) {
        // [자식] 공유 메모리 읽기 -> 대상 파일 쓰기
        FILE *dest = fopen(argv[2], "wb");
        if (!dest) { perror("대상 파일 열기 실패"); exit(1); }

        while (1) {
            // 데이터가 준비될 때까지 대기
            while (shm->data_ready == 0);

            if (shm->size > 0) {
                fwrite(shm->buffer, 1, shm->size, dest);
            }

            if (shm->eof) {
                break;
            }

            shm->data_ready = 0; // 읽기 완료 표시
        }
        fclose(dest);
        shmdt(shm);
        exit(0);
    } else {
        perror("fork 실패");
        exit(1);
    }

    return 0;
}

