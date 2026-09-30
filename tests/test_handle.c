#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "librepo/librepo.h"
#include "librepo/rcodes.h"
#include "librepo/handle.h"
#include "librepo/url_substitution.h"
#include "librepo/cleanup.h"

#include "fixtures.h"
#include "testsys.h"
#include "test_handle.h"

START_TEST(test_handle)
{
    LrHandle *h = NULL;
    GError *tmp_err = NULL;

    h = lr_handle_init();
    ck_assert_ptr_nonnull(h);
    lr_handle_free(h);
    h = NULL;

    /* This test is meant to check memory leaks. (Use valgrind) */
    h = lr_handle_init();
    char *urls[] = {"foo", NULL};
    ck_assert(lr_handle_setopt(h, &tmp_err, LRO_URLS, urls));
    ck_assert_ptr_null(tmp_err);
    ck_assert(lr_handle_setopt(h, &tmp_err, LRO_URLS, urls));
    ck_assert_ptr_null(tmp_err);
    ck_assert(lr_handle_setopt(h, &tmp_err, LRO_MIRRORLIST, "foo"));
    ck_assert_ptr_null(tmp_err);
    ck_assert(lr_handle_setopt(h, &tmp_err, LRO_MIRRORLIST, "bar"));
    ck_assert_ptr_null(tmp_err);
    ck_assert(lr_handle_setopt(h, NULL, LRO_USERPWD, "user:pwd"));
    ck_assert(lr_handle_setopt(h, NULL, LRO_USERNAME, "user"));
    ck_assert(lr_handle_setopt(h, NULL, LRO_PASSWORD, "pwd"));
    ck_assert(lr_handle_setopt(h, NULL, LRO_PROXY, "proxy"));
    ck_assert(lr_handle_setopt(h, NULL, LRO_PROXYUSERPWD, "proxyuser:pwd"));
    ck_assert(lr_handle_setopt(h, NULL, LRO_DESTDIR, "foodir"));
    ck_assert(lr_handle_setopt(h, NULL, LRO_USERAGENT, "librepo/0.0"));
    char *dlist[] = {"primary", "filelists", NULL};
    ck_assert(lr_handle_setopt(h, NULL, LRO_YUMDLIST, dlist));
    ck_assert(lr_handle_setopt(h, NULL, LRO_YUMBLIST, dlist));
    LrUrlVars *vars = NULL;
    vars = lr_urlvars_set(vars, "foo", "bar");
    ck_assert(lr_handle_setopt(h, NULL, LRO_VARSUB, vars));
    ck_assert(lr_handle_setopt(h, NULL, LRO_FASTESTMIRRORCACHE,
                              "/var/cache/fastestmirror.librepo"));
    ck_assert(lr_handle_setopt(h, NULL, LRO_SSLCLIENTCERT, "/etc/cert.pem"));
    ck_assert(lr_handle_setopt(h, NULL, LRO_SSLCLIENTKEY, "/etc/cert.key"));
    ck_assert(lr_handle_setopt(h, NULL, LRO_SSLCACERT, "/etc/ca.pem"));
    ck_assert(lr_handle_setopt(h, NULL, LRO_PROXY_SSLCLIENTCERT, "/etc/proxy_cert.pem"));
    ck_assert(lr_handle_setopt(h, NULL, LRO_PROXY_SSLCLIENTKEY, "/etc/proxy_cert.key"));
    ck_assert(lr_handle_setopt(h, NULL, LRO_PROXY_SSLCACERT, "/etc/proxy_ca.pem"));
    (void)lr_handle_setopt(h, NULL, LRO_HTTPAUTHMETHODS, LR_AUTH_NTLM);
    ck_assert(lr_handle_setopt(h, NULL, LRO_PROXYAUTHMETHODS, LR_AUTH_DIGEST));
    lr_handle_free(h);
}
END_TEST

START_TEST(test_handle_getinfo)
{
    long num;
    char *str;
    char **strlist;
    LrHandle *h = NULL;
    GError *tmp_err = NULL;

    h = lr_handle_init();

    num = -1;
    ck_assert(lr_handle_getinfo(h, &tmp_err, LRI_UPDATE, &num));
    ck_assert(num == 0);
    ck_assert_ptr_null(tmp_err);

    strlist = NULL;
    ck_assert(lr_handle_getinfo(h, &tmp_err, LRI_URLS, &strlist));
    ck_assert_ptr_null(strlist);
    ck_assert_ptr_null(tmp_err);

    str = NULL;
    ck_assert(lr_handle_getinfo(h, NULL, LRI_MIRRORLIST, &str));
    ck_assert_ptr_null(str);

    num = -1;
    ck_assert(lr_handle_getinfo(h, NULL, LRI_LOCAL, &num));
    ck_assert(num == 0);

    str = NULL;
    ck_assert(lr_handle_getinfo(h, NULL, LRI_DESTDIR, &str));
    ck_assert_ptr_null(str);

    num = -1;
    ck_assert(lr_handle_getinfo(h, NULL, LRI_REPOTYPE, &num));
    ck_assert(num == 0);

    str = NULL;
    ck_assert(lr_handle_getinfo(h, NULL, LRI_USERAGENT, &str));
    ck_assert_ptr_null(str);

    strlist = NULL;
    ck_assert(lr_handle_getinfo(h, NULL, LRI_YUMDLIST, &strlist));
    ck_assert_ptr_null(strlist);

    strlist = NULL;
    ck_assert(lr_handle_getinfo(h, NULL, LRI_YUMBLIST, &strlist));
    ck_assert_ptr_null(strlist);

    num = -1;
    ck_assert(lr_handle_getinfo(h, NULL, LRI_MAXMIRRORTRIES, &num));
    ck_assert(num == 0);

    LrUrlVars *vars = NULL;
    ck_assert(lr_handle_getinfo(h, NULL, LRI_VARSUB, &vars));
    ck_assert_ptr_null(strlist);

    str = NULL;
    ck_assert(lr_handle_getinfo(h, NULL, LRI_FASTESTMIRRORCACHE, &str));
    ck_assert_ptr_null(str);

    num = -1;
    ck_assert(lr_handle_getinfo(h, NULL, LRI_FASTESTMIRRORMAXAGE, &num));
    ck_assert(num == LRO_FASTESTMIRRORMAXAGE_DEFAULT);

    str = NULL;
    ck_assert(lr_handle_getinfo(h, NULL, LRI_SSLCLIENTCERT, &str));
    ck_assert_ptr_null(str);

    str = NULL;
    ck_assert(lr_handle_getinfo(h, NULL, LRI_SSLCLIENTKEY, &str));
    ck_assert_ptr_null(str);

    str = NULL;
    ck_assert(lr_handle_getinfo(h, NULL, LRI_SSLCACERT, &str));
    ck_assert_ptr_null(str);

    str = NULL;
    ck_assert(lr_handle_getinfo(h, NULL, LRI_PROXY_SSLCLIENTCERT, &str));
    ck_assert_ptr_null(str);

    str = NULL;
    ck_assert(lr_handle_getinfo(h, NULL, LRI_PROXY_SSLCLIENTKEY, &str));
    ck_assert_ptr_null(str);

    str = NULL;
    ck_assert(lr_handle_getinfo(h, NULL, LRI_PROXY_SSLCACERT, &str));
    ck_assert_ptr_null(str);

    LrAuth auth = LR_AUTH_NONE;
    ck_assert(lr_handle_getinfo(h, NULL, LRI_HTTPAUTHMETHODS, &auth));
    ck_assert(auth == LR_AUTH_BASIC);

    auth = LR_AUTH_NONE;
    ck_assert(lr_handle_getinfo(h, NULL, LRI_PROXYAUTHMETHODS, &auth));
    ck_assert(auth == LR_AUTH_BASIC);

    lr_handle_free(h);
}
END_TEST

START_TEST(test_handle_maxmirrortries)
{
    long num = -1;
    GError *tmp_err = NULL;
    LrHandle *h = lr_handle_init();
    ck_assert_ptr_nonnull(h);

    // A negative value must be rejected and must not change the option
    ck_assert(!lr_handle_setopt(h, &tmp_err, LRO_MAXMIRRORTRIES, -1L));
    ck_assert_ptr_nonnull(tmp_err);
    ck_assert_int_eq(tmp_err->code, LRE_BADOPTARG);
    g_clear_error(&tmp_err);
    ck_assert(lr_handle_getinfo(h, NULL, LRI_MAXMIRRORTRIES, &num));
    ck_assert_int_eq(num, LRO_MAXMIRRORTRIES_DEFAULT);

    ck_assert(lr_handle_setopt(h, &tmp_err, LRO_MAXMIRRORTRIES, 5L));
    ck_assert_ptr_null(tmp_err);
    ck_assert(lr_handle_getinfo(h, NULL, LRI_MAXMIRRORTRIES, &num));
    ck_assert_int_eq(num, 5);

    // Once a valid value is set, an invalid one must still be rejected
    ck_assert(!lr_handle_setopt(h, &tmp_err, LRO_MAXMIRRORTRIES, -3L));
    ck_assert_ptr_nonnull(tmp_err);
    g_clear_error(&tmp_err);
    ck_assert(lr_handle_getinfo(h, NULL, LRI_MAXMIRRORTRIES, &num));
    ck_assert_int_eq(num, 5);

    lr_handle_free(h);
}
END_TEST

/* _i == 0: all mirrors fail. _i == 1: retry the complete pair on a good
 * mirror. The bad repomd is longer, so retry must also truncate its contents. */
START_TEST(test_handle_gpgcheck_mirror_retry)
{
    GError *err = NULL;
    _cleanup_free_ char *source = g_canonicalize_filename(test_globals.testdata_dir, NULL);
    _cleanup_free_ char *good = lr_pathconcat(source, "repo_yum_01", NULL);
    _cleanup_free_ char *bad = g_strdup_printf("%s/bad-gpg-%d", test_globals.tmpdir, _i);
    _cleanup_free_ char *repodata = lr_pathconcat(bad, "repodata", NULL);
    _cleanup_free_ char *home = g_strdup_printf("%s/keyring-%d", test_globals.tmpdir, _i);
    _cleanup_free_ char *dest = g_strdup_printf("%s/gpg-dest-%d", test_globals.tmpdir, _i);
    ck_assert_int_eq(g_mkdir_with_parents(repodata, 0700), 0);
    ck_assert_int_eq(g_mkdir_with_parents(home, 0700), 0);
    ck_assert_int_eq(g_mkdir_with_parents(dest, 0700), 0);

    _cleanup_free_ char *key = lr_pathconcat(good, "repodata/repomd.xml.key.asc", NULL);
    ck_assert(lr_gpg_import_key(key, home, &err));
    _cleanup_free_ char *src = lr_pathconcat(good, "repodata/repomd.xml", NULL);
    _cleanup_free_ char *dst = lr_pathconcat(repodata, "repomd.xml", NULL);
    _cleanup_free_ char *content = NULL;
    ck_assert(g_file_get_contents(src, &content, NULL, &err));
    _cleanup_free_ char *modified = g_strconcat(content, "\n<!-- out of sync -->\n", NULL);
    ck_assert(g_file_set_contents(dst, modified, -1, &err));
    _cleanup_free_ char *sig_src = lr_pathconcat(good, "repodata/repomd.xml.asc", NULL);
    _cleanup_free_ char *sig_dst = lr_pathconcat(repodata, "repomd.xml.asc", NULL);
    ck_assert_int_eq(symlink(sig_src, sig_dst), 0);

    _cleanup_free_ char *bad_url = g_strconcat("file://", bad, NULL);
    _cleanup_free_ char *good_url = g_strconcat("file://", good, NULL);
    char *urls[] = {bad_url, _i ? good_url : NULL, NULL};
    char *dlist[] = {NULL};
    LrHandle *h = lr_handle_init();
    ck_assert(lr_handle_setopt(h, &err, LRO_URLS, urls));
    ck_assert(lr_handle_setopt(h, &err, LRO_DESTDIR, dest));
    ck_assert(lr_handle_setopt(h, &err, LRO_REPOTYPE, LR_YUMREPO));
    ck_assert(lr_handle_setopt(h, &err, LRO_YUMDLIST, dlist));
    ck_assert(lr_handle_setopt(h, &err, LRO_GPGCHECK, 1L));
    ck_assert(lr_handle_setopt(h, &err, LRO_GNUPGHOMEDIR, home));
    ck_assert(lr_handle_setopt(h, &err, LRO_MAXMIRRORTRIES, 2L));
    LrResult *r = lr_result_init();
    gboolean ret = lr_handle_perform(h, r, &err);
    ck_assert_msg(ret == _i, "Unexpected GPG result: %s", err ? err->message : "success");
    if (_i) {
        LrYumRepo *repo = NULL;
        ck_assert_ptr_null(err);
        ck_assert(lr_result_getinfo(r, &err, LRR_YUM_REPO, &repo));
        ck_assert_ptr_nonnull(repo->signature);
        ck_assert_str_eq(repo->url, good_url);
    } else {
        ck_assert_ptr_nonnull(err);
        ck_assert_int_eq(err->code, LRE_BADGPG);
    }
    g_clear_error(&err);
    lr_result_free(r);
    lr_handle_free(h);
}
END_TEST

Suite *
handle_suite(void)
{
    Suite *s = suite_create("handle");
    TCase *tc = tcase_create("Main");
    tcase_add_test(tc, test_handle);
    tcase_add_test(tc, test_handle_getinfo);
    tcase_add_test(tc, test_handle_maxmirrortries);
    tcase_add_loop_test(tc, test_handle_gpgcheck_mirror_retry, 0, 2);
    suite_add_tcase(s, tc);
    return s;
}
