#include "wav.h"

static void manageArgs(int argc, char **argv, FILE **input, FILE **output, float *lvl) {

    int option;
    while ((option = getopt (argc, argv, "i:o:l:")) != -1) {
        float parsed_arg;
        switch (option) {

            case 'i':      
                (*input) = fopen(optarg, "r");
                if ((*input) == NULL) {
                    fprintf(stderr, "Couldn't open file %s", optarg);
                    exit(-1);
                }
                break;

            case 'o':
                (*output) = fopen(optarg, "a");
                break;

            case 'l':
                parsed_arg = atof(optarg);
                if (parsed_arg <= 10.0 && parsed_arg >= 0.0) {
                    (*lvl) = parsed_arg;
                    break;
                }

                fprintf(stderr, "Level flag must have a value bigger than 0.0 and lesser than 1.0\n");
                exit(-2);

            default:
                fprintf (stderr, "Usage: %s -l level -i input -o output\n", argv[0]);
	            exit(-3);
        }
    }
}

int main (int argc, char **argv) {

    FILE *file_input = stdin;
    FILE *file_output = stdout;
    float level = 1.0;
    wav_t wav;

    manageArgs(argc, argv, &file_input, &file_output, &level);
    readAudioData(&wav, file_input);
    wider(&wav, level);
    writeAudioData(&wav, file_output);

    return 1;
}