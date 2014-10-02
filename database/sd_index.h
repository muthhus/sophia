#ifndef SD_INDEX_H_
#define SD_INDEX_H_

/*
 * sophia database
 * sphia.org
 *
 * Copyright (c) Dmitry Simonenko
 * BSD License
*/

typedef struct sdindexheader sdindexheader;
typedef struct sdindexfooter sdindexfooter;
typedef struct sdindexpage sdindexpage;
typedef struct sdindex sdindex;

struct sdindexheader {
	uint32_t crc;
	uint16_t block;
	uint32_t count;
	uint32_t keys;
	uint32_t total;
	uint32_t totalkv;
	uint64_t lsnmin;
	uint64_t lsnmax;
} srpacked;

struct sdindexfooter {
	uint32_t crc;
	uint32_t magic;
	uint32_t size;
} srpacked;

struct sdindexpage {
	uint32_t offset;
	uint32_t size;
	uint16_t sizemin;
	uint16_t sizemax;
	uint64_t lsnmin;
	uint64_t lsnmax;
} srpacked;

struct sdindex {
	srbuf i;
	sdindexheader *h;
	sdindexfooter *f;
};

static inline char*
sd_indexpage_min(sdindexpage *p) {
	return (char*)p + sizeof(sdindexpage);
}

static inline char*
sd_indexpage_max(sdindexpage *p) {
	return (char*)p + sizeof(sdindexpage) + p->sizemin;
}

static inline void
sd_indexinit(sdindex *i) {
	sr_bufinit(&i->i);
	i->h = NULL;
	i->f = NULL;
}

static inline void
sd_indexfree(sdindex *i, sra *a) {
	sr_buffree(&i->i, a);
}

static inline sdindexheader*
sd_indexheader(sdindex *i) {
	return (sdindexheader*)(i->i.s);
}

static inline sdindexfooter*
sd_indexfooter(sdindex *i) {
	return (sdindexfooter*)(i->i.p - sizeof(sdindexfooter));
}

static inline sdindexpage*
sd_indexpage(sdindex *i, uint32_t pos)
{
	assert(pos < i->h->count);
	char *p = (char*)sr_bufat(&i->i, i->h->block, pos);
   	p += sizeof(sdindexheader);
	return (sdindexpage*)p;
}

static inline sdindexpage*
sd_indexmin(sdindex *i) {
	return sd_indexpage(i, 0);
}

static inline sdindexpage*
sd_indexmax(sdindex *i) {
	return sd_indexpage(i, i->h->count - 1);
}
static inline uint16_t
sd_indexkeysize(sdindex *i)
{
	if (srunlikely(i->h == NULL))
		return 0;
	return (sd_indexheader(i)->block - sizeof(sdindexpage)) / 2;
}

static inline uint32_t
sd_indexkeys(sdindex *i)
{
	if (srunlikely(i->h == NULL))
		return 0;
	return sd_indexheader(i)->keys;
}

static inline uint32_t
sd_indextotal(sdindex *i)
{
	if (srunlikely(i->h == NULL))
		return 0;
	return sd_indexheader(i)->total;
}

static inline uint32_t
sd_indextotal_kv(sdindex *i)
{
	if (srunlikely(i->h == NULL))
		return 0;
	return sd_indexheader(i)->totalkv;
}

static inline int
sd_indexpage_cmp(sdindexpage *p, void *key, int size, srcomparator *c)
{
	register int l =
		sr_compare(c, sd_indexpage_min(p), p->sizemin, key, size);
	register int r =
		sr_compare(c, sd_indexpage_max(p), p->sizemax, key, size);
	/* inside page range */
	if (l <= 0 && r >= 0)
		return 0;
	/* key > page */
	if (l == -1)
		return -1;
	/* key < page */
	assert(r == 1);
	return 1;
}

int sd_indexbegin(sdindex*, sra*, uint32_t);
int sd_indexcommit(sdindex*, sra*);
int sd_indexadd(sdindex*, sra*, uint32_t, uint32_t, uint32_t, uint32_t,
                char*, int, char*, int,
                uint64_t, uint64_t);
sdindexheader*
sd_indexvalidate(srmap*);
int sd_indexrecover(sdindex*, sra*, srmap*);

#endif
