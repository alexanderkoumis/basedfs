#include <linux/fs.h>
#include <net/sock.h> //sockaddr_in
#include <linux/inet.h>
#include <linux/net.h>
#include <linux/workqueue.h>

static struct socket *udpsocket = NULL;
static struct socket *clientsocket=NULL;
struct workqueue_struct* wq;

struct wq_wrapper{
  struct work_struct worker;
  struct sock * sk;
};

struct wq_wrapper wq_data;

static void callback(struct sock* sk, int bytes) {
  wq_data.sk = sk;
  queue_work(wq, &wq_data.worker);
}

void send_answer(struct work_struct *data){
  struct wq_wrapper* foo = container_of(data, struct wq_wrapper, worker);
  int len = 0;

  while((len = skb_queue_len(&foo->sk->sk_receive_queue)) > 0) {
    struct sk_buff* skb = NULL;
    unsigned short* port;
    struct msghdr msg;
    struct iovec iov;
    mm_segment_t oldfs;
    struct sockaddr_in serverOut;

    skb = skb_dequeue(&foo->sk->sk_receive_queue);
    printk("Message length: %d\nMessage: %s\n", skb->len-8, skb->data+8);
    memset(&serverOut, 0, sizeof(serverOut));
    serverOut.sin_family = AF_INET;
    serverOut.sin_addr.s_addr = in_aton("127.0.0.1");
    port = (unsigned short*)skb->data;
    serverOut.sin_port = *port;
    memset(&msg, 0, sizeof(msg));

    iov.iov_base = skb->data+8;
    iov.iov_len = skb->len-8;
    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    oldfs = get_fs();
    set_fs(KERNEL_DS);
    len = sock_sendmsg(clientsocket, &msg, skb->len-8);
    set_fs(oldfs);
    kfree_skb(skb);
  }

}

static ssize_t basedfs_write(struct file* filp, char __user* buf, size_t len, loff_t* offset) {
  return 0;
}

static ssize_t basedfs_read(struct file* filp, char* buf, size_t count, loff_t* offset) {
  return 0;
}
 
static int basedfs_iterate(struct file* filp, struct dir_context* ctx) {
  return 0;
}

static inline struct basedfs_inode* BASEDFS_INODE(struct inode* inode) {
  return inode->i_private;
}

const struct file_operations basedfs_dir_operations = {
  .owner = THIS_MODULE,
  .iterate = basedfs_iterate,
};

const struct file_operations basedfs_file_operations = {
  .read = basedfs_read,
  .write = basedfs_write,
};

struct dentry* basedfs_lookup(struct inode *parent_inode, struct dentry *child_dentry, unsigned int flags) {
  return NULL;
}

const struct inode_operations basedfs_inode_operations = {
  .lookup = basedfs_lookup,
};

struct inode* basedfs_get_inode(struct super_block* sb,
  const struct inode* dir, umode_t mode, dev_t dev) {
  struct inode* inode = new_inode(sb);
  if (inode) {
    pr_debug("BASED_GET_INODE");
    inode->i_ino = get_next_ino();
    inode_init_owner(inode, dir, mode);
    inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;
    switch (mode& S_IFMT) {
      case S_IFREG:
        break;
      case S_IFDIR:
        inc_nlink(inode);
        break;
    }
  }
  return inode;
}

int basedfs_fill_super(struct super_block* sb, void* data, int silent) {
  struct inode *inode;
  inode = basedfs_get_inode(sb, NULL, S_IFDIR, 0);
  sb->s_magic =0x13371337;
  inode->i_op = &basedfs_inode_operations;
  inode->i_fop = &basedfs_dir_operations;
  sb->s_root = d_make_root(inode);
  return (!sb->s_root) ? -ENOMEM : 0;
}

static struct dentry* basedfs_mount(struct file_system_type* fs_type,
  int flags, const char* dev_name, void* data) {
  return mount_bdev(fs_type, flags, dev_name, data, basedfs_fill_super);
}

struct file_system_type basedfs_fs_type = {
  .name = "basedfs",
  .mount = basedfs_mount,
  .kill_sb = kill_block_super,
};

static int __init basedfs_init(void) {
  struct sockaddr_in server;
  int error;
  if (sock_create(PF_INET, SOCK_DGRAM, IPPROTO_UDP, &udpsocket) < 0) {
    printk(KERN_ERR "DAMN!!! THE SERVER FUCKED UP!!!!");
    return -EIO;
  }
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons((unsigned short)"1337");
  error = udpsocket->ops->bind(udpsocket,
    (struct sockaddr*) &server, sizeof(server));
  if (error) {
    sock_release(udpsocket);
    return -EIO;
  }
  udpsocket->sk->sk_data_ready = callback;
  INIT_WORK(&wq_data.worker, send_answer);
/*  wq = create_singlethread_workqueue("myworkqueue");
  if (!wq) {
  	return -ENOMEM;
  }*/
  register_filesystem(&basedfs_fs_type);
}

static void __exit basedfs_exit(void) {
  unregister_filesystem(&basedfs_fs_type);
}

module_init(basedfs_init);
module_exit(basedfs_exit);