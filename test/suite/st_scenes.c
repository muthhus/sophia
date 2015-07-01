
/*
 * sophia database
 * sphia.org
 *
 * Copyright (c) Dmitry Simonenko
 * BSD License
*/

#include <sophia.h>
#include <libss.h>
#include <libsf.h>
#include <libsr.h>
#include <libsv.h>
#include <libso.h>
#include <libst.h>

void st_scene_rmrf(stscene *s ssunused)
{
	rmrf(st_r.conf->sophia_dir);
	rmrf(st_r.conf->backup_dir);
	rmrf(st_r.conf->log_dir);
	rmrf(st_r.conf->db_dir);
}

void st_scene_test(stscene *s ssunused)
{
	if (st_r.verbose) {
		printf(".test");
		fflush(NULL);
	}
	st_r.test->function();
	st_r.stat_test++;
}

void st_scene_pass(stscene *s ssunused)
{
	printf(": ok\n");
	fflush(NULL);
}

void st_scene_init(stscene *s ssunused)
{
	st_listinit(&st_r.gc, 1);
	ss_aopen(&st_r.a, &ss_stda);
	sr_schemeinit(&st_r.scheme);
	memset(&st_r.injection, 0, sizeof(st_r.injection));
	sr_errorinit(&st_r.error);
	sr_seqinit(&st_r.seq);
	st_r.crc = ss_crc32c_function();
	st_r.fmt = SF_KV;
	st_r.fmt_storage = SF_SRAW;
	st_r.compression = NULL;
	memset(&st_r.r, 0, sizeof(st_r.r));
	st_r.key_start = 8;
	st_r.key_end = 32;
	st_r.value_start = 0;
	st_r.value_end = 0;
	st_r.is_random = 0;
}

void st_scene_scheme_u32(stscene *s ssunused)
{
	srkey *part = sr_schemeadd(&st_r.scheme, &st_r.a);
	t( sr_keysetname(part, &st_r.a, "key") == 0 );
	t( sr_keyset(part, &st_r.a, "u32") == 0 );
}

void st_scene_rt(stscene *s ssunused)
{
	sr_init(&st_r.r, &st_r.error, &st_r.a,
	        NULL, /* quota */
	        &st_r.seq,
	         st_r.fmt,
	         st_r.fmt_storage,
	         NULL, /* update */
	        &st_r.scheme,
	        &st_r.injection,
	         st_r.crc,
	         st_r.compression);

	st_generator_init(&st_r.g, &st_r.r,
	                  st_r.is_random,
	                  st_r.key_start,
	                  st_r.key_end,
	                  st_r.value_start,
	                  st_r.value_end);
}

void st_scene_gc(stscene *s ssunused)
{
	st_listfree(&st_r.gc, &st_r.a);
	ss_aclose(&st_r.a);
	sr_errorfree(&st_r.error);
	sr_seqfree(&st_r.seq);
	sr_schemefree(&st_r.scheme, &st_r.a);
}

void st_scene_env(stscene *s ssunused)
{
	if (st_r.verbose) {
		printf(".env");
		fflush(NULL);
	}

	t( st_r.env == NULL );
	t( st_r.db  == NULL );

	void *env = sp_env();
	t( env != NULL );
	st_r.env = env;

	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.page_checksum", 1) == 0 );
	t( sp_setint(env, "log.enable", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setint(env, "log.sync", 0) == 0 );
	t( sp_setint(env, "log.rotate_sync", 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.format", "kv", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.compression", "none", 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );

	st_r.db = sp_getobject(env, "db.test");
	t( st_r.db != NULL );
}

void st_scene_branch_wm_1(stscene *s ssunused)
{
	t( sp_setint(st_r.env, "compaction.0.branch_wm", 1) == 0 );
}

void st_scene_thread_5(stscene *s ssunused)
{
	printf(".thread_5");
	fflush(NULL);
	t( sp_setint(st_r.env, "scheduler.threads", 5) == 0 );
}

void st_scene_open(stscene *s ssunused)
{
	if (st_r.verbose) {
		printf(".open");
		fflush(NULL);
	}
	t( sp_open(st_r.env) == 0 );
}

void st_scene_destroy(stscene *s ssunused)
{
	if (st_r.verbose) {
		printf(".destroy");
		fflush(NULL);
	}
	t( st_r.env != NULL );
	t( sp_destroy(st_r.env) == 0 );
	st_r.env = NULL;
	st_r.db  = NULL;
}

void st_scene_truncate(stscene *s ssunused)
{
	if (st_r.verbose) {
		printf(".truncate");
		fflush(NULL);
	}
	void *o = sp_object(st_r.db);
	t( o != NULL );
	void *c = sp_cursor(st_r.db, o);
	t( c != NULL );
	while ((o = sp_get(c, NULL))) {
		void *k = sp_object(st_r.db);
		t( k != NULL );
		int keysize;
		void *key = sp_getstring(o, "key", &keysize);
		sp_setstring(k, "key", key, keysize);
		t( sp_delete(st_r.db, k) == 0 );
		t( sp_destroy(o) == 0 );
	}
	t( sp_destroy(c) == 0 );
}

void st_scene_recover(stscene *s ssunused)
{
	printf("\n (recover) ");
	fflush(NULL);
}

void st_scene_phase(stscene *s)
{
	st_r.phase_scene = s->state;
	st_r.phase = 0;
	if (st_r.verbose == 0)
		return;
	switch (s->state) {
	case 0:
		printf(".in_memory");
		fflush(NULL);
		break;
	case 1:
		printf(".branch");
		fflush(NULL);
		break;
	case 2:
		printf(".compact");
		fflush(NULL);
		break;
	case 3:
		printf(".logrotate_gc");
		fflush(NULL);
		break;
	case 4:
		printf(".branch_compact");
		fflush(NULL);
		break;
	}
}
