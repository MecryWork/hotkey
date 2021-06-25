#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/input.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

static bool bDelete = false;
static bool bAlt = false;
static bool bControl = false;

void simulate_key(int fd, int kval)

{
    struct input_event event;
    gettimeofday(&event.time, 0);
    //按下kval键
    event.type = EV_KEY;
    event.value = 1;
    event.code = kval;
    write(fd, &event, sizeof(event));
    //同步，也就是把它报告给系统
    event.type = EV_SYN;
    event.value = 0;
    event.code = SYN_REPORT;
    write(fd, &event, sizeof(event));
    memset(&event, 0, sizeof(event));
    gettimeofday(&event.time, 0);
    //松开kval键
    event.type = EV_KEY;
    event.value = 0;
    event.code = kval;
    write(fd, &event, sizeof(event));
    //同步，也就是把它报告给系统
    event.type = EV_SYN;
    event.value = 0;
    event.code = SYN_REPORT;
    write(fd, &event, sizeof(event));
}

void getKey(unsigned short key, int event)
{
    if (key == 111) {
        if (event == 1) {
            bDelete = true;
            printf("Delete true\n");
        } else if (event == 0) {
            printf("Delete false\n");
            bDelete = false;
        }
    }
    if (key == 29) {
        if (event == 1) {
            bControl = true;
            printf("Control true\n");
        } else if (event == 0) {
            printf("Control false\n");
            bControl = false;
        }
    }
    if (key == 56) {
        if (event == 1) {
            printf("bAlt true\n");
            bAlt = true;
        } else if (event == 0) {
            printf("bAlt false\n");
            bAlt = false;
        }
    }
}

void *listenKey(void *ptr)
{
    while (1) {
        if (bDelete && bControl && bAlt) {
            bDelete = false;
            bControl = false;
            bAlt = false;
            system("su mecry -c \"VBoxManage controlvm Windows10 keyboardputscancode 1d 38 53 9d "
                   "b8 d3\"");//调用virtualbox写入ctrl+alt+delete键，su中的mecry必须是当前virtualbox的用户

            int fd_kbd = -1;
            fd_kbd = open("/dev/input/event3", O_RDWR);
            if (fd_kbd <= 0)
            {
                printf("Can not open keyboard input file\n");
            }else {
                printf("Send keyboard to uos right control\n");
                simulate_key(fd_kbd,97);
            }
            printf("success get key!!!!!!!!!!\n");
        }
    }
}

void listen_device(const char *dev, int timeout)
{
    pthread_t thread1;

    int ret = pthread_create(&thread1, NULL, listenKey, NULL);
    if (ret != 0) {
        printf("线程1创建失败\n");
    } else {
        printf("线程1创建成功\n");
    }
    ret = pthread_detach(thread1);
    if (0 == ret) {
        printf("线程1分离成功\n");
    } else {
        printf("线程1分离失败\n");
    }

    int retval;
    fd_set readfds;
    struct timeval tv;

    int fd = open(dev, O_RDONLY);

    struct input_event event;

    if (fd < 0) {
        perror(dev);
        return;
    }

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);
        tv.tv_sec = timeout;
        tv.tv_usec = 0;
        if ((retval = select(fd + 1, &readfds, NULL, NULL, &tv)) == 1) {
            if (read(fd, &event, sizeof(event)) == sizeof(event)) {
                if (event.type == EV_KEY) {
                    if (event.value == 0 || event.value == 1) {
                        getKey(event.code, event.value);
                        printf("key %d %s\n", event.code, event.value ? "Pressed" : "Released");
                    }
                } else {
                    printf("type=%x %d %d\n", event.type, event.code, event.value);
                }
            }
        } else {
            break;
        }
    }
    close(fd);
}

void listen_keyboard(int timeout)
{
    listen_device("/dev/input/event3", timeout);
}

int main(int argc, char *argv[])
{
    listen_keyboard(20);
    printf("mouse timeout\n");
    return 0;
}
