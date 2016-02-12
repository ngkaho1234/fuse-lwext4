#ifndef ACL_H
#define ACL_H

#include <stdint.h>
#include <stdlib.h>

/*
 * Copied from acl_ea.h in libacl source; ACLs have to be sent to and from fuse
 * in this format... at least on Linux.
 */
#define ACL_EA_ACCESS		"system.posix_acl_access"
#define ACL_EA_DEFAULT		"system.posix_acl_default"

#define ACL_EA_VERSION		0x0002

typedef struct {
	uint16_t	e_tag;
	uint16_t	e_perm;
	uint32_t	e_id;
} acl_ea_entry;

typedef struct {
	u_int32_t	a_version;
	acl_ea_entry	a_entries[0];
} acl_ea_header;

/*
 * ext4 ACL structures, copied from fs/ext4/acl.h.
 */
#define EXT4_ACL_VERSION	0x0001

typedef struct {
	uint16_t	e_tag;
	uint16_t	e_perm;
	uint32_t	e_id;
} ext4_acl_entry;

typedef struct {
	uint16_t	e_tag;
	uint16_t	e_perm;
} ext4_acl_entry_short;

typedef struct {
	uint32_t	a_version;
} ext4_acl_header;


int fuse_to_ext4_acl(acl_ea_header *facl, size_t facl_sz,
			    ext4_acl_header **eacl, size_t *eacl_sz);
int ext4_to_fuse_acl(acl_ea_header **facl, size_t *facl_sz,
			    ext4_acl_header *eacl, size_t eacl_sz);

#endif
