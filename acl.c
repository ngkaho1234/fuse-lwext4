#include "acl.h"

#include <errno.h>
#include <sys/acl.h>


static inline size_t acl_ea_size(int count)
{
	return sizeof(acl_ea_header) + count * sizeof(acl_ea_entry);
}

static inline int acl_ea_count(size_t size)
{
	if (size < sizeof(acl_ea_header))
		return -1;
	size -= sizeof(acl_ea_header);
	if (size % sizeof(acl_ea_entry))
		return -1;
	return size / sizeof(acl_ea_entry);
}

static inline size_t ext4_acl_size(int count)
{
	if (count <= 4) {
		return sizeof(ext4_acl_header) +
		       count * sizeof(ext4_acl_entry_short);
	} else {
		return sizeof(ext4_acl_header) +
		       4 * sizeof(ext4_acl_entry_short) +
		       (count - 4) * sizeof(ext4_acl_entry);
	}
}

static inline int ext4_acl_count(size_t size)
{
	ssize_t s;

	size -= sizeof(ext4_acl_header);
	s = size - 4 * sizeof(ext4_acl_entry_short);
	if (s < 0) {
		if (size % sizeof(ext4_acl_entry_short))
			return -1;
		return size / sizeof(ext4_acl_entry_short);
	}
	if (s % sizeof(ext4_acl_entry))
		return -1;
	return s / sizeof(ext4_acl_entry) + 4;
}

static uint16_t ext2fs_cpu_to_le16(uint16_t v) { return v; }
static uint32_t ext2fs_cpu_to_le32(uint32_t v) { return v; }

static uint16_t ext2fs_le16_to_cpu(uint16_t v) { return v; }
static uint32_t ext2fs_le32_to_cpu(uint32_t v) { return v; }

int fuse_to_ext4_acl(const acl_ea_header *facl, size_t facl_sz,
			    ext4_acl_header **eacl, size_t *eacl_sz)
{
	int i, facl_count;
	ext4_acl_header *h;
	size_t h_sz;
	ext4_acl_entry *e;
	acl_ea_entry *a;
	unsigned char *hptr;
	int err = 0;

	facl_count = acl_ea_count(facl_sz);
	h_sz = ext4_acl_size(facl_count);
	if (facl_count < 0 || facl->a_version != ACL_EA_VERSION)
		return -EINVAL;

	h = malloc(h_sz);
	if (!h)
		return -ENOMEM;

	h->a_version = ext2fs_cpu_to_le32(EXT4_ACL_VERSION);
	hptr = (unsigned char *) (h + 1);
	for (i = 0, a = facl->a_entries; i < facl_count; i++, a++) {
		e = (ext4_acl_entry *) hptr;
		e->e_tag = ext2fs_cpu_to_le16(a->e_tag);
		e->e_perm = ext2fs_cpu_to_le16(a->e_perm);

		switch (a->e_tag) {
		case ACL_USER:
		case ACL_GROUP:
			e->e_id = ext2fs_cpu_to_le32(a->e_id);
			hptr += sizeof(ext4_acl_entry);
			break;
		case ACL_USER_OBJ:
		case ACL_GROUP_OBJ:
		case ACL_MASK:
		case ACL_OTHER:
			hptr += sizeof(ext4_acl_entry_short);
			break;
		default:
			err = -EINVAL;
			goto out;
		}
	}

	*eacl = h;
	*eacl_sz = h_sz;
	return err;
out:
	free(h);
	return err;
}

int ext4_to_fuse_acl(acl_ea_header **facl, size_t *facl_sz,
		     const ext4_acl_header *eacl, size_t eacl_sz)
{
	int i, eacl_count;
	acl_ea_header *f;
	ext4_acl_entry *e;
	acl_ea_entry *a;
	size_t f_sz;
	unsigned char *hptr;
	int err = 0;

	eacl_count = ext4_acl_count(eacl_sz);
	f_sz = acl_ea_size(eacl_count);
	if (eacl_count < 0 ||
	    eacl->a_version != ext2fs_cpu_to_le32(EXT4_ACL_VERSION))
		return -EINVAL;

	f = malloc(f_sz);
	if (!f)
		return -ENOMEM;

	f->a_version = ACL_EA_VERSION;
	hptr = (unsigned char *) (eacl + 1);
	for (i = 0, a = f->a_entries; i < eacl_count; i++, a++) {
		e = (ext4_acl_entry *) hptr;
		a->e_tag = ext2fs_le16_to_cpu(e->e_tag);
		a->e_perm = ext2fs_le16_to_cpu(e->e_perm);

		switch (a->e_tag) {
		case ACL_USER:
		case ACL_GROUP:
			a->e_id = ext2fs_le32_to_cpu(e->e_id);
			hptr += sizeof(ext4_acl_entry);
			break;
		case ACL_USER_OBJ:
		case ACL_GROUP_OBJ:
		case ACL_MASK:
		case ACL_OTHER:
			hptr += sizeof(ext4_acl_entry_short);
			break;
		default:
			err = -EINVAL;
			goto out;
		}
	}

	*facl = f;
	*facl_sz = f_sz;
	return err;
out:
	free(f);
	return err;
}
