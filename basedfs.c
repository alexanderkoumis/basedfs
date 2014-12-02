#include <linux/fs.h>

static int basedfs_iterate(struct file *filp, struct dir_context *ctx){
	return 0;
}

const struct file_operations basedfs_dir_operations = {
	.owner = THIS_MODULE,
	.iterate = basedfs_iterate,
};

static inline struct basedfs_inode* BASEDFS_INODE(struct inode* inode) {
	return inode->i_private;
}

ssize_t basedfs_read(struct file* flip, char __user * buf, size_t len, loff_t* ppos) {

	struct basedfs_inode* inode = BASEDFS_INODE(flip->f_inode);
	int nbytes = 10;
	return nbytes;

}

const struct file_operations basedfs_file_operations = {
	.read = basedfs_read,
};

struct dentry *basedfs_lookup(struct inode *parent_inode, struct dentry *child_dentry, unsigned int flags){
	return NULL;
}

const struct inode_operations basedfs_inode_operations = {
	.lookup = basedfs_lookup,
};


struct inode* basedfs_get_inode(struct super_block* sb,
	const struct inode* dir, umode_t mode, dev_t dev) {
	struct inode* inode = new_inode(sb);
	if (inode) {
		inode->i_ino = get_next_ino();
		inode_init_owner(inode, dir, mode);
		inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;
		switch (mode & S_IFMT) {
			case S_IFREG:
			case S_IFDIR:
				inc_nlink(inode);
				break;
		}
	}
	return inode;
}

int basedfs_fill_super(struct super_block* sb, void* data, int silent) {
	sb->s_magic =0x13371337;
	struct inode *inode = basedfs_get_inode(sb, NULL, S_IFDIR, 0);
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
	int ret;
	ret = register_filesystem(&basedfs_fs_type);
	return ret;
}

static void __exit basedfs_exit(void) {
	unregister_filesystem(&basedfs_fs_type);
	//destroy_inodecache();
}

module_init(basedfs_init);
module_exit(basedfs_exit);
