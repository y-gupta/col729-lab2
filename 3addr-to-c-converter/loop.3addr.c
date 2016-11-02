#include <stdio.h>
#define WriteLine() printf("\n");
#define WriteLong(x) printf(" %lld", x);
#define ReadLong(a) if (fscanf(stdin, "%lld", &a) != 1) a = 0;
long long global_variables[32768/8];
void *GP = global_variables;
void function_label_3()
{
long long local_vars[4];
void *FP = &(local_vars[4]);
// Scalar local variable declared below
long long a;
long long c;
long long b;
long long e;
long long d;
long long f;
long long n;
long long x;
label_4: ; n /* -28 */ = 13;
label_5: ; x /* -32 */ = 0;
label_6: ; a /* -4 */ = 0;
label_7: ; long long register_7 = (a /* -4 */ < n /* -28 */);
label_8: ; if (!(register_7)) goto label_45;
label_9: ; b /* -8 */ = 0;
label_10: ; x /* -32 */ = 0;
label_11: ; long long register_11 = (b /* -8 */ < n /* -28 */);
label_12: ; if (!(register_11)) goto label_42;
label_13: ; c /* -12 */ = 0;
label_14: ; long long register_14 = (c /* -12 */ < n /* -28 */);
label_15: ; if (!(register_14)) goto label_39;
label_16: ; d /* -16 */ = 0;
label_17: ; long long register_17 = (d /* -16 */ < n /* -28 */);
label_18: ; if (!(register_17)) goto label_36;
label_19: ; e /* -20 */ = 0;
label_20: ; long long register_20 = (e /* -20 */ < n /* -28 */);
label_21: ; if (!(register_20)) goto label_33;
label_22: ; f /* -24 */ = 0;
label_23: ; long long register_23 = (f /* -24 */ < n /* -28 */);
label_24: ; if (!(register_23)) goto label_30;
label_25: ; long long register_25 = (x /* -32 */ + 1);
label_26: ; x /* -32 */ = register_25;
label_27: ; long long register_27 = (f /* -24 */ + 1);
label_28: ; f /* -24 */ = register_27;
label_29: ; goto label_23;
label_30: ; long long register_30 = (e /* -20 */ + 1);
label_31: ; e /* -20 */ = register_30;
label_32: ; goto label_20;
label_33: ; long long register_33 = (d /* -16 */ + 1);
label_34: ; d /* -16 */ = register_33;
label_35: ; goto label_17;
label_36: ; long long register_36 = (c /* -12 */ + 1);
label_37: ; c /* -12 */ = register_36;
label_38: ; goto label_14;
label_39: ; long long register_39 = (b /* -8 */ + 1);
label_40: ; b /* -8 */ = register_39;
label_41: ; goto label_11;
label_42: ; long long register_42 = (a /* -4 */ + 1);
label_43: ; a /* -4 */ = register_42;
label_44: ; goto label_7;
label_45: ; WriteLong(x /* -32 */);
label_46: ; WriteLine();
label_47: ; }
int main() { function_label_3(); return 0; }
