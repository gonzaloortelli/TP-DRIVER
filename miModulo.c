#include <linux/ctype.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
void encryptCaesar(char *msg, char *dest);

#define SUCCESS 0
#define DEVICE_NAME "SORII"
#define BUF_LEN 80
#define LENGHT_ALPHABET 26
#define INIT_LOWERCASE_ALPHABET 65
#define INIT_UPPERCASE_ALPHABET 97

static int Major;
static int Device_Open = 0;
static char msg[BUF_LEN];
static char *msg_Ptr;
static char *msg_Caesar;
const char *lowercase = "abcdefghijklmnopqrstuvwxyz";
const char *uppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

int init_module(void) {
    Major = register_chrdev(0, DEVICE_NAME, &fops);
    if (Major < 0) {
        printk(KERN_ALERT "Registering char device failed with %d\n", Major);
        return Major;
    }
    printk(KERN_INFO "I was assigned major number %d. To talk to\n", Major);
    printk(KERN_INFO "the driver, create a dev file with\n");
    printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);
    printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
    printk(KERN_INFO "the device file.\n");
    printk(KERN_INFO "Remove the device file and module when done.\n");

    return SUCCESS;
}

void cleanup_module(void) {
    unregister_chrdev(Major, DEVICE_NAME);
    /*if (ret < 0) {
        printk(KERN_ALERT "Error in unregister_chrdev: %d\n", ret);
    }*/
}

static int device_open(struct inode *inode, struct file *file) {
    if (Device_Open) {
        return -EBUSY;
    }
    Device_Open++;
    msg_Ptr = msg;
    try_module_get(THIS_MODULE);

    return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file) {
    Device_Open--;
    module_put(THIS_MODULE);

    return SUCCESS;
}

static ssize_t device_read(struct file *file, char *buffer, size_t length, loff_t *offset) {
    int bytes_read = 0;
    if (*msg_Ptr == 0) {
        return 0;
    }
    while (length && *msg_Ptr) {
        put_user(*(msg_Ptr++), buffer++);
        length--;
        bytes_read++;
    }
    return bytes_read;
}

static ssize_t device_write(struct file *file, const char __user *buffer, size_t length, loff_t *offset) {
    int i;
    for (i = 0; (i < length) && (i < BUF_LEN); i++) {
        get_user(msg[i], buffer+i);
    } 
    msg_Caesar = msg;   
    encryptCaesar(msg, msg_Caesar);
    msg_Ptr = msg_Caesar;
    return i;
}

void encryptCaesar(char *msg, char *msgCaesar) {
  int i = 0;
  while (msg[i]) {
    char actualChar = msg[i];
    int origPos = (int) actualChar; 
    if (!isalpha(actualChar)) {
      msgCaesar[i] = actualChar;
      i++;
      continue; 
    }
    if (isupper(actualChar)) {
      msgCaesar[i] = uppercase[(origPos - INIT_UPPERCASE_ALPHABET + 1) % LENGHT_ALPHABET];
    } else {
      msgCaesar[i] = lowercase[(origPos - INIT_UPPERCASE_ALPHABET + 1) % LENGHT_ALPHABET];
    }
    i++;
  }
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gonzalo Ortelli");
MODULE_DESCRIPTION("Driver para SORII");