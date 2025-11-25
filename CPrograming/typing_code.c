#include <stdio.h>
#include <string.h>
#include <time.h>

#define NUM_SENTENCES 3
#define MAX_LEN 256

int main() {
    const char *sentences[NUM_SENTENCES] = {
        "테스트입니다..",
        "안녕하세요",
        "끝입니다"
    };

    char input[MAX_LEN];
    int total_errors = 0;
    int total_chars = 0;
    clock_t start, end;

    printf("=== 타자 연습 프로그램 ===\n");
    printf("화면에 표시되는 문장을 입력하세요.\n\n");

    start = clock(); // 시작 시간

    for (int i = 0; i < NUM_SENTENCES; i++) {
        printf("문장 %d: %s\n", i + 1, sentences[i]);
        printf("입력: ");

        fgets(input, sizeof(input), stdin);

        // 입력 문자열에서 개행 문자 제거
        input[strcspn(input, "\n")] = 0;

        int errors = 0;
        int len = strlen(sentences[i]);
        total_chars += len;

        for (int j = 0; j < len; j++) {
            if (input[j] != sentences[i][j]) {
                errors++;
            }
        }

        total_errors += errors;

        printf("이 문장에서 잘못 입력한 글자 수: %d\n\n", errors);
    }

    end = clock(); // 종료 시간
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC; // 초 단위
    double minutes = time_taken / 60.0;

    double wpm = (total_chars / 5.0) / minutes; // 평균 WPM 계산 (5글자 기준)
    
    printf("=== 연습 결과 ===\n");
    printf("총 잘못 입력한 글자 수: %d\n", total_errors);
    printf("평균 분당 타자수(WPM): %.2f\n", wpm);

    return 0;
}

