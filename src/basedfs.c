#include <linux/fs.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/inet.h>
#include <linux/net.h>
#include <linux/workqueue.h>
#include <linux/buffer_head.h>
#include <linux/mutex.h>
#include <asm/current.h> // current process information
#include <net/sock.h> //sockaddr_in

#define bufferSize 4096

static struct socket* serverSocket = NULL;
static struct socket* clientSocket = NULL;

struct workqueue_struct* wq;

struct wq_wrapper {
  struct work_struct worker;
  struct sock* sk;
} wq_data;

struct usrMsg {
	char* op;
	char* fid;
	char* pay;
};

struct mutex lock;

int readBusy = 0;

static char readBuffer[bufferSize];


void kernelDebug(const char* message) {
  printk(message);
  printk("\t-> User space pid: %i\n\t-> User space process: %s\n",
  current->pid, current->comm);
}

void openResponse(struct usrMsg* msg) {
  kernelDebug("OPEN_RESPONSE\n");
}

void readResponse(struct usrMsg* msg) {
  kernelDebug("READ_RESPONSE\n");
  memcpy(readBuffer, msg->pay, bufferSize);
  readBusy = 0;
}

void writeResponse(struct usrMsg* msg) {
  kernelDebug("WRITE_RESPONSE\n");
}

void kernelToUser(struct usrMsg* msg) {
  kernelDebug("KERNEL_TO_USER\n");

  if (strstr(msg->op, "open")) {
    openResponse(msg);
  }
  else if (strstr(msg->op, "read")) {
    readResponse(msg);
  }
  else if (strstr(msg->op, "write")) {
    writeResponse(msg);
  }
  else {
    printk(KERN_ERR "Not sure why msg->op is \"%s\"...\n", msg->op);
  }

}

void consumeDaemonPkg(char* daemonResponse, struct usrMsg* msg) {
  kernelDebug("CONSUME_DAEMON_PKG\n");

  char* token = NULL;
  char* delim = "\n";

  int count = 0;

  while (token = strsep(&daemonResponse, delim)) {
    printk("Token received: %s\n", token);
    switch (count++) {
      case 0:
        msg->op = token;
      break;
      case 1:
        msg->fid = token;
      break;
      case 2:
        msg->pay = token;
      break;
      default:
        printk("you fucked up!\n");
      break;
    }
  }
}


static int basedfs_mknod(struct inode* dir, struct dentry* dentry,
  unsigned short mode, dev_t dev);
struct inode* basedfs_make_inode(struct super_block* sb,
  const struct inode* dir, umode_t mode, dev_t dev);


static void callback(struct sock* sk, int bytes) {
  kernelDebug("CALLBACK\n");
  wq_data.sk = sk;
  queue_work(wq, &wq_data.worker);
}

void send_answer(struct work_struct* data) {
  struct wq_wrapper* foo = container_of(data, struct wq_wrapper, worker);
  kernelDebug("SEND_ANSWER\n");
  int len = 0;

  while((len = skb_queue_len(&foo->sk->sk_receive_queue)) > 0) {
    struct sk_buff* skb = NULL;
    skb = skb_dequeue(&foo->sk->sk_receive_queue);
    printk(KERN_DEBUG "Message length: %d\nMessage: %s\n", skb->len-8, skb->data+8);
    struct usrMsg* msg;
    consumeDaemonPkg(skb->data+8, msg);
    kernelToUser(msg);
  }
}


static inline struct basedfs_inode* BASEDFS_INODE(struct inode* inode) {
  kernelDebug("BASEDFS_INODE\n");
  return inode->i_private;
}

static int basedfs_open(struct inode* dir, struct file* filp) {
  kernelDebug("BASEDFS_OPEN\n");

  struct msghdr msg;
  struct iovec iov[2];
  mm_segment_t oldfs;
  struct sockaddr_in serverOut;

  int length = 0;

  char sendStr[512] = "";
 
  memset(&serverOut, 0, sizeof(serverOut));
  serverOut.sin_family = AF_INET;
  serverOut.sin_addr.s_addr = in_aton("127.0.0.1");
  serverOut.sin_port = htons(5003);

  iov[0].iov_base = "open\n";
  iov[0].iov_len = strlen("open\n");
  filp->private_data = (void*)dir->i_ino;

  sprintf(sendStr, "%lx", dir->i_ino);
  iov[1].iov_base = sendStr;
  iov[1].iov_len = 512;

  printk(KERN_DEBUG "The value in sendStr: %s, it is %d bytes long\n",
    sendStr, sizeof(sendStr));

  memset(&msg, 0, sizeof(msg));
  msg.msg_name = &serverOut;
  msg.msg_namelen = sizeof(sendStr);
  msg.msg_control = NULL;
  msg.msg_controllen = 0;
  msg.msg_iov = &iov[0];
  msg.msg_iovlen = sizeof(iov);

  oldfs = get_fs();
  set_fs(KERNEL_DS);
  length = sock_sendmsg(clientSocket, &msg, sizeof(msg));
  set_fs(oldfs);

  return 0;
}

static ssize_t basedfs_read(struct file* filp, char __user* buf, size_t len, loff_t* offset) {
  kernelDebug("BASEDFS_READ\n");

  unsigned long fileID;


  mutex_lock(&lock);
  readBusy = 1;

  memset(&readBuffer[0], 0, sizeof(readBuffer));

  struct msghdr msg;
  struct iovec iov;
  mm_segment_t oldfs;
  struct sockaddr_in serverOut;

  int length = 0;
  int busyCount = 0;

  char sendStr[512] = "";
 
  memset(&serverOut, 0, sizeof(serverOut));
  serverOut.sin_family = AF_INET;
  serverOut.sin_addr.s_addr = in_aton("127.0.0.1");
  serverOut.sin_port = htons(5003);

  fileID = (unsigned long) filp->private_data;

  sprintf(sendStr, "%s\n%lx", "read", fileID);

  iov.iov_base = sendStr;
  iov.iov_len = strlen(sendStr);

  memset(&msg, 0, sizeof(msg));
  msg.msg_name = &serverOut;
  msg.msg_namelen = sizeof(sendStr);
  msg.msg_control = NULL;
  msg.msg_controllen = 0;
  msg.msg_iov = &iov;
  msg.msg_iovlen = sizeof(iov);

  oldfs = get_fs();
  set_fs(KERNEL_DS);
  length = sock_sendmsg(clientSocket, &msg, sizeof(msg));
  set_fs(oldfs);

  while (readBusy) {
    printk(KERN_DEBUG "read is busy... (%d)\n", busyCount++);
  }
  copy_to_user(buf, readBuffer, bufferSize);
  *offset += len;

  mutex_unlock(&lock);

  return len;
}

static ssize_t basedfs_write(struct file* filp, char __user* buf, size_t len, loff_t* offset) {
  kernelDebug("BASEDFS_WRITE\n");

  unsigned long fileID;
  struct msghdr msg;
  struct iovec iov;
  mm_segment_t oldfs;
  struct sockaddr_in serverOut;

  int length = 0;
  char sendStr[512];

  if (copy_from_user(sendStr, buf, len)) {
    printk(KERN_ERR "FUCK!\n");
    return -EFAULT;
  }
 
  memset(&serverOut, 0, sizeof(serverOut));
  serverOut.sin_family = AF_INET;
  serverOut.sin_addr.s_addr = in_aton("127.0.0.1");
  serverOut.sin_port = htons(5003);

  fileID = (unsigned long) filp->private_data;

  sprintf(sendStr, "%s\n%lx\n%s", "write", fileID, buf);
  iov.iov_base = sendStr;
  iov.iov_len = 512;

  printk(KERN_DEBUG "YOU TRYNA WRITE: %s\n", sendStr);

  memset(&msg, 0, sizeof(msg));
  msg.msg_name = &serverOut;
  msg.msg_namelen = 512;
  msg.msg_control = NULL;
  msg.msg_controllen = 0;
  msg.msg_iov = &iov;
  msg.msg_iovlen = sizeof(iov);

  oldfs = get_fs();
  set_fs(KERNEL_DS);
  length = sock_sendmsg(clientSocket, &msg, sizeof(msg));
  set_fs(oldfs);

  return length;
}

static int basedfs_create(struct inode* dir, struct dentry* dent,
  unsigned short mode) {
  kernelDebug("BASEDFS_CREATE\n");
  return basedfs_mknod(dir, dent, mode | S_IFREG, 0);
}

static int basedfs_mkdir(struct inode* dir, struct dentry* dent,
  unsigned short mode) {
  kernelDebug("BASEDFS_MKDIR\n");
  d_add(dent, dir);
  return 0;
}

static int basedfs_mknod(struct inode* dir, struct dentry* dentry,
  unsigned short mode, dev_t dev) {
  kernelDebug("BASEDFS_MKNOD\n");
  struct inode* ret = basedfs_make_inode(dir->i_sb, dir, mode, dev);
  int err = -ENOSPC;
  if (ret) {
    d_instantiate(dentry, ret);
    dget(dentry);
    err = 0;
    dir->i_mtime = dir->i_ctime = CURRENT_TIME;
  }
  return err;
}

const struct file_operations basedfs_file_operations = {
  .open = basedfs_open,
  .read = basedfs_read,
  .write = basedfs_write,
  .fsync = noop_fsync,
};

const struct inode_operations basedfs_file_inode_operations = {
  .setattr = simple_setattr,
  .getattr = simple_getattr,
};

 const struct inode_operations basedfs_dir_inode_operations = {
  .create = basedfs_create,
  .lookup = simple_lookup,
  .unlink = simple_unlink,
  .rename = simple_rename,
  .mkdir = basedfs_mkdir,
  .mknod = basedfs_mknod,
};


struct inode* basedfs_make_inode(struct super_block* sb,
  const struct inode* dir, umode_t mode, dev_t dev) {
  kernelDebug("BASEDFS_MAKE_INODE\n");
  struct inode* inode = new_inode(sb);
  if (inode) {
    inode->i_ino = get_next_ino();
    inode_init_owner(inode, dir, mode);
    inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;
    switch (mode& S_IFMT) {
      case S_IFREG:
        inode->i_op = &basedfs_file_inode_operations;
        inode->i_fop = &basedfs_file_operations;
        kernelDebug("S_IFREG - Creating file inode\n");
        break;
      case S_IFDIR:
        inode->i_op = &basedfs_dir_inode_operations;
        inode->i_fop = &simple_dir_operations;
        kernelDebug("S_IFREG - Creating file inode\n");
        inc_nlink(inode);
        break;
      default:
        kernelDebug("DEFAULT CASE");
        break;
    }
  }
  return inode;
}

int basedfs_fill_super(struct super_block* sb, void* data, int silent) {
  kernelDebug("BASEDFS_FILL_SUPER\n");
  
  struct inode* rootInode;
  rootInode = basedfs_make_inode(sb, NULL, S_IFDIR | 0755, 0);

  sb->s_magic = 0x13371337;
  sb->s_root = d_make_root(rootInode);

  return (!sb->s_root) ? -ENOMEM : 0;
}

static struct dentry* basedfs_mount(struct file_system_type* fs_type,
  int flags, const char* dev_name, void* data) {
  kernelDebug("BASEDFS_MOUNT\n");
  return mount_bdev(fs_type, flags, dev_name, data, basedfs_fill_super);
}

struct file_system_type basedfs_fs_type = {
  .name = "basedfs",
  .mount = basedfs_mount,
  .kill_sb = kill_block_super,
};

static int __init basedfs_init(void) {
  struct sockaddr_in server;
  kernelDebug("BASEDFS_INIT\n");
  int error;
  if (sock_create(PF_INET, SOCK_DGRAM, IPPROTO_UDP, &serverSocket) < 0) {
      printk(KERN_ERR "DAMN!!! THE SERVER FUCKED UP!!!!\n");
      printk("\tUser space pid -> %i\n\tUser space process -> %s\n",
        current->pid, current->comm);
    return -EIO;
  }
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = in_aton("127.0.0.1");
  server.sin_port = htons(5002);
  error = serverSocket->ops->bind(serverSocket,  // The last two arguments are used as
    (struct sockaddr*) &server, sizeof(server)); // arguments to the callback function
  if (error) {
    sock_release(serverSocket);
    return -EIO;
  }
  serverSocket->sk->sk_data_ready = callback;
  INIT_WORK(&wq_data.worker, send_answer);
  wq = create_singlethread_workqueue("myworkqueue");
  if (!wq) {
    return -ENOMEM;
  }
  if (sock_create(PF_INET, SOCK_DGRAM, IPPROTO_UDP, &clientSocket) > 0) {
    printk(KERN_ERR "FUCKKKK!!!!! I COULDNT SET UP THE CLIEENT SOCKET!!");
    return -EIO;
  }
  register_filesystem(&basedfs_fs_type);
  return 0;
}

static void __exit basedfs_exit(void) {
  kernelDebug("BASEDFS_EXIT\n");
  if (serverSocket) {
    sock_release(serverSocket);
  }
  if (clientSocket) {
    sock_release(clientSocket);
  }
  if (wq) {
    flush_workqueue(wq);
    destroy_workqueue(wq);
  }
  unregister_filesystem(&basedfs_fs_type);
}

module_init(basedfs_init);
module_exit(basedfs_exit);

MODULE_DESCRIPTION("CmpE142 Toy Filesystem");
MODULE_AUTHOR("A.K., C.L., E.L., D.K., L.N.");
MODULE_LICENSE("GPL");