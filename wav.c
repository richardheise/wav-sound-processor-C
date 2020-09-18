#include "wav.h"

void readAudioData(wav_t *wav_pointer,FILE *input) {

    fread(&wav_pointer->header, sizeof(wav_pointer->header), 1, input);
    wav_pointer->audio = malloc(sizeof(int16_t) * wav_pointer->header.data.SubChunk2Size/2);
    fread(wav_pointer->audio, 2, wav_pointer->header.data.SubChunk2Size, input);
}

//------------------------------------------------------------------//

void writeAudioData(wav_t *wav_pointer, FILE *output) {
    fwrite(&wav_pointer->header, sizeof(wav_pointer->header), 1, output);
    fwrite(wav_pointer->audio,sizeof(int16_t),wav_pointer->header.data.SubChunk2Size/2, output);
}

//==================================================================//

void changeVol(wav_t *wav_pointer, float lvl) {
    int i;
    long int test;
    for(i=0; i < wav_pointer->header.data.SubChunk2Size/2; i++) {
        test = wav_pointer->audio[i] * lvl;
        if (test > MAX_VOL) {
            test = MAX_VOL;
        } else if (test < -MAX_VOL) {
            test = -MAX_VOL;
        }
        wav_pointer->audio[i] = test;
    }
}

//====================================================================//

int16_t max(int16_t *array, long int size) {
    long int i;
    int16_t max;
    max = abs(array[0]);
    for (i=1; i < size; i++) {
        if(max < abs(array[i])) {
            max = array[i];
        }
    }
    return max;
}

//===================================================================//

void normalizer(wav_t *wav_pointer) {
    int16_t peak;
    int16_t diff;
    float level;
    peak = max(wav_pointer->audio, wav_pointer->header.data.SubChunk2Size/2);
    diff = (MAX_VOL-peak);
    level = (((diff*100)/MAX_VOL)/100.0) + 1;
    fprintf(stderr, "Normalizing with value %f\n", level);
    changeVol(wav_pointer, level);
}

//==================================================================//

void reverser(wav_t *wav_pointer, FILE *output) {
    int i;

    fwrite(&wav_pointer->header, sizeof(wav_pointer->header), 1, output);

    for (i=(wav_pointer->header.data.SubChunk2Size/2)-2; i >= 0; i -= 2) {
        fwrite(&wav_pointer->audio[i], sizeof(int16_t), 1, output);
        ++i;
        fwrite(&wav_pointer->audio[i--], sizeof(int16_t), 1, output);
    }
}

//=================================================================//

void echo(wav_t *wav_pointer, int time, float lvl) {
    int i;
    int delay = wav_pointer->header.fmt.Nchannels * wav_pointer->header.fmt.Sample_rate * time/1000;
    for (i=delay+1; i < wav_pointer->header.data.SubChunk2Size/2; i++) {
        wav_pointer->audio[i] += (lvl * wav_pointer->audio[i - delay]);
    }
}

//================================================================//

void wider(wav_t *wav_pointer, float k) {
    int i;
    int16_t diff;
    for (i = 0; i < wav_pointer->header.data.SubChunk2Size/2; i++) {
        diff = wav_pointer->audio[i+1] - wav_pointer->audio[i];
        wav_pointer->audio[i] -= k * diff; 
        ++i;
        wav_pointer->audio[i] += k * diff;
    }
}

//===============================================================//

void mixAudio(int16_t *inputA, uint32_t size, int16_t *outputA) {
    uint32_t i;
    int32_t test = 0;
    for (i = 0; i < size; i++) {
        test = outputA[i] + inputA[i];
        if (test > MAX_VOL) {
            test = MAX_VOL;
        } else if (test < -MAX_VOL) {
            test = -MAX_VOL;
        }

        outputA[i] = test;
    }
}

//==============================================================//

void mixWavs(wav_t *wavs, int size, wav_t *wav_out) {
    int i;
    int max = wavs[0].header.data.SubChunk2Size;

    wav_out->header = wavs[0].header;
    for (i = 1; i < size; i++) {
        if (wavs[i].header.data.SubChunk2Size > max) {
            max = wavs[i].header.data.SubChunk2Size;
            wav_out->header = wavs[i].header;
        }
    }
    wav_out->audio = malloc(sizeof(int16_t) * max/2);

    for (i = 0; i < size; i++) {
        mixAudio(wavs[i].audio, wavs[i].header.data.SubChunk2Size/2, wav_out->audio);
    }
} 

//============================================================//

void concatWavs(wav_t *wavs, int size, wav_t *wav_out) {
    int i;
    uint32_t currentPos = 0;
    
    wav_out->header = wavs[0].header;
    for (i = 1; i < size; i++) {
        wav_out->header.data.SubChunk2Size += wavs[i].header.data.SubChunk2Size;
        wav_out->header.RIFF.ChunkSize += wavs[i].header.RIFF.ChunkSize;
    }
    wav_out->audio = malloc(sizeof(int16_t) * wav_out->header.data.SubChunk2Size/2);

    for (i = 0; i < size; i++) {
        copyAudio(wavs[i].audio, wavs[i].header.data.SubChunk2Size/2, wav_out->audio, &currentPos);
        currentPos += 1;
    }
}

//===============================================================//

void copyAudio(int16_t *inputA, uint32_t size, int16_t *outputA, uint32_t *beg) {
    uint32_t i;

    for (i = 0; i < size; i++) {
        outputA[(*beg)+i] = inputA[i];
    }
    (*beg) += i;
} 

//=============================================================//

void readWavs(wav_t *wavs, int size, char **inputs, int indexs[]) {
    int i;
    FILE *input;
    
    for (i=0; i < size; i++) {
        input = fopen(inputs[indexs[i]], "r");
        if (!input) {
          fprintf(stderr, "Couldn't open file %s\n", inputs[indexs[i]]);
          exit(-2);
        }
        readAudioData(&wavs[i], input);
    }
} 