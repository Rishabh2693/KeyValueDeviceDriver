//////////////////////////////////////////////////////////////////////
//	Group Members: 	
//	Kshitij Patil (kspatil2 ; Student id : 200150642)
//	Rishabh Sinha (rsinha2 ; Student id : 001044877)		 
//
//////////////////////////////////////////////////////////////////////
//                             North Carolina State University
//
//
//
//                             Copyright 2016
//
////////////////////////////////////////////////////////////////////////
//
// This program is free software; you can redistribute it and/or modify it
// under the terms and conditions of the GNU General Public License,
// version 2, as published by the Free Software Foundation.
//
// This program is distributed in the hope it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
//
////////////////////////////////////////////////////////////////////////
//
//   Author:  Hung-Wei Tseng
//
//   Description:
//     Skeleton of KeyValue Pseudo Device
//
////////////////////////////////////////////////////////////////////////

#include "keyvalue.h"

#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/poll.h>
#include <linux/semaphore.h>
#include <linux/delay.h>
/* needed for __init,__exit directives */ 
#include <linux/init.h>
#include<linux/rwsem.h>
/* obviously, for kmalloc */ 
//#include <linux/malloc.h>

/* this header files wraps some common module-space operations ... 
   here we use mem_map_reserve() macro */ 
//#include <linux/wrapper.h> 

/* needed for virt_to_phys() */ 
#include <asm/io.h> // virt_to_phys()
 
struct rw_semaphore lock;
unsigned transaction_id;

struct NODE
{
	__u64 key;
	__u64 size;
	void *data;
	struct NODE *next;
};

struct NODE *head; 
 
static void free_callback(void *data)
{
}

// Function to send the data corresponding to the given parameter key
// returns transaction id
static long keyvalue_get(struct keyvalue_get __user *ukv)
{
	down_read(&lock);
	int i;
	struct keyvalue_get kv;
	struct NODE* temp;
	temp = head;
	if(temp == NULL)
	{
		up_read(&lock);
		return -1;
	}
	while(temp->key!= ukv->key)
	{
		if(temp->next == NULL)
		{
			up_read(&lock);	
			return -1;
		}
		temp = temp->next;
	}
	for(i=0;i<temp->size;i++)
                        *(char *)(ukv->data + i) = *(char *)(temp->data + i);
	ukv->size = temp->size;
	up_read(&lock);
        return transaction_id++;
}

// Function to save given key-value pair in the kernel memory
// returns transaction id 
static long keyvalue_set(struct keyvalue_set __user *ukv)
{
	
	down_write(&lock);
        int i;
	if(ukv->size > 4096)
	{
		up_write(&lock);
		 return -1;
	}
	struct keyvalue_set kv;
	struct NODE *temp;

	if(head==NULL)
	{
		head = (struct NODE*) kmalloc(sizeof(struct NODE), GFP_KERNEL);
		head->data = kmalloc(ukv->size, GFP_KERNEL);
		for(i=0;i<ukv->size;i++)
			*(char *)(head->data + i) = *(char *)(ukv->data + i); 
		(head->key)=ukv->key;
		(head->size)=(ukv->size);
		head->next=NULL;
	}
	else
	{
		struct NODE* temp1 = head;
                int flag = 0;
		while(temp1!=NULL)
		{
			if(temp1->key == ukv->key)
			{			
			flag =1;
			break;
			}
			temp1 = temp1->next;
		}
		if(flag == 1)
		{	
			for(i = 0;i<ukv->size;i++)
                        *(char *)(temp1->data + i) = *(char *)(ukv->data + i);
			 (temp1->size) = (ukv->size);
			up_write(&lock);	
        		return transaction_id++;	
		}	


		temp = head;
		while(temp->next!=NULL)
		{
			temp=temp->next;
		}
		temp->next = kmalloc(sizeof(struct NODE), GFP_KERNEL);
		temp->next->data =kmalloc(ukv->size, GFP_KERNEL);
		(temp->next->key) = ukv->key;
		for(i = 0;i<ukv->size;i++)
			*(char *)(temp->next->data + i) = *(char *)(ukv->data + i);
		(temp->next->size) = (ukv->size);
	
		temp->next->next=NULL;
		
	}
	up_write(&lock);
	return transaction_id++;
}

// Function to delete a key-value pair given its key
// returns transaction id
static long keyvalue_delete(struct keyvalue_delete __user *ukv)
{

	down_write(&lock);
	struct NODE* tem=head;
   	while(	tem!=NULL){
		tem = tem->next;
	}
	struct keyvalue_delete kv;
	int i;
        struct NODE* temp;
        temp = head;
	 if(temp == NULL)
        {
		up_write(&lock);
                return -1;
        }
	if(temp->key==ukv->key)
	{
	
		head = temp->next;
		kfree(temp);
		up_write(&lock);
		return transaction_id++;
	}
	struct NODE* prev;
        while(temp->key!= ukv->key)
        {
                if(temp->next == NULL)
		{
			up_write(&lock);
                	return -1;
		}
		prev = temp;
                temp = temp->next;
        }
	if(temp->next ==NULL)
       { 
		prev->next=NULL;
		kfree(temp);
	}
	else
	{
 		struct NODE* tempfree = temp;
		prev->next = temp->next;
		kfree(tempfree);	
	}
	up_write(&lock);
        return transaction_id++;

}

//Added by Hung-Wei
     
unsigned int keyvalue_poll(struct file *filp, struct poll_table_struct *wait)
{
    unsigned int mask = 0;
    printk("keyvalue_poll called. Process queued\n");
    return mask;
}

static long keyvalue_ioctl(struct file *filp, unsigned int cmd,
                                unsigned long arg)
{
    switch (cmd) {
    case KEYVALUE_IOCTL_GET:
        return keyvalue_get((void __user *) arg);
    case KEYVALUE_IOCTL_SET:
        return keyvalue_set((void __user *) arg);
    case KEYVALUE_IOCTL_DELETE:
        return keyvalue_delete((void __user *) arg);
    default:
        return -ENOTTY;
    }
}

static int keyvalue_mmap(struct file *filp, struct vm_area_struct *vma)
{
        return 0;   
}

static const struct file_operations keyvalue_fops = {
    .owner                = THIS_MODULE,
    .unlocked_ioctl       = keyvalue_ioctl,
    .mmap                 = keyvalue_mmap,
//    .poll		  = keyvalue_poll,
};

static struct miscdevice keyvalue_dev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "keyvalue",
    .fops = &keyvalue_fops,
};

static int __init keyvalue_init(void)
{
	init_rwsem(&lock);
    int ret;

    if ((ret = misc_register(&keyvalue_dev)))
        printk(KERN_ERR "Unable to register \"keyvalue\" misc device\n");
    return ret;
}

static void __exit keyvalue_exit(void)
{
    misc_deregister(&keyvalue_dev);
}

MODULE_AUTHOR("Hung-Wei Tseng <htseng3@ncsu.edu>");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
module_init(keyvalue_init);
module_exit(keyvalue_exit);
