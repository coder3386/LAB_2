#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <signal.h>

#define MAX_TEXT 512
#define MSG_KEY_PATH "/tmp"  // 키 생성을 위한 경로
#define MSG_KEY_ID   'C'     // 키 생성을 위한 ID

// 메시지 버퍼 구조체
struct msg_buffer {
    long msg_type;
    char msg_text[MAX_TEXT];
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "사용법: %s <ID (1 or 2)>\n", argv[0]);
        exit(1);
    }

    int id = atoi(argv[1]);
    if (id != 1 && id != 2) {
        fprintf(stderr, "ID는 1 또는 2여야 합니다.\n");
        exit(1);
    }

    // 키 생성
    key_t key = ftok(MSG_KEY_PATH, MSG_KEY_ID);
    if (key == -1) {
        perror("ftok 실패");
        exit(1);
    }

    // 메시지 큐 생성 (없으면 생성, 있으면 연결)
    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget 실패");
        exit(1);
    }

    printf("--- 채팅 시작 (ID: %d) ---\n", id);
    printf("종료하려면 'exit'을 입력하세요.\n");

    // 자식 프로세스: 메시지 수신 담당
    pid_t pid = fork();

    if (pid == 0) {
        // 수신 루프
        struct msg_buffer msg;
        // ID가 1이면 타입 2를 읽고, ID가 2면 타입 1을 읽음
        long recv_type = (id == 1) ? 2 : 1;

        while (1) {
            if (msgrcv(msgid, &msg, sizeof(msg.msg_text), recv_type, 0) == -1) {
                perror("msgrcv 실패");
                exit(1);
            }
            if (strcmp(msg.msg_text, "exit") == 0) {
                printf("\n상대방이 나갔습니다.\n");
                kill(getppid(), SIGKILL); // 부모 프로세스 종료
                exit(0);
            }
            printf("\r[상대방]: %s\n[나]: ", msg.msg_text);
            fflush(stdout);
        }
    } else {
        // 부모 프로세스: 메시지 송신 담당
        struct msg_buffer msg;
        // ID가 1이면 타입 1로 보내고(상대방이 2를 읽음), ID가 2면 타입 2로 보냄
        msg.msg_type = (id == 1) ? 1 : 2;

        while (1) {
            printf("[나]: ");
            if (fgets(msg.msg_text, MAX_TEXT, stdin) == NULL) break;
            
            // 개행 문자 제거
            msg.msg_text[strcspn(msg.msg_text, "\n")] = 0;

            if (msgsnd(msgid, &msg, sizeof(msg.msg_text), 0) == -1) {
                perror("msgsnd 실패");
                exit(1);
            }

            if (strcmp(msg.msg_text, "exit") == 0) {
                kill(pid, SIGKILL); // 자식 프로세스 종료
                break;
            }
        }
        // 마지막에 큐 삭제는 생략 (실제 구현시 한쪽에서 msgctl로 삭제 필요)
    }

    return 0;
}

