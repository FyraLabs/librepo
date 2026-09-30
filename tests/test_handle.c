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

/** Copy a directory recursively (used to build test repositories)
 */
static void
test_copy_dir(const char *src, const char *dst)
{
    char *cmd = g_strdup_printf("cp -r '%s' '%s'", src, dst);
    gchar *stdout_ = NULL, *stderr_ = NULL;
    gint exit_status = 0;
    gboolean ret = g_spawn_command_line_sync(cmd, &stdout_, &stderr_, &exit_status, NULL);
    g_free(cmd);
    ck_assert_msg(ret, "cp -r spawn failed");
    ck_assert_msg(exit_status == 0, "cp -r failed: %s",
                  stderr_ ? stderr_ : "");
    g_free(stdout_);
    g_free(stderr_);
}

START_TEST(test_handle_gpgcheck_mirror_retry)
{
    // https://github.com/rpm-software-management/librepo/issues/415
    // When GPG verification of repomd.xml fails on a mirror, the next
    // mirror should be tried instead of failing the whole download.

    LrHandle *h = NULL;
    LrResult *r = NULL;
    GError *tmp_err = NULL;
    char *gnupg_home = NULL;
    char *testdata = NULL;
    char *key_path = NULL;
    char *bad_repo = NULL;
    char *bad_sig_src = NULL;
    char *bad_sig_dst = NULL;
    char *good_repo = NULL;
    char *urls[3] = {NULL, NULL, NULL};
    gchar *bad_sig_content = NULL;
    gsize bad_sig_len = 0;

    // file:// URLs must be absolute, canonicalize the test data dir
    testdata = g_canonicalize_filename(test_globals.testdata_dir, NULL);
    ck_assert_ptr_nonnull(testdata);

    // Import the test public key into a temporary GPG home
    gnupg_home = lr_gettmpdir();
    ck_assert_ptr_nonnull(gnupg_home);
    key_path = lr_pathconcat(testdata,
                             "repo_yum_01/repodata/repomd.xml.key.asc", NULL);
    ck_assert(lr_gpg_import_key(key_path, gnupg_home, &tmp_err));
    ck_assert_ptr_null(tmp_err);

    // Create a "bad" repository: a copy of repo_yum_01 whose
    // repomd.xml.asc does not verify (signed by an unknown key)
    bad_repo = lr_pathconcat(test_globals.tmpdir, "repo_badgpg", NULL);
    test_copy_dir(lr_pathconcat(testdata, "repo_yum_01", NULL),
                  bad_repo);
    bad_sig_src = lr_pathconcat(testdata,
                               "repo_yum_01/repodata/repomd.xml_bad.sig", NULL);
    bad_sig_dst = lr_pathconcat(bad_repo, "repodata/repomd.xml.asc", NULL);
    // overwrite the good signature with the bad one
    ck_assert(g_file_get_contents(bad_sig_src, &bad_sig_content, &bad_sig_len, &tmp_err));
    ck_assert(g_file_set_contents(bad_sig_dst, bad_sig_content, bad_sig_len, &tmp_err));
    g_free(bad_sig_content);

    good_repo = lr_pathconcat(testdata, "repo_yum_01", NULL);

    h = lr_handle_init();
    ck_assert_ptr_nonnull(h);
    urls[0] = g_strdup_printf("file://%s", bad_repo);
    urls[1] = g_strdup_printf("file://%s", good_repo);
    ck_assert(lr_handle_setopt(h, &tmp_err, LRO_URLS, urls));
    ck_assert_ptr_null(tmp_err);
    ck_assert(lr_handle_setopt(h, &tmp_err, LRO_REPOTYPE, LR_YUMREPO));
    ck_assert(lr_handle_setopt(h, &tmp_err, LRO_DESTDIR, test_globals.tmpdir));
    ck_assert(lr_handle_setopt(h, &tmp_err, LRO_GPGCHECK, 1L));
    ck_assert(lr_handle_setopt(h, &tmp_err, LRO_GNUPGHOMEDIR, gnupg_home));

    r = lr_result_init();
    ck_assert_ptr_nonnull(r);

    // Before the fix: fails, because the GPG check was done only for the
    // mirror that provided repomd.xml. After the fix: succeeds, because
    // the second (good) mirror is tried.
    ck_assert_msg(lr_handle_perform(h, r, &tmp_err),
                  "GPG check should retry the next mirror: %s",
                  tmp_err ? tmp_err->message : "");
    ck_assert_ptr_null(tmp_err);

    // The verified signature must have been downloaded
    LrYumRepo *yum_repo = NULL;
    ck_assert(lr_result_getinfo(r, &tmp_err, LRR_YUM_REPO, &yum_repo));
    ck_assert_ptr_nonnull(yum_repo);
    ck_assert_ptr_nonnull(yum_repo->signature);

    // The used mirror must be the good one
    ck_assert(yum_repo->url && g_str_has_suffix(yum_repo->url, "repo_yum_01"));

    lr_result_free(r);
    lr_handle_free(h);
    g_free(urls[0]);
    g_free(urls[1]);
    g_free(gnupg_home);
    g_free(testdata);
    g_free(key_path);
    g_free(bad_repo);
    g_free(bad_sig_src);
    g_free(bad_sig_dst);
    g_free(good_repo);
}
END_TEST

START_TEST(test_handle_gpgcheck_single_bad_mirror)
{
    // Regression guard: with a single mirror whose repomd.xml.asc does not
    // verify, the GPG check must still fail (no behavior change).

    LrHandle *h = NULL;
    LrResult *r = NULL;
    GError *tmp_err = NULL;
    char *gnupg_home = NULL;
    char *testdata = NULL;
    char *key_path = NULL;
    char *bad_repo = NULL;
    char *bad_sig_src = NULL;
    char *bad_sig_dst = NULL;
    char *urls[2] = {NULL, NULL};
    char *destdir = NULL;
    gchar *bad_sig_content = NULL;
    gsize bad_sig_len = 0;

    testdata = g_canonicalize_filename(test_globals.testdata_dir, NULL);
    ck_assert_ptr_nonnull(testdata);

    gnupg_home = lr_gettmpdir();
    ck_assert_ptr_nonnull(gnupg_home);
    key_path = lr_pathconcat(testdata,
                             "repo_yum_01/repodata/repomd.xml.key.asc", NULL);
    ck_assert(lr_gpg_import_key(key_path, gnupg_home, &tmp_err));
    ck_assert_ptr_null(tmp_err);

    bad_repo = lr_pathconcat(test_globals.tmpdir, "repo_badgpg2", NULL);
    test_copy_dir(lr_pathconcat(testdata, "repo_yum_01", NULL),
                  bad_repo);
    bad_sig_src = lr_pathconcat(testdata,
                               "repo_yum_01/repodata/repomd.xml_bad.sig", NULL);
    bad_sig_dst = lr_pathconcat(bad_repo, "repodata/repomd.xml.asc", NULL);
    ck_assert(g_file_get_contents(bad_sig_src, &bad_sig_content, &bad_sig_len, &tmp_err));
    ck_assert(g_file_set_contents(bad_sig_dst, bad_sig_content, bad_sig_len, &tmp_err));
    g_free(bad_sig_content);

    h = lr_handle_init();
    ck_assert_ptr_nonnull(h);
    urls[0] = g_strdup_printf("file://%s", bad_repo);
    ck_assert(lr_handle_setopt(h, &tmp_err, LRO_URLS, urls));
    ck_assert_ptr_null(tmp_err);
    ck_assert(lr_handle_setopt(h, &tmp_err, LRO_REPOTYPE, LR_YUMREPO));
    destdir = lr_pathconcat(test_globals.tmpdir, "destdir2", NULL);
    if (mkdir(destdir, S_IRWXU) == -1 && errno != EEXIST)
        ck_abort_msg("Cannot create destdir");
    ck_assert(lr_handle_setopt(h, &tmp_err, LRO_DESTDIR, destdir));
    ck_assert(lr_handle_setopt(h, &tmp_err, LRO_GPGCHECK, 1L));
    ck_assert(lr_handle_setopt(h, &tmp_err, LRO_GNUPGHOMEDIR, gnupg_home));

    r = lr_result_init();
    ck_assert_ptr_nonnull(r);

    ck_assert(!lr_handle_perform(h, r, &tmp_err));
    ck_assert_ptr_nonnull(tmp_err);
    ck_assert_int_eq(tmp_err->code, LRE_BADGPG);

    lr_result_free(r);
    lr_handle_free(h);
    g_free(urls[0]);
    g_free(destdir);
    g_free(gnupg_home);
    g_free(testdata);
    g_free(key_path);
    g_free(bad_repo);
    g_free(bad_sig_src);
    g_free(bad_sig_dst);
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
    tcase_add_test(tc, test_handle_gpgcheck_mirror_retry);
    tcase_add_test(tc, test_handle_gpgcheck_single_bad_mirror);
    suite_add_tcase(s, tc);
    return s;
}
