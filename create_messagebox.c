#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <lab3/messagebox.h>

struct messageboxlist{
struct list_head list;
struct messagebox this_mb;
struct list_head sublist;
long sub_count;
};

struct subscriber_list{
int pid;
struct list_head list;
struct list_head mlist;
};

struct message{
struct list_head list;
int message_id;
char *message_text;
};

static int mb_id_global = 1;				//starts from 1 since 0 is broadcast
static LIST_HEAD(mb_list_head);


asmlinkage long sys_create_messagebox(struct messagebox *mb){
	struct messageboxlist *templist = kmalloc(sizeof(*templist), GFP_KERNEL);
	struct messagebox temp;

	copy_from_user(&temp, mb, sizeof(temp));			//copying messagebox passed by user
	temp.mb_id = mb_id_global;
	templist->this_mb = temp;

	INIT_LIST_HEAD(&(templist->list));
	list_add_tail(&(templist->list), &mb_list_head);
	INIT_LIST_HEAD(&(templist->sublist));
	templist->sub_count = 0;
	return mb_id_global++;
}


asmlinkage long sys_list_all_messageboxes(struct messagebox *mbs, int maxcount){
	struct messageboxlist *templist;
	int i =0;
	list_for_each_entry(templist, &mb_list_head, list){
		if(i < maxcount){
			copy_to_user(&mbs[i], &(templist->this_mb), sizeof(templist->this_mb));
			i++;
		}
	}
	return i;
}

asmlinkage long	sys_list_messageboxes_by_topic(char* topic, int topic_length, struct messagebox *mbs, int maxcount){
	struct messageboxlist *templist;
int i = 0;
char *substr = kmalloc(topic_length*(sizeof(char)),GFP_KERNEL);
copy_from_user(substr, topic, topic_length*(sizeof(char)));

	list_for_each_entry(templist, &mb_list_head, list){
		if(i < maxcount && (strstr(templist->this_mb.topic, substr) != NULL)){			//strstr returns null if no substring found
			copy_to_user(&mbs[i], &(templist->this_mb), sizeof(templist->this_mb));
			i++;
		}
	}
	return i;

}
asmlinkage long	sys_subscribe_to_messagebox(int mb_id, int pid){
struct messageboxlist *templist;
struct subscriber_list *new_sub = kmalloc(sizeof(*new_sub), GFP_KERNEL);
new_sub->pid = pid;
	list_for_each_entry(templist, &mb_list_head, list){
		if(templist->this_mb.mb_id == mb_id){
			INIT_LIST_HEAD(&(new_sub->mlist));
			templist->sub_count++;
			INIT_LIST_HEAD(&(new_sub->list));
			list_add_tail(&(new_sub->list), &(templist->sublist));
			return templist->this_mb.mb_id;
		}	
	}
	return -1;			//messagebox does not exist
}
asmlinkage long	sys_get_number_of_subscribers(int mb_id){
struct messageboxlist *templist;
	list_for_each_entry(templist, &mb_list_head, list){
		if(templist->this_mb.mb_id == mb_id){
			return templist->sub_count;
		}	
	}
	return -1;			//messagebox does not exist
}
asmlinkage long	sys_list_subscribers(int mb_id, int* pids, int maxcount){
	struct messageboxlist *templist;
	struct subscriber_list * tempsublist2;
	int *sendpids = kmalloc(maxcount*(sizeof(int)),GFP_KERNEL);
	int i = 0;
	list_for_each_entry(templist, &mb_list_head, list){
		if(templist->this_mb.mb_id == mb_id){
			list_for_each_entry(tempsublist2, &(templist->sublist), list){
				if(i < maxcount){
					sendpids[i] = tempsublist2->pid;
					i++;
				}
			}
		}
	}
	copy_to_user(pids, sendpids, maxcount*(sizeof(int)));
	return i;	
}
asmlinkage long	sys_send_message(int mb_id, char* message, int length, int pid){
	struct messageboxlist *templist;
	struct subscriber_list *tempsublist2;
	char* received = kmalloc(length*sizeof(char), GFP_KERNEL);
	struct message *sending = kmalloc(sizeof(*sending), GFP_KERNEL);
	sending->message_text = received;
	INIT_LIST_HEAD(&(sending->list));
	list_for_each_entry(templist, &mb_list_head, list){
		if(templist->this_mb.mb_id == mb_id){
			list_for_each_entry(tempsublist2, &(templist->sublist), list){
				if(pid == tempsublist2->pid){
					list_add_tail(&(sending->list), &(tempsublist2->mlist));
					return 0;
				}
			}
		}	
	}
	return -1;
					
					
					
			
}

asmlinkage long	sys_get_message(int mb_id, int mid, char *message, int maxlength){
	struct messageboxlist *templist;
	struct subscriber_list *tempsublist2;
	list_for_each_entry(templist, &mb_list_head, list){
		if(templist->this_mb.mb_id == mb_id){
			}

	}
return 0;
		
		
}
asmlinkage long	sys_get_list_of_messages(int mb_id, int* mids, int* length, int maxcount){
	struct messageboxlist *templist;
	struct subscriber_list *tempsublist2;
	int i = 0;
	struct message *sending;
	list_for_each_entry(templist, &mb_list_head, list){
		if(templist->this_mb.mb_id == mb_id){
			list_for_each_entry(tempsublist2, &(templist->sublist), list){
				if(i<maxcount){
				 
					list_add_tail(&(sending->list), &(tempsublist2->mlist));
					return 0;
				}
			}
		}	
	}
	return 0;
}



