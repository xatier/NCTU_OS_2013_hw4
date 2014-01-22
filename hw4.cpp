#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <pthread.h>

const int num_thread = 5;

struct Instr {
    // T_12928 T Account_ID_5123 93
    // T_120741 B
    int to;
    int amount;
    int type;   // 0 for B, 1 for T
};

struct Account {
    int id;
    long long balance;
    int no_t;
    struct Instr *instr;
};

void init_acct (struct Account *acct, int id, long long balance, int no_t) {
    acct->id = id;
    acct->balance = balance;
    acct->no_t = no_t;
    acct->instr = (struct Instr *)malloc(no_t * sizeof(struct Instr));
}

void del_acct (struct Account *acct) {
    free(acct->instr);
}


void read_input (void) {
    FILE *fin;
    fin = fopen("transactions.txt", "r");
    if (!fin) {
        exit(1);
    }

    int no_c;
    fscanf(fin, "%d", &no_c);
    // printf("%d\n", no_c);
    //
    Account *user_list = (struct Account *)malloc(no_c *sizeof(struct Account));

    for(int noci = 0; noci < no_c; noci++) {
        // Account_ID_2819 1910 16
        char buf[100], buf2[100], buf3[100];
        int acct_id;
        int balance;
        int no_t;
        int amount;
        int to;
        char c[5];
        fscanf(fin, "%s%d%d ", buf, &balance, &no_t);
        sscanf((buf+11), "%d", &acct_id);
        init_acct(&user_list[noci], acct_id, balance, no_t);
        //printf("%d %d %d\n", acct_id, balance, no_t);
        for (int noti = 0; noti < no_t; noti++) {
            // T_12928 T Account_ID_5123 93
            // T_120741 B
            fgets(buf, 100, fin);
            if (strlen(buf) > 13) {
                sscanf(buf, "%s%s%s%d", buf2, c, buf3, &amount);
                sscanf((buf3+11), "%d", &to);
                user_list[noci].instr[noti].type = 1;
                user_list[noci].instr[noti].to = to;
                user_list[noci].instr[noti].amount = amount;
            }
            else {
                user_list[noci].instr[noti].type = 0;
            }
        }
    }

    // for(int noci = 0; noci < no_c; noci++) {
    //     printf("ID: %d [%lld]\n", user_list[noci].id, user_list[noci].balance);
    //     for (int noti = 0; noti < user_list[noci].no_t; noti++) {
    //         if (user_list[noci].instr[noti].type) {
    //             printf("T: to: %d -> amount: %d\n",
    //                    user_list[noci].instr[noti].to,
    //                    user_list[noci].instr[noti].amount);
    //         }
    //         else {
    //             printf("B!!!!!!!!!!\n");
    //         }
    //     }
    // }

    free(user_list);
    fclose(fin);
}
int main (void) {
    read_input();
    return 0;
}
