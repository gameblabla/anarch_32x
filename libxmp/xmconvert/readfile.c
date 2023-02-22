#include "readfile.h"
#include "output.h"
#include "xmfile.h"
/* #include "s3mfile.h" */
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define FILETYPE_S3M 1
#define FILETYPE_XM 2

globals_t globals = { };

int get_filetype(char *name)
{
    /* see what the name extension is */
    char *ext = rindex(name, '.');
    if (ext == 0)
        return -1;
    ext++;
    if (strncasecmp(ext, "s3m", 3) == 0) {
        return FILETYPE_S3M;
    }
    if (strncasecmp(ext, "xm", 2) == 0) {
        return FILETYPE_XM;
    }
    return -1;
}

char *arg0;

const char *const c_file_name = "module_structure.c";
const char *const s_file_name = "module_data.s";
const char *const h_file_name = "modules.h";

void print_usage()
{
    printf("\nxmconvert v1.2\n\n");
    printf("Program to convert XM (and S3M?) files to compilable format\n\n");
    printf("Usage: %s [OPTIONS]  FILE1 FILE2 ... \n", arg0);
    printf("OPTIONS are:\n");
    printf("-v              Verbose output\n");
    printf("-u              Convert sample data to unsigned (+128)\n");
    printf("-s              Use underscore in asm file labels\n");
    printf("-e              Include volume envelope in samples\n");
    printf("-h              Print this help and exit\n");
    printf("\nExamples:\n");
    printf("\t%s file1 file2...\n", arg0);
    printf("\t%s *.xm\n\n", arg0);
    printf("Output produced in %s %s and %s\n", c_file_name,
           s_file_name, h_file_name);
}

int main(int argc, char **argv)
{
    arg0 = argv[0];
    FILE *file;
    int filearg = 1;
    if (argc < 2) {
        print_usage();
        return 1;
    }
    int op;
    globals.use_unsigned = 0;
    while ((op = getopt(argc, argv, "vuse")) != -1) {
        switch (op) {
            case 'v':
                globals.verbose = 1;
                break;
            case 'u':
                globals.use_unsigned = 1;
                break;
            case 's':
                globals.use_underscore = 1;
                break;
            case 'e':
                globals.inc_envelope = 1;
                break;
            case 'h':
            case '?':
                print_usage();
                return 0;
            default:
                break;
        }
    }
    if (argv[optind] == 0) {
        print_usage();
        printf("Must specify an input file!\n");
        return 1;
    }
    filearg = optind;
    int type = get_filetype(argv[filearg]);
    FILE *output_s;
    FILE *output_c;
    FILE *output_h;
    if (type != -1) {
        output_s = fopen(s_file_name, "wb");
        output_c = fopen(c_file_name, "wb");
        output_h = fopen(h_file_name, "wb");
    }
    output_init_c(output_c);
    output_init_s(output_s);
    /* can use globbing to match all *.xm or whatever */
    globals.file_index = 0;
    globals.song_titles =
        (char **) malloc(sizeof(char *) * (argc - filearg));
    while (filearg < argc) {
        /* get file type */
        file = fopen(argv[filearg], "rb");
        ASSERT(file != 0);
        type = get_filetype(argv[filearg]);
        if (type == FILETYPE_S3M) {
            /* parse file etc */
            /* read_s3m(file); */
        } else if (type == FILETYPE_XM) {
            /* parse file etc */
            read_xm(file, output_s, output_c);
        } else {
            printf("File type not recognised\n");
        }
        globals.file_index++;
        fclose(file);
        filearg++;
    }
    output_module_table(output_c);
    output_modules_header(output_h);
    fclose(output_s);
    fclose(output_c);
    fclose(output_h);
    return 0;
}
