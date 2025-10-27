// كود C مولد للغة العربية
// =======================

#include <stdio.h>
#include <stdlib.h>

int main() {
    int عداد = 0;
    int t0 = 0;
    int t1 = 0;
    int t2 = 0;
    int t3 = 0;
    int t4 = 0;
    char* str_0 = "الرقم هو: ";
    char* str_1 = "انتهت الحلقة";

    t0 = 1;
    عداد = t0;
    L0:
    t1 = عداد;
    t2 = 5;
    /* CMP t1, t2 */
    if (t1 > t2) goto L1;
    printf("%s\n", str_0);
    printf("%d\n", عداد);
    t3 = 1;
    t1 = عداد;
    t4 = t1 + t3;
    عداد = t4;
    goto L0;
    L1:
    printf("%s\n", str_1);
    return 0;
}
