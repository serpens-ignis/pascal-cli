#include <pulse/sample.h>
#include <pulse/simple.h>
#include <stdlib.h>
#include <threads.h>

#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"

#define KEY_UP 65
#define KEY_DOWN 66

extern unsigned char pascal_ogg[];
extern unsigned int pascal_ogg_len;

const char *pascal = "\n\
J?~      :7?\n\
::75. ..557:\n\
.:J&Y7:^&&P^.\n\
~~~?PP5PG5~^:.\n\
5Y7?PB&#YJ!!!~.\n\
5PGB#BJ!~~7!^:::\n\
:..:~!J~::^:::::::.\n\
   :.^?YYYJ~^:::::::....\n\
   :~7P#&&BJ^:::!~^^^^:::::::::....\n\
   :JYP#@@@B^:~YG5::^~!!::^~~^^^::::..\n\
   .GPJG@@@&Y5&@@G~J?Y##YYP#BGJ:^^::::.\n\
   .&B!!GB&@@@&G@&#@775&&&&GJ5B~^^^::::.\n\
   .&&P.~7YBBGJ!G#@&J7^YYJ?~:!BB!^::::^!\n\
   .&@G^..:!!~::J5@@#5.:::   :J&#G?^::?G\n\
   .&@G7:.. ....^7G@@B~...    :5&@&G^^?#\n\
   .&@G!..       .?#@&5.:.     :YG@BJ!:J\n\
   :&&57          .7B#G^:.      .^JPPJ\n\
  .7BG?~            !JP5:.        .75Y!\n\
  .?5Y!              ^55!^          ?YJ\n\
  .55J:              .?57~          :!Y\n\
 :?5?~               :YP7~           .!\n\
 ";

const char *asmcode = "\n\
RESET:\n\
    SEI\n\
    CLC\n\
    XCE\n\
    CLD\n\
\n\
    X16\n\
    M8\n\
    LDX #1FFFH\n\
    TXS\n\
\n\
    STZ NMITIME\n\
    LDA #BLANKING\n\
    STA INIDSP\n\
\n\
    BJSR ATLUS\n\
\n\
    'EL ELOHIM ELOHO ELOHIM SEBAOTH'\n\
    'ELION EIECH ADIER EIECH ADONAI'\n\
    'JAH SADAI TETRAGRAMMATON SADAI'\n\
    'AGIOS O THEOS ISCHIROS ATHANATOS'\n\
    'AGLA AMEN'\n\
\n\
    \
";

short *bgm;
int samples, channels, sample_rate;

void load_bgm(void) {
    samples = stb_vorbis_decode_memory(
        pascal_ogg, pascal_ogg_len, &channels, &sample_rate, &bgm
    );
}

void play_bgm(void) {
    load_bgm();
    pa_sample_spec ss = {
        .channels = channels,
        .format = PA_SAMPLE_S16NE,
        .rate = sample_rate,
    };
    pa_simple *s = pa_simple_new(
        NULL,
        "pascal-cli",
        PA_STREAM_PLAYBACK,
        NULL,
        "Music",
        &ss,
        NULL,
        NULL,
        NULL
    );
    pa_simple_write(s, bgm, samples, NULL);
    pa_simple_free(s);
    free(bgm);
}

void wait(int dt_ms) {
    nanosleep(&(const struct timespec) { .tv_nsec = dt_ms * 1000000 }, NULL);
}

void clear_screen(void) {
    printf("\n\033[2J\033[1;1H");
    fflush(stdout);
}

void move_cursor(int x, int y) {
    printf("\033[%d;%dH", y, x);
    fflush(stdout);
}

void move_cursor_right(int n) {
    printf("\033[%dC", n);
    fflush(stdout);
}

void hide_cursor(void) {
    printf("\033[?25l");
    fflush(stdout);
}

void show_cursor(void) {
    printf("\033[?25h");
    fflush(stdout);
}

void save_cursor(void) {
    printf("\033[ 7");
    fflush(stdout);
}

void restore_cursor(void) {
    printf("\033[ 8");
    fflush(stdout);
}

void scroll_up(int n, int dt_ms) {
    for (int i = 0; i < n; i++) {
        wait(dt_ms);
        printf("\033[1S");
        printf("\033[1A");
        fflush(stdout);
    }
}

void print(const char *str, int dt_ms) {
    while (*str != 0) {
        wait(dt_ms);
        putchar(*str);
        str++;
        int skip = 0;
        while (*str == '\n') {
            str++;
            putchar('\n');
        }
        while (*str == ' ') {
            str++;
            skip++;
        }
        if (skip) {
            move_cursor_right(skip);
        }
        fflush(stdout);
    }
}

void erase(int n) {
    printf("\033[%dD", n);
    printf("\033[0K");
    fflush(stdout);
}

void draw_window(int w, int h) {
    printf("╔");
    for (int i = 0; i < w; i++) {
        printf("═");
    }
    printf("╗\033[%dD\033[1B", w + 2);

    for (int i = 0; i < h; i++) {
        printf("║");
        for (int j = 0; j < w; j++) {
            putchar(' ');
        }
        printf("║\033[%dD\033[1B", w + 2);
    }

    printf("╚");
    for (int i = 0; i < w; i++) {
        printf("═");
    }
    printf("╝");

    fflush(stdout);
}

void draw_window_slow(int w, int h) {
    for (int i = 0; i <= h; i++) {
        draw_window(w, i);
        printf("\033[%dD\033[%dA", w + 2, i + 1);
        wait(100);
    }
}

void draw_selection_prompt(int selection) {
    move_cursor(72, 3);
    if (selection == 0) {
        printf("\033[7m");
        print("Yes", 40);
        printf("\033[0m");
        move_cursor(72, 4);
        print("No", 40);
    } else {
        print("Yes", 40);
        move_cursor(72, 4);
        printf("\033[7m");
        print("No", 40);
        printf("\033[0m");
    }
}

void smt_intro(void) {
    print(asmcode, 40);
    scroll_up(12, 100);

    print("SHIN·", 200);
    wait(400);
    erase(5);

    printf("新·");
    fflush(stdout);
    wait(400);
    erase(3);

    printf("神·");
    fflush(stdout);
    wait(400);
    erase(3);

    printf("真·");
    fflush(stdout);
    wait(200);

    print("MEGAMI...TENSEI....", 40);
    wait(200);

    erase(sizeof("MEGAMI...TENSEI...."));
    printf("女神転生");
    fflush(stdout);
    wait(500);
}

void meet_pascal(void) {
    print(pascal, 2);

    move_cursor(45, 2);
    draw_window_slow(24, 2);
    move_cursor(46, 3);
    print(">Pascal the dog is here.", 40);
    move_cursor(46, 4);
    print(" Will you talk to him?", 40);
    move_cursor(71, 2);
    draw_window_slow(3, 2);

    system("stty raw");
    int selection = 0;
    draw_selection_prompt(selection);
    while (1) {
        char c = getchar();
        if (c == '\033' && getchar() == '\133') {
            switch (getchar()) {
            case KEY_UP:
                selection = 0;
                draw_selection_prompt(selection);
                break;
            case KEY_DOWN:
                selection = 1;
                draw_selection_prompt(selection);
                break;
            default:
                break;
            }
        } else if (c == '\r') {
            break;
        }
    }
    system("stty cooked");

    move_cursor(46, 3);
    printf("                        ");
    move_cursor(46, 4);
    printf("                        ");
    fflush(stdout);
    move_cursor(46, 3);
    print("Pascal the dog:", 40);
    move_cursor(46, 4);
    if (selection == 0) {
        print("Bowwow! Bowwow!", 40);
    } else {
        print("*whimper*", 40);
    }
    wait(999);
}

int main(void) {
    system("stty -echo");

    clear_screen();
    smt_intro();
    clear_screen();

    thrd_t audio_thread;
    thrd_create(&audio_thread, (thrd_start_t) play_bgm, (void *) 1);

    hide_cursor();
    meet_pascal();
    show_cursor();
    clear_screen();

    return 0;
}
