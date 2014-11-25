#include <linux/fs.h>

struct inode* basedfs_get_inode(struct super_block* sb,
	const struct inode* dir, umode_t mode, dev_t dev) {
	struct inode* inode = new_inode(sb);
	if (inode) {
		inode->i_ino = get_next_ino();
		inode_init_owner(inode, dir, mode);
		inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;
		switch (mode & S_IFMT) {
			case S_IFREG:
				inode->i_op = &basedfs_file_inode_operations;
				inode->i_fop = &basedfs_file_operations;
				break;
			case S_IFDIR:
				inode->i_op = &basedfs_dir_inode_operations;
				inode->i_fop = &simple_dir_operations;
				inc_nlink(inode);
				break;
		}
	}
	return inode;
}

int basedfs_fill_super(struct super_block* sb, void* data, int silent) {
	sb->sb_magic =0x13371337;
	struct inode *inode = basedfs_get_inode(sb, NULL, SIFDIR, 0);
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

static void basedfs_init() {
	register_filesystem(&basedfs_fs_type);
}

module_init(basedfs_init);
module_edit(basedfs_exit);