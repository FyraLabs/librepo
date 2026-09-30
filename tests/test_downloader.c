#define _GNU_SOURCE
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "librepo/librepo.h"
#include "librepo/rcodes.h"
#include "librepo/util.h"
#include "librepo/downloader.h"
#include "librepo/handle_internal.h"

#include "fixtures.h"
#include "testsys.h"
#include "test_url_substitution.h"

START_TEST(test_downloader_no_list)
{
    GError *err = NULL;
    ck_assert(lr_download(NULL, FALSE, &err));
    ck_assert_ptr_null(err);
}
END_TEST

START_TEST(test_downloader_single_file)
{
    LrHandle *handle;
    GSList *list = NULL;
    GError *err = NULL;
    int fd1;
    char *tmpfn1;
    LrDownloadTarget *t1;
    GError *tmp_err = NULL;

    // Prepare handle

    handle = lr_handle_init();
    ck_assert_ptr_nonnull(handle);

    char *urls[] = {"http://www.google.com", NULL};
    ck_assert(lr_handle_setopt(handle, NULL, LRO_URLS, urls));
    lr_handle_prepare_internal_mirrorlist(handle, FALSE, &tmp_err);
    ck_assert_ptr_null(tmp_err);


    // Prepare list of download targets

    tmpfn1 = lr_pathconcat(test_globals.tmpdir, "single_file_XXXXXX", NULL);

    fd1 = mkstemp(tmpfn1);
    g_free(tmpfn1);
    ck_assert_int_ge(fd1, 0);

    t1 = lr_downloadtarget_new(handle, "index.html", NULL, fd1, NULL, NULL,
                               0, 0, NULL, NULL, NULL, NULL, NULL, 0, 0, NULL,
                               FALSE, FALSE);
    ck_assert_ptr_nonnull(t1);

    list = g_slist_append(list, t1);

    // Download

    ck_assert(lr_download(list, FALSE, &err));
    ck_assert_ptr_null(err);

    lr_handle_free(handle);

    // Check results

    for (GSList *elem = list; elem; elem = g_slist_next(elem)) {
            LrDownloadTarget *dtarget = elem->data;
            if (dtarget->err) {
                printf("Error msg: %s\n", dtarget->err);
                ck_abort();
            }
    }

    g_slist_free_full(list, (GDestroyNotify) lr_downloadtarget_free);
    close(fd1);
}
END_TEST

START_TEST(test_downloader_single_file_2)
{
    GSList *list = NULL;
    GError *err = NULL;
    int fd1;
    char *tmpfn1;
    LrDownloadTarget *t1;

    // Prepare list of download targets

    tmpfn1 = lr_pathconcat(test_globals.tmpdir, "single_file_2_XXXXXX", NULL);

    fd1 = mkstemp(tmpfn1);
    g_free(tmpfn1);
    ck_assert_int_ge(fd1, 0);

    t1 = lr_downloadtarget_new(NULL, "http://seznam.cz/index.html", NULL,
                               fd1, NULL, NULL, 0, 0, NULL, NULL, NULL,
                               NULL, NULL, 0, 0, NULL, FALSE, FALSE);
    ck_assert_ptr_nonnull(t1);

    list = g_slist_append(list, t1);

    // Download

    ck_assert(lr_download(list, FALSE, &err));
    ck_assert_ptr_null(err);

    // Check results

    for (GSList *elem = list; elem; elem = g_slist_next(elem)) {
            LrDownloadTarget *dtarget = elem->data;
            if (dtarget->err) {
                printf("Error msg: %s\n", dtarget->err);
                ck_abort();
            }
    }

    g_slist_free_full(list, (GDestroyNotify) lr_downloadtarget_free);
    close(fd1);
}
END_TEST

START_TEST(test_downloader_two_files)
{
    LrHandle *handle;
    GSList *list = NULL;
    GError *err = NULL;
    int fd1, fd2;
    char *tmpfn1, *tmpfn2;
    LrDownloadTarget *t1, *t2;
    GError *tmp_err = NULL;

    // Prepare handle

    handle = lr_handle_init();
    ck_assert_ptr_nonnull(handle);

    char *urls[] = {"http://www.google.com", NULL};
    ck_assert(lr_handle_setopt(handle, NULL, LRO_URLS, urls));
    lr_handle_prepare_internal_mirrorlist(handle, FALSE, &tmp_err);
    ck_assert_ptr_null(tmp_err);

    // Prepare list of download targets

    tmpfn1 = lr_pathconcat(test_globals.tmpdir, "single_file_1_XXXXXX", NULL);
    tmpfn2 = lr_pathconcat(test_globals.tmpdir, "single_file_2_XXXXXX", NULL);

    fd1 = mkstemp(tmpfn1);
    fd2 = mkstemp(tmpfn2);
    g_free(tmpfn1);
    g_free(tmpfn2);
    ck_assert_int_ge(fd1, 0);
    ck_assert_int_ge(fd2, 0);

    t1 = lr_downloadtarget_new(handle, "index.html", NULL, fd1, NULL,
                               NULL, 0, 0, NULL, NULL, NULL,
                               NULL, NULL, 0, 0, NULL, FALSE, FALSE);
    ck_assert_ptr_nonnull(t1);
    t2 = lr_downloadtarget_new(handle, "index.html", "http://seznam.cz", fd2,
                               NULL, NULL, 0, 0, NULL, NULL, NULL,
                               NULL, NULL, 0, 0, NULL, FALSE, FALSE);
    ck_assert_ptr_nonnull(t2);

    list = g_slist_append(list, t1);
    list = g_slist_append(list, t2);

    // Download

    ck_assert(lr_download(list, FALSE, &err));
    ck_assert_ptr_null(err);

    lr_handle_free(handle);

    // Check results

    for (GSList *elem = list; elem; elem = g_slist_next(elem)) {
            LrDownloadTarget *dtarget = elem->data;
            if (dtarget->err) {
                printf("Error msg: %s\n", dtarget->err);
                ck_abort();
            }
    }

    g_slist_free_full(list, (GDestroyNotify) lr_downloadtarget_free);
    close(fd1);
    close(fd2);
}
END_TEST

START_TEST(test_downloader_three_files_with_error)
{
    LrHandle *handle;
    GSList *list = NULL;
    GError *err = NULL;
    int fd1, fd2, fd3;
    char *tmpfn1, *tmpfn2, *tmpfn3;
    LrDownloadTarget *t1, *t2, *t3;
    GError *tmp_err = NULL;

    // Prepare handle

    handle = lr_handle_init();
    ck_assert_ptr_nonnull(handle);

    char *urls[] = {"http://www.google.com", NULL};
    ck_assert(lr_handle_setopt(handle, NULL, LRO_URLS, urls));
    lr_handle_prepare_internal_mirrorlist(handle, FALSE, &tmp_err);
    ck_assert_ptr_null(tmp_err);

    // Prepare list of download targets

    tmpfn1 = lr_pathconcat(test_globals.tmpdir, "single_file_1_XXXXXX", NULL);
    tmpfn2 = lr_pathconcat(test_globals.tmpdir, "single_file_2_XXXXXX", NULL);
    tmpfn3 = lr_pathconcat(test_globals.tmpdir, "single_file_3_XXXXXX", NULL);

    fd1 = mkstemp(tmpfn1);
    fd2 = mkstemp(tmpfn2);
    fd3 = mkstemp(tmpfn3);
    g_free(tmpfn1);
    g_free(tmpfn2);
    g_free(tmpfn3);
    ck_assert_int_ge(fd1, 0);
    ck_assert_int_ge(fd2, 0);
    ck_assert_int_ge(fd3, 0);

    t1 = lr_downloadtarget_new(handle, "index.html", NULL, fd1, NULL, NULL,
                               0, 0, NULL, NULL, NULL, NULL, NULL, 0, 0, NULL,
                               FALSE, FALSE);
    ck_assert_ptr_nonnull(t1);

    t2 = lr_downloadtarget_new(handle, "index.html", "http://seznam.cz", fd2,
                               NULL, NULL, 0, 0, NULL, NULL, NULL, NULL,
                               NULL, 0, 0, NULL, FALSE, FALSE);
    ck_assert_ptr_nonnull(t2);

    t3 = lr_downloadtarget_new(handle, "i_hope_this_page_doesnt_exists.html",
                               "http://google.com", fd3, NULL, NULL,
                               0, 0, NULL, NULL, NULL, NULL, NULL, 0, 0, NULL,
                               FALSE, FALSE);
    ck_assert_ptr_nonnull(t3);

    list = g_slist_append(list, t1);
    list = g_slist_append(list, t2);
    list = g_slist_append(list, t3);

    // Download

    ck_assert(lr_download(list, FALSE, &err));
    ck_assert_ptr_null(err);

    lr_handle_free(handle);

    // Check results

    int x = 0;
    for (GSList *elem = list; elem; elem = g_slist_next(elem)) {
            LrDownloadTarget *dtarget = elem->data;
            ++x;

            if (x != 3 && dtarget->err) {
                printf("Error msg: %s\n", dtarget->err);
                ck_abort();
            }

            if (x == 3 && !dtarget->err) {
                printf("No 404 error raised!\n");
                ck_abort();
            }
    }

    g_slist_free_full(list, (GDestroyNotify) lr_downloadtarget_free);
    close(fd1);
    close(fd2);
    close(fd3);
}
END_TEST

START_TEST(test_downloader_checksum)
{
    const struct {
        const char *sha512;
        int expect_err;
    } tests[] = {
        {
            "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e",
            0,
        },
        {
            "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
            1,
        },
        {
            NULL
        }
    };
    int i;

    for (i = 0; tests[i].sha512; i++) {
        LrHandle *handle;
        GSList *list = NULL;
        GError *err = NULL;
        int fd1;
        char *tmpfn1;
        LrDownloadTargetChecksum *checksum;
        GSList *checksums = NULL;
        LrDownloadTarget *t1;
        GError *tmp_err = NULL;

        // Prepare handle

        handle = lr_handle_init();
        ck_assert_ptr_nonnull(handle);

        char *urls[] = {"file:///", NULL};
        ck_assert(lr_handle_setopt(handle, NULL, LRO_URLS, urls));
        lr_handle_prepare_internal_mirrorlist(handle, FALSE, &tmp_err);
        ck_assert_ptr_null(tmp_err);


        // Prepare list of download targets

        tmpfn1 = lr_pathconcat(test_globals.tmpdir, "single_file_XXXXXX", NULL);

        fd1 = mkstemp(tmpfn1);
        g_free(tmpfn1);
        ck_assert_int_ge(fd1, 0);

        checksum = lr_downloadtargetchecksum_new(LR_CHECKSUM_SHA512,
                                                 tests[i].sha512);
        checksums = g_slist_append(checksums, checksum);

        t1 = lr_downloadtarget_new(handle, "dev/null", NULL, fd1, NULL, checksums,
                                   0, 0, NULL, NULL, NULL, NULL, NULL, 0, 0, NULL,
                                   FALSE, FALSE);
        ck_assert_ptr_nonnull(t1);

        list = g_slist_append(list, t1);

        // Download

        ck_assert(lr_download(list, FALSE, &err));
        ck_assert_ptr_null(err);

        lr_handle_free(handle);

        // Check results

        for (GSList *elem = list; elem; elem = g_slist_next(elem)) {
                LrDownloadTarget *dtarget = elem->data;
                if (!tests[i].expect_err) {
                    if (dtarget->err) {
                        printf("Error msg: %s\n", dtarget->err);
                        ck_abort();
                    }
                } else {
                    if (!dtarget->err) {
                        printf("No checksum error raised!\n");
                        ck_abort();
                    }
                }
        }

        g_slist_free_full(list, (GDestroyNotify) lr_downloadtarget_free);
        close(fd1);
    }
}
END_TEST

/* Answer every connection with the response until killed */
static void
http_server(int sock, const char *response)
{
    for (;;) {
        char buf[4096];
        int conn = accept(sock, NULL, NULL);
        if (conn < 0)
            continue;
        ssize_t r = read(conn, buf, sizeof(buf));  // The request is ignored
        r = write(conn, response, strlen(response));
        (void) r;
        close(conn);
    }
}

/* Header names are case-insensitive (HTTP/2 servers send them in lower
 * case) and there doesn't have to be a space after the colon. Such
 * Content-Length must be compared with the expected size too. */
START_TEST(test_downloader_content_length_header)
{
    const char *response = "HTTP/1.1 200 OK\r\n"
                           "content-length:11\r\n"
                           "Connection: close\r\n"
                           "\r\n"
                           "hello world";
    const gint64 expected_sizes[] = {1000, 11};  // A wrong and the right size
    struct sockaddr_in addr = {0};
    socklen_t addr_len = sizeof(addr);

    // Listen on a free port of the loopback interface
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    ck_assert_int_eq(bind(sock, (struct sockaddr *) &addr, sizeof(addr)), 0);
    ck_assert_int_eq(listen(sock, 8), 0);
    ck_assert_int_eq(getsockname(sock, (struct sockaddr *) &addr, &addr_len), 0);

    pid_t server = fork();
    ck_assert_int_ge(server, 0);
    if (server == 0)
        http_server(sock, response);
    close(sock);

    // No proxy for the local transfer
    LrHandle *handle = lr_handle_init();
    ck_assert_ptr_nonnull(handle);
    ck_assert(lr_handle_setopt(handle, NULL, LRO_PROXY, ""));

    gchar *url = g_strdup_printf("http://127.0.0.1:%d/file", ntohs(addr.sin_port));
    char *fn = lr_pathconcat(test_globals.tmpdir, "content_length_header", NULL);
    for (int i = 0; i < 2; i++) {
        GError *err = NULL;
        LrDownloadTarget *t = lr_downloadtarget_new(handle, url, NULL, -1, fn, NULL,
                                                    expected_sizes[i], FALSE,
                                                    NULL, NULL, NULL, NULL, NULL,
                                                    0, 0, NULL, FALSE, FALSE);
        GSList *list = g_slist_append(NULL, t);
        ck_assert(lr_download(list, FALSE, &err));
        ck_assert_ptr_null(err);
        if (expected_sizes[i] != 11)
            ck_assert_msg(t->err && strstr(t->err, "Inconsistent server data"),
                          "Size mismatch not detected: %s",
                          t->err ? t->err : "(no error)");
        else
            ck_assert_msg(t->err == NULL, "Unexpected error: %s", t->err);
        g_slist_free_full(list, (GDestroyNotify) lr_downloadtarget_free);
    }

    // SIGKILL: the server inherited check's SIGTERM handler, which would
    // forward the signal to the whole process group
    kill(server, SIGKILL);
    waitpid(server, NULL, 0);
    lr_handle_free(handle);
    lr_free(fn);
    g_free(url);
}
END_TEST

#ifdef WITH_ZCHUNK
#include <zck.h>

/** Create a zchunk file from a plain file.
 * Returns the header digest (to be freed by the caller) and sets
 * *header_size and *file_size.
 */
static char *
test_create_zck_fixture(const char *zck_path, const char *src_path,
                        gint64 *header_size, gsize *file_size,
                        GError **err)
{
    int out_fd = open(zck_path, O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (out_fd < 0) {
        g_set_error(err, LR_DOWNLOADER_ERROR, LRE_IO,
                    "Cannot create %s: %s", zck_path, g_strerror(errno));
        return NULL;
    }

    zckCtx *ctx = zck_create();
    if (!ctx || !zck_init_write(ctx, out_fd)) {
        g_set_error(err, LR_DOWNLOADER_ERROR, LRE_ZCK,
                    "zck_init_write failed: %s", zck_get_error(ctx));
        zck_free(&ctx);
        close(out_fd);
        return NULL;
    }

    FILE *src = fopen(src_path, "rb");
    if (!src) {
        g_set_error(err, LR_DOWNLOADER_ERROR, LRE_IO,
                    "Cannot open %s: %s", src_path, g_strerror(errno));
        zck_free(&ctx);
        close(out_fd);
        return NULL;
    }
    *file_size = 0;
    char buf[65536];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), src)) > 0) {
        *file_size += n;
        if (zck_write(ctx, buf, n) != (ssize_t) n) {
            g_set_error(err, LR_DOWNLOADER_ERROR, LRE_ZCK,
                        "zck_write failed: %s", zck_get_error(ctx));
            fclose(src);
            zck_free(&ctx);
            close(out_fd);
            return NULL;
        }
    }
    fclose(src);

    if (!zck_end_chunk(ctx) || !zck_close(ctx)) {
        g_set_error(err, LR_DOWNLOADER_ERROR, LRE_ZCK,
                    "zck close failed: %s", zck_get_error(ctx));
        zck_free(&ctx);
        close(out_fd);
        return NULL;
    }
    zck_free(&ctx);
    close(out_fd);

    // Read back the header digest and size
    int in_fd = open(zck_path, O_RDONLY);
    if (in_fd < 0) {
        g_set_error(err, LR_DOWNLOADER_ERROR, LRE_IO,
                    "Cannot open %s: %s", zck_path, g_strerror(errno));
        return NULL;
    }
    zckCtx *rctx = zck_create();
    if (!rctx || !zck_init_adv_read(rctx, in_fd) || !zck_read_lead(rctx)
            || !zck_read_header(rctx)) {
        g_set_error(err, LR_DOWNLOADER_ERROR, LRE_ZCK,
                    "zck read failed: %s", zck_get_error(rctx));
        zck_free(&rctx);
        close(in_fd);
        return NULL;
    }
    *header_size = zck_get_header_length(rctx);
    char *digest = g_strdup(zck_get_header_digest(rctx));
    zck_free(&rctx);
    close(in_fd);
    return digest;
}

static int test_validate_reject_calls;

static gboolean
test_validate_reject(LrDownloadTarget *target, const char *mirror_url,
                     GError **err)
{
    (void) target;
    (void) mirror_url;
    test_validate_reject_calls++;
    g_set_error(err, LR_DOWNLOADER_ERROR, LRE_BADGPG,
                "Rejecting target");
    return FALSE;
}

static int test_validate_accept_calls;

static gboolean
test_validate_accept(LrDownloadTarget *target, const char *mirror_url,
                     GError **err)
{
    (void) target;
    (void) mirror_url;
    (void) err;
    test_validate_accept_calls++;
    return TRUE;
}

/** Prepare a handle with a dead URL and a zchunk cache directory
 */
static LrHandle *
test_zchunk_prepare_handle(const char *cachedir, GError **err)
{
    LrHandle *h = lr_handle_init();
    ck_assert_ptr_nonnull(h);
    const char *urls[] = {"http://127.0.0.1:1", NULL};
    ck_assert(lr_handle_setopt(h, err, LRO_URLS, urls));
    ck_assert(lr_handle_setopt(h, err, LRO_CACHEDIR, cachedir));
    ck_assert(lr_handle_setopt(h, err, LRO_DESTDIR, test_globals.tmpdir));
    ck_assert(lr_handle_prepare_internal_mirrorlist(h, FALSE, err));
    return h;
}

START_TEST(test_downloader_zchunk_cached_validate_reject)
{
    // A fully cached zchunk file must be validated by the target's
    // validatecb before being reported as successful - an always
    // rejecting validator must fail the download with its error
    GError *err = NULL;
    char *cachedir = lr_pathconcat(test_globals.tmpdir, "zck_cache_reject", NULL);
    ck_assert(mkdir(cachedir, 0700) == 0 || errno == EEXIST);
    char *fixture = lr_pathconcat(cachedir, "fixture.zck", NULL);
    char *src = lr_pathconcat(test_globals.testdata_dir,
                              "repo_yum_01/repodata/repomd.xml", NULL);
    gint64 header_size = 0;
    gsize file_size = 0;
    char *digest = test_create_zck_fixture(fixture, src, &header_size,
                                           &file_size, &err);
    ck_assert_ptr_nonnull(digest);
    ck_assert_ptr_null(err);

    LrHandle *h = test_zchunk_prepare_handle(cachedir, &err);
    ck_assert_ptr_null(err);

    GSList *checks = g_slist_append(NULL,
        lr_downloadtargetchecksum_new(LR_CHECKSUM_SHA256, digest));
    int fd = lr_gettmpfile();
    LrDownloadTarget *t = lr_downloadtarget_new(h, "review-cached.zck",
                                              NULL, fd, NULL, checks,
                                              (gint64) file_size, FALSE,
                                              NULL, NULL, NULL, NULL,
                                              NULL, 0, 0, NULL, FALSE, TRUE);
    t->zck_header_size = header_size;
    t->validatecb = test_validate_reject;

    GError *dl_err = NULL;
    test_validate_reject_calls = 0;
    gboolean ok = lr_download_target(t, &dl_err);

    // The validator ran (on the cached file) and rejected it
    ck_assert_int_gt(test_validate_reject_calls, 0);
    ck_assert_int_eq(ok, FALSE);
    ck_assert_int_eq(t->rcode, LRE_BADGPG);
    ck_assert_str_eq(dl_err->message, "Rejecting target");

    g_clear_error(&dl_err);
    lr_downloadtarget_free(t);
    lr_handle_free(h);
    close(fd);
    g_free(digest);
    g_free(cachedir);
    g_free(fixture);
    g_free(src);
}
END_TEST

START_TEST(test_downloader_zchunk_cached_validate_accept)
{
    // A fully cached zchunk file that passes the validatecb must be
    // reported as successful without re-downloading (the URL points
    // to a dead server)
    GError *err = NULL;
    char *cachedir = lr_pathconcat(test_globals.tmpdir, "zck_cache_accept", NULL);
    ck_assert(mkdir(cachedir, 0700) == 0 || errno == EEXIST);
    char *fixture = lr_pathconcat(cachedir, "fixture.zck", NULL);
    char *src = lr_pathconcat(test_globals.testdata_dir,
                              "repo_yum_01/repodata/repomd.xml", NULL);
    gint64 header_size = 0;
    gsize file_size = 0;
    char *digest = test_create_zck_fixture(fixture, src, &header_size,
                                           &file_size, &err);
    ck_assert_ptr_nonnull(digest);
    ck_assert_ptr_null(err);

    LrHandle *h = test_zchunk_prepare_handle(cachedir, &err);
    ck_assert_ptr_null(err);

    GSList *checks = g_slist_append(NULL,
        lr_downloadtargetchecksum_new(LR_CHECKSUM_SHA256, digest));
    int fd = lr_gettmpfile();
    LrDownloadTarget *t = lr_downloadtarget_new(h, "review-cached-ok.zck",
                                              NULL, fd, NULL, checks,
                                              (gint64) file_size, FALSE,
                                              NULL, NULL, NULL, NULL,
                                              NULL, 0, 0, NULL, FALSE, TRUE);
    t->zck_header_size = header_size;
    t->validatecb = test_validate_accept;

    GError *dl_err = NULL;
    test_validate_accept_calls = 0;
    gboolean ok = lr_download_target(t, &dl_err);

    // The validator ran once (on the cached file) and the target
    // succeeded without touching the (dead) server
    ck_assert_int_eq(ok, TRUE);
    ck_assert_int_eq(t->rcode, LRE_OK);
    ck_assert_int_eq(test_validate_accept_calls, 1);
    ck_assert_ptr_null(dl_err);

    lr_downloadtarget_free(t);
    lr_handle_free(h);
    close(fd);
    g_free(digest);
    g_free(cachedir);
    g_free(fixture);
    g_free(src);
}
END_TEST
#endif /* WITH_ZCHUNK */

Suite *
downloader_suite(void)
{
    Suite *s = suite_create("downloader");
    TCase *tc = tcase_create("Main");
    tcase_add_test(tc, test_downloader_no_list);
    tcase_add_test(tc, test_downloader_single_file);
    tcase_add_test(tc, test_downloader_single_file_2);
    tcase_add_test(tc, test_downloader_two_files);
    tcase_add_test(tc, test_downloader_three_files_with_error);
    tcase_add_test(tc, test_downloader_checksum);
    suite_add_tcase(s, tc);
    return s;
}

/* Tests that don't need an internet connection */
Suite *
downloader_local_suite(void)
{
    Suite *s = suite_create("downloader_local");
    TCase *tc = tcase_create("Main");
    tcase_add_test(tc, test_downloader_content_length_header);
#ifdef WITH_ZCHUNK
    tcase_add_test(tc, test_downloader_zchunk_cached_validate_reject);
    tcase_add_test(tc, test_downloader_zchunk_cached_validate_accept);
#endif /* WITH_ZCHUNK */
    suite_add_tcase(s, tc);
    return s;
}
