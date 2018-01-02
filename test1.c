#include "../uml/linux-4.9.51/arch/x86/include/generated/uapi/asm/unistd_64.h"
#include "../uml/linux-4.9.51/include/lab3/messagebox.h"
#include <stdlib.h>
#include <stdio.h>
int main(){
struct messagebox temp = {
.topic = "one two three",
.description = "nicedesc",
.len = 11
};
struct messagebox temp2 = {
.topic = "four five six",
.description = "notnicedesc",
.len = 11
};
long  i = syscall(__NR_create_messagebox, &temp);
long i2 = syscall(__NR_create_messagebox, &temp2);
printf("i = %li", i);
printf("i2 = %li", i2);
struct messagebox *mbs = malloc(10*sizeof(struct messagebox));
int x = 0;
long i3 = syscall(__NR_list_all_messageboxes, mbs, 10);
printf("mbs[0].topic = %s\n", mbs[0].topic);
printf("mbs[0].mb_id = %d\n", mbs[0].mb_id);
printf("mbs[0].len = %d\n", mbs[0].len);
printf("mbs[0].description = %s\n", mbs[0].description);

printf("mbs[1].topic = %s\n", mbs[1].topic);
printf("mbs[1].description = %s\n", mbs[1].description);
printf("mbs[2].topic = %s\n", mbs[2].topic);
printf("mbs[2].description = %s\n", mbs[2].description);
free(mbs);
struct messagebox *mbs2 = malloc(10*sizeof(struct messagebox));
char topic[10] = "two";
printf("looking for %s", topic);
long i4 = syscall(__NR_list_messageboxes_by_topic, topic, 10, mbs2, 3);
printf("mbs2[0].topic = %s\n", mbs2[0].topic);
free(mbs2);
long i5 = syscall(__NR_subscribe_to_messagebox, 1, 0);
i5 = syscall(__NR_subscribe_to_messagebox, 1, 1);
long i6 = syscall(__NR_get_number_of_subscribers, 0);
long i7 = syscall(__NR_get_number_of_subscribers, 1);
printf("subcount to mb0 = %li \n", i6);
printf("subcount to mb1 = %li \n", i7);
int* pids = malloc(10*sizeof(int));
long i8 = syscall(__NR_list_subscribers, 1, pids, 10);
printf("subs found %li\n", i8);
int j = 0;
printf("subs to mb1:\n");
printf("pid = %d\n", pids[0]);
printf("pid = %d\n", pids[1]);
free(pids);
}
