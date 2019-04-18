BEGIN {
    printf("OBJ =")
    n = 1
}
{
    if ( n == 0 ) printf("   ");
    printf(" %s", $1)
    if (++n >= 5) {
        printf(" \\\n");
        n = 0
    }
}
END { if ( n > 0 ) printf("\n") }
