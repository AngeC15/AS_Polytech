#include <stdio.h>
#include <string.h>



int get_hash(const char* s, const int n) {
    const int p = 31, m = 1e9 + 7;
    int hash = 0;
    long p_pow = 1;
    for(int i = 0; i < n; i++) {
        hash = (hash + (s[i] - 'a' + 1) * p_pow) % m;
        p_pow = (p_pow * p) % m;
    }
    return hash;
}
 
int run_hash() {
    char s[] = "this is a test";
    int n = strlen(s);
    printf("Hash of %s is %d\n", s, get_hash(s, n));
    return 0;
}