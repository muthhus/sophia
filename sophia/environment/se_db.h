#ifndef SE_DB_H_
#define SE_DB_H_

/*
 * sophia database
 * sphia.org
 *
 * Copyright (c) Dmitry Simonenko
 * BSD License
*/

typedef struct sedb sedb;

struct sedb {
	so         o;
	uint32_t   created;
	siprofiler rtp;
	sischeme  *scheme;
	si        *index;
	sr        *r;
	sxindex    coindex;
};

int  se_dbopen(so*);
int  se_dbdestroy(so*);
so  *se_dbnew(se*, char*, int);
so  *se_dbmatch(se*, char*);
so  *se_dbmatch_id(se*, uint32_t);

#endif
