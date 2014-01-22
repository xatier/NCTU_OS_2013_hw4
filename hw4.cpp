#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <pthread.h>

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

const int num_thread = 10;
struct Account *user_list;
struct Account *q[num_thread];
int get_last = 0;
int g_noc;
int mark[10] = {0};
pthread_mutex_t mutexsum;



struct Args {
    int ts;
    int u;
};

//void *go () {
void *go (void *arg) {
    int ts = ((struct Args *)arg)->ts;
    int u = ((struct Args *)arg)->u;

    struct Account *acct = q[u];

    pthread_mutex_lock (&mutexsum);
    mark[u]++;
    pthread_mutex_unlock (&mutexsum);

    if (acct->no_t > 0) {
        pthread_mutex_lock (&mutexsum);
        // pop one out
        Instr instr_ = acct->instr[0];
        for (int i = 1; i < acct->no_t; i++) {
            acct->instr[i-1] = acct->instr[i];
        }

        acct->no_t--;
        pthread_mutex_unlock (&mutexsum);


        // B
        if (instr_.type == 0) {
            pthread_mutex_lock (&mutexsum);
            mark[u]++;
            pthread_mutex_unlock (&mutexsum);
        }
        else {
            int to = instr_.to-1;
            int amount = instr_.amount;
            printf("%d u: %d / acct.id: %d -> %d [%d]\n", ts, u, acct->id, instr_.to, amount);
            if (acct->balance >= amount) {
                pthread_mutex_lock (&mutexsum);

                acct->balance -= amount;
                user_list[to].balance += amount;
                mark[u]++;

                pthread_mutex_unlock (&mutexsum);
            }
        }
    }

    pthread_exit(NULL);

}

int getone (void) {
    for (int i = get_last; i < g_noc; i++) {
        if (user_list[i].no_t != 0) {
            get_last = i;
            return i;
        }
    }
    return -1;
}

void run (void) {
    pthread_t threads[num_thread];
    void *status;
    pthread_attr_t attr;
    pthread_mutex_init(&mutexsum, NULL);
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);

    int ts = 0;
    get_last = 0;

    for (int i = 0; i < num_thread; i++) {
        q[i] = &user_list[i];
        mark[i] = -1;
    }


    int done = 0;

    while (!done) {
        ts++;
        for (int u = 0; u < num_thread; u++) {
            struct Args a;
            a.ts = ts;
            a.u = u;
            pthread_create(&threads[u], &attr, go, (void *)&a);
        }
        pthread_attr_destroy(&attr);

        // for (int i = 0; i < num_thread; i++) {
        //     pthread_join(threads[i], &status);
        // }


        for (int i = 0; i < num_thread; i++) {
            if (mark[i] == 0 || mark[i] > 2) {
                int g = getone();
                if (g == -1) {
                    done = 1;
                    break;
                }
                q[i] = &user_list[i];
                mark[i] = -1;
            }
        }
    }

    pthread_mutex_destroy(&mutexsum);
    pthread_exit(NULL);

}


void init_acct (struct Account *acct, int id, long long balance, int no_t) {
    acct->id = id;
    acct->balance = balance;
    acct->no_t = no_t;
    acct->instr = (struct Instr *)malloc(no_t * sizeof(struct Instr));
}

void del_acct (struct Account *acct) {
    free(acct->instr);
}


int cmp (const void *a, const void *b) {
    const struct Account *c = (struct Account *)a;
    const struct Account *d = (struct Account *)b;

    if (c->id < d->id) {
        return -1;
    }
    else if (c->id == d->id) {
        return 0;
    }
    else {
        return 1;
    }
}

void read_input (void) {
    FILE *fin;
    fin = fopen("transactions.txt", "r");
    if (!fin) {
        exit(1);
    }

    int no_c;
    fscanf(fin, "%d", &no_c);
    g_noc = no_c;
    // printf("%d\n", no_c);
    //
    user_list = (struct Account *)malloc(no_c *sizeof(struct Account));

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

    qsort(user_list, g_noc, sizeof(struct Account), cmp);
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

    fclose(fin);
}
int main (void) {
    read_input();
    run();
    for(int noci = 0; noci < g_noc; noci++) {
        printf("ID: %d [%lld]\n", user_list[noci].id, user_list[noci].balance);
    }

    free(user_list);
    return 0;
}
