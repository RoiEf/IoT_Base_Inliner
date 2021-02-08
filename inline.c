#include <stdio.h>

int main(int argc, char **argv) {
    FILE *pInputFile = NULL;
    FILE *pCSSInput = NULL;
    FILE *pOutputFile = NULL;

    printf("Hello inline\n");

    if ((pInputFile = fopen("index.html", "r")) == NULL) {
        printf("Error opening input file\n");
        return 1;
    }

    if ((pOutputFile = fopen("html.h", "w")) == NULL) {
        printf("Error opening output file\n");
        fclose(pInputFile);
        return 1;
    }

    fclose(pInputFile);
    fclose(pOutputFile);
    printf("inline complete");

    return 0;
}