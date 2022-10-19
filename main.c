#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "8seg.h"

void delay_1s(void);
void write_time(int hr, int min, int sec);

int main(int argc, char *argv[])
{
    int c;
    bool stopwatch;
    bool quiet;
    char *hr_str, *min_str, *sec_str;
    int hr, min, sec;
    
    quiet = false;
    stopwatch = false;
    hr_str = min_str = sec_str = NULL;
    opterr = 0;

    while((c = getopt (argc, argv, "Sqh:m:s:")) != -1) {
        switch(c) {
            case 'S':
                stopwatch = true;
                break;
            case 'q':
                quiet = true;
                break;
            case 'h':
                hr_str = optarg;
                break;
            case 'm':
                min_str = optarg;
                break;
            case 's':
                sec_str = optarg;
                break;
            case '?':
                if(strchr("hms", optopt)) {
                    fprintf (stderr, "Missing argument for -%c\n", optopt);
                    return 1;
                }
                break;
            default:
                abort ();
        }
    }

    if(!stopwatch) {
        hr = hr_str ? atoi(hr_str) : 0;
        min = min_str ? atoi(min_str) : 0;
        sec = sec_str ? atoi(sec_str) : 0;
        
        min += sec / 60;
        hr += min / 60;
        sec %= 60;
        min %= 60;

        if(hr > 99) {
            hr = 99;
            min = 59;
            sec = 59;
        }
    } else {
        hr = min = sec = 0;
    }
    
    /* skip until the next full second before starting */
    delay_1s();
    
    if(!stopwatch) {
        /* timer mode */
        while(hr || min || sec) {
            write_time(hr, min, sec);
            
            delay_1s();
            
            if(!sec) {
                sec = 59;
                if(!min) {
                    min = 59;
                    hr--;
                } else {
                    min--;
                }
            } else {
                sec--;
            }
        }
        
        for(;;) {
            delay_1s();
            write_time(0, 0, 0);
            if(!quiet)
                putchar('\a');

            delay_1s();
            printf("\033[0F\033[0J");
            fflush(stdout);
            if(!quiet)
                putchar('\a');
        }
    } else {
        /* stopwatch mode */
        for(;;) {
            write_time(hr, min, sec);
            delay_1s();
            if(sec == 59) {
                sec = 0;
                if(min == 59) {
                    min = 0;
                    hr++;
                } else {
                    min++;
                }
            } else {
                sec++;
            }
            
            if(hr == 99 && min == 59 && sec == 59) {
                for(;;) {
                    write_time(hr, min, sec);
                    delay_1s();
                    printf("\033[0F\033[0J");
                    fflush(stdout);
                    delay_1s();
                }
            }
        }
    }
    return 0;
}

void write_time(int hr, int min, int sec)
{
    for(int i = 0; i < CHAR_HEIGHT; i++) {
        printf("%s%s%s%s%s%s%s%s\n", 
                DIGIT_STRINGS[hr/10][i], DIGIT_STRINGS[hr%10][i], 
                COLON_STR[i],
                DIGIT_STRINGS[min/10][i], DIGIT_STRINGS[min%10][i], 
                COLON_STR[i],
                DIGIT_STRINGS[sec/10][i], DIGIT_STRINGS[sec%10][i], 
                COLON_STR[i]);
    }
    printf("\033[6F");
}

void delay_1s(void)
{
    time_t t;
    t = time(NULL);
    while(time(NULL) < t + 1);
}
