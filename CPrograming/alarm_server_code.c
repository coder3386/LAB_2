#include <stdio.h>
#include <stdlib.h>
#include <dbus/dbus.h>

int main() {
    DBusConnection *conn;
    DBusError err;

    dbus_error_init(&err);

    // Session Bus에 연결
    conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "연결 에러: %s\n", err.message);
        dbus_error_free(&err);
        return 1;
    }

    // Well-known name 요청 (선택 사항이나 식별을 위해 좋음)
    int ret = dbus_bus_request_name(conn, "test.ipc.server", DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Name Error: %s\n", err.message);
        dbus_error_free(&err);
        return 1;
    }

    // 수신할 메시지 필터 추가 (Signal 타입)
    dbus_bus_add_match(conn, "type='signal',interface='test.ipc.signal'", &err);
    dbus_connection_flush(conn);

    printf("알림 메시지 대기 중...\n");

    while (1) {
        dbus_connection_read_write(conn, 0);
        DBusMessage *msg = dbus_connection_pop_message(conn);

        if (msg == NULL) {
            continue;
        }

        if (dbus_message_is_signal(msg, "test.ipc.signal", "notify")) {
            char *sigvalue;
            if (dbus_message_get_args(msg, &err, DBUS_TYPE_STRING, &sigvalue, DBUS_TYPE_INVALID)) {
                printf("수신된 알림: %s\n", sigvalue);
            }
        }
        dbus_message_unref(msg);
    }

    return 0;
}

