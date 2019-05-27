/* Duskul version 0.1.5,  2018.08.12,   Takeshi Ogihara, (C) 2018 */
/* Duskul version 1.0.2,  2019.03.27 */
#include <stdio.h>
#include "getitem.h"
#include "symset.h"
#include "identifiers.h"
#include "statements.h"
#include "evaluate.h"
#include "abort.h"
#define  VERSION    "1.0.2"

int main(int argc, char **argv)
{
    int code = 0;
    bool runflag = true;
    int ac = 1;
    while (ac < argc) {
        if (argv[ac][0] == '-') {
            switch(argv[ac][1]) {
            case 'p':
                runflag = false; break;
            case 'h':
            default:
                fprintf(stderr, "Duskul version " VERSION "\n");
                fprintf(stderr, "Usage: %s [options] input\n", argv[0]);
                fprintf(stderr,
                    "Options:  -p    Parse only\n"
                    "          -h    Show this help\n"
                );
                return 1;
            }
        }else
            break;
        ac++;
    }
    FILE *fp = NULL;
    if (ac >= argc)
        abortMessage("no source");
    if ((fp = fopen(argv[ac], "r")) == NULL) {
        fprintf(stderr, "Error: can't open: %s\n", argv[ac]);
        return 1;
    }
    stdtxin = getTextBuffer(fp);
    idtablesInitialize();
    statInitialize();
    symInitialize();
    int mainindex = parseProgram();
    freeTextBuffer(stdtxin);
    stdtxin = NULL;
    idtablesFree();
    if (!runflag)
        return code;

    if (mainindex < 0)
        abortMessage("no main");
    code = executeProgram(mainindex);
    return code;
}
