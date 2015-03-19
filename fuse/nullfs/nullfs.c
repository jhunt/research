#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <dirent.h>

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <libgen.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/xattr.h>


int nullfs_getattr(const char *path, struct stat *st)
{
	return 0;
}

int nullfs_readlink(const char *path, char *target, size_t len)
{
	return 1;
}

int nullfs_mknod(const char *path, mode_t mode, dev_t dev)
{
	return 0;
}

int nullfs_mkdir(const char *path, mode_t mode)
{
	return 0;
}

int nullfs_unlink(const char *path)
{
	return 0;
}

int nullfs_rmdir(const char *path)
{
	return 0;
}

int nullfs_symlink(const char *path, const char *target)
{
	return 0;
}

int nullfs_rename(const char *oldpath, const char *newpath)
{
	return 0;
}

int nullfs_link(const char *oldpath, const char *newpath)
{
	return 0;
}

int nullfs_chmod(const char *path, mode_t mode)
{
	return 0;
}

int nullfs_chown(const char *path, uid_t uid, gid_t gid)
{
	return 0;
}

int nullfs_truncate(const char *path, off_t newsize)
{
	return 0;
}

int nullfs_utimens(const char *path, const struct timespec tv[2])
{
	return 0;
}

int nullfs_open(const char *path, struct fuse_file_info *info)
{
	return 0;
}

int nullfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *info)
{
	return 0;
}

int nullfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *info)
{
	return 0;
}

int nullfs_statfs(const char *path, struct statvfs *st)
{
	return -EINVAL;
}

int nullfs_flush(const char *path, struct fuse_file_info *info)
{
	return 0;
}

int nullfs_release(const char *path, struct fuse_file_info *info)
{
	return 0;
}

int nullfs_fsync(const char *path, int datasync, struct fuse_file_info *info)
{
	return 0;
}

int nullfs_setxattr(const char *path, const char *name, const char *value, size_t size, int flags)
{
	return 0;
}

int nullfs_getxattr(const char *path, const char *name, char *value, size_t size)
{
	return 1;
}

int nullfs_listxattr(const char *path, char *list, size_t size)
{
	return 1;
}

int nullfs_removexattr(const char *path, const char *name)
{
	return 0;
}

int nullfs_opendir(const char *path, struct fuse_file_info *info)
{
	return 0;
}

int nullfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *info)
{
	return 0;
}

int nullfs_releasedir(const char *path, struct fuse_file_info *info)
{
	return 0;
}

int nullfs_fsyncdir(const char *path, int datasync, struct fuse_file_info *info)
{
	return 0;
}

void *nullfs_init(struct fuse_conn_info *conn)
{
	return NULL;
}

void nullfs_destroy(void *userdata)
{
}

int nullfs_access(const char *path, int mask)
{
	return 0;
}

int nullfs_create(const char *path, mode_t mode, struct fuse_file_info *info)
{
	return 0;
}

int nullfs_ftruncate(const char *path, off_t offset, struct fuse_file_info *info)
{
	return 0;
}

int nullfs_fgetattr(const char *path, struct stat *statbuf, struct fuse_file_info *info)
{
	return 0;
}

struct fuse_operations nullfs_oper = {
	.getattr     = nullfs_getattr,
	.readlink    = nullfs_readlink,
	.getdir      = NULL, /* deprecated */
	.mknod       = nullfs_mknod,
	.mkdir       = nullfs_mkdir,
	.unlink      = nullfs_unlink,
	.rmdir       = nullfs_rmdir,
	.symlink     = nullfs_symlink,
	.rename      = nullfs_rename,
	.link        = nullfs_link,
	.chmod       = nullfs_chmod,
	.chown       = nullfs_chown,
	.truncate    = nullfs_truncate,

	.utime       = NULL,
	.utimens     = nullfs_utimens,
	.open        = nullfs_open,
	.read        = nullfs_read,
	.write       = nullfs_write,
	.statfs      = nullfs_statfs,
	.flush       = nullfs_flush,
	.release     = nullfs_release,
	.fsync       = nullfs_fsync,
	.setxattr    = nullfs_setxattr,
	.getxattr    = nullfs_getxattr,
	.listxattr   = nullfs_listxattr,
	.removexattr = nullfs_removexattr,
	.opendir     = nullfs_opendir,
	.readdir     = nullfs_readdir,
	.releasedir  = nullfs_releasedir,
	.fsyncdir    = nullfs_fsyncdir,
	.init        = nullfs_init,
	.destroy     = nullfs_destroy,
	.access      = nullfs_access,
	.create      = nullfs_create,
	.ftruncate   = nullfs_ftruncate,
	.fgetattr    = nullfs_fgetattr,
};

int main(int argc, char **argv)
{
	int rc = fuse_main(argc, argv, &nullfs_oper, NULL);
	return rc;
}
