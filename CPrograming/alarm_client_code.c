
#include <stdio.h>
#include <stdlib.h>
#include <dbus/dbus.h>

int main(int argc, char *argv[]) {
    DBusConnection *conn;
    DBusError err;
    DBusMessage *msg;
    DBusMessageIter args;
    char *sigvalue = "안녕하세요! DBus 알림입니다.";

    if(argc > 1) sigvalue = argv[1];

    dbus_error_init(&err);
    conn = dbus_bus_get(DBUS_BUS_SESSION, &err);

    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "연결 에러: %s\n", err.message);
        dbus_error_free(&err);
        return 1;
    }

    // Signal 메시지 생성 (경로, 인터페이스, 시그널이름)
    msg = dbus_message_new_signal("/test/ipc/signal", "test.ipc.signal", "notify");
    if (msg == NULL) {
        fprintf(stderr, "메시지 NULL\n");
        exit(1);
    }

    // 인자 추가 (문자열)
    dbus_message_iter_init_append(msg, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &sigvalue)) {
        fprintf(stderr, "메모리 부족\n");
        exit(1);
    }

    // 메시지 전송
    if (!dbus_connection_send(conn, msg, NULL)) {
        fprintf(stderr, "메모리 부족\n");
        exit(1);
    }
    
    dbus_connection_flush(conn);
    printf("알림 전송 완료: %s\n", sigvalue);

    dbus_message_unref(msg);
    return 0;
}

