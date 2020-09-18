#include "wav.h"

static void manageArgs(int argc, char **argv, FILE **output, int indexs[],int *size) {
    int i;
    for (i = 1; i < argc; i++) {
      if (argv[i][0] == '-' && argv[i][1] == 'o') {
        if (!argv[i+1]) {
          fprintf(stderr, "Usage: %s arqs -o output\n", argv[0]);
          exit(-3);
        }
        (*output) = fopen(argv[i+1], "a");
        i++;
      } else {
        indexs[(*size)++] = i;
      }
    }
}

int main (int argc, char **argv) {

    FILE *file_output = stdout;
    wav_t *input_wavs;
    wav_t output_wav;
    int files_indexs[argc-1]; // Declared with a temp size
    int arg_number = 0;      // Relative "true" size

    manageArgs(argc, argv, &file_output, files_indexs, &arg_number);

    input_wavs = malloc(sizeof(wav_t) * arg_number);

    readWavs(input_wavs, arg_number, argv, files_indexs);

    concatWavs(input_wavs, arg_number, &output_wav);

    writeAudioData(&output_wav, file_output);


    return 1; 
}