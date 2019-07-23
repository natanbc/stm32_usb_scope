#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const double voltsPerUnit = 0.00080566406;

int main(int argc, char* argv[]) {
    if(argc < 2) {
        fprintf(stderr, "Usage: %s <amount to average> [voltage divider ratio in percent]", argv[0]);
        exit(1);
    }
    int N = atoi(argv[1]);
    double dividerRatio = argc > 2 ? atof(argv[2])/100.0 : 1.0;
    double multiplier = 1.0/dividerRatio * voltsPerUnit;
    int eof = 0;
    while(!eof) {
        double sum = 0;
        int read = 0;
        for(int i = 0; i < N; i++) {
            int v;
            scanf("%d", &v);
            if(getchar() == EOF) {
                eof = 1;
                break;
            }
            read++;
            sum += v;
        }
        if(read == 0) {
            break;
        }
        printf("%lf\n", ((sum * multiplier) / read));
        fflush(stdout);
    }
    return 0;
}
