#ifndef SR_H_
#define SR_H_

/*
 * sophia database
 * sphia.org
 *
 * Copyright (c) Dmitry Simonenko
 * BSD License
*/

typedef struct sr sr;

struct sr {
	srstatus *status;
	srlog *log;
	srerror *e;
	sfupsert *fmt_upsert;
	sfstorage fmt_storage;
	sfscheme *scheme;
	srseq *seq;
	ssa *a;
	ssa *aref;
	ssvfs *vfs;
	srquota *quota;
	srzonemap *zonemap;
	ssinjection *i;
	srstat *stat;
	sscrcf crc;
};

static inline void
sr_init(sr *r,
        srstatus *status,
        srlog *log,
        srerror *e,
        ssa *a,
        ssa *aref,
        ssvfs *vfs,
        srquota *quota,
        srzonemap *zonemap,
        srseq *seq,
        sfstorage fmt_storage,
        sfupsert *fmt_upsert,
        sfscheme *scheme,
        ssinjection *i,
		srstat *stat,
        sscrcf crc)
{
	r->status      = status;
	r->log         = log;
	r->e           = e;
	r->a           = a;
	r->aref        = aref;
	r->vfs         = vfs;
	r->quota       = quota;
	r->zonemap     = zonemap;
	r->seq         = seq;
	r->scheme      = scheme;
	r->fmt_storage = fmt_storage;
	r->fmt_upsert  = fmt_upsert;
	r->i           = i;
	r->stat        = stat;
	r->crc         = crc;
}

static inline srzone *sr_zoneof(sr *r)
{
	int p = sr_quotaused_percent(r->quota, r->stat);
	return sr_zonemap(r->zonemap, p);
}

#endif
