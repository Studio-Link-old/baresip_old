/**
 * @file httpd.c Webserver UI module
 *
 * Copyright (C) 2010 Creytiv.com
 */
#include <re.h>
#include <baresip.h>


static struct http_sock *httpsock;


static int html_print_cmd(struct re_printf *pf, const struct http_msg *req)
{
	struct pl params;

	if (!pf || !req)
		return EINVAL;

	if (pl_isset(&req->prm)) {
		params.p = req->prm.p + 1;
		params.l = req->prm.l - 1;
	}
	else {
		params.p = "h";
		params.l = 1;
	}

    switch (*params.p) {
        case 'e': params.p = "A"; //Stop Audio
                  params.l = 1;
                  break;
        case 'f': params.p = "\n"; //Accept Call
                  params.l = 1;
                  break;
        case 'j': params.p = " "; //Toggle UAs
                  params.l = 1;
                  break;
    }

	return re_hprintf(pf, "%H\n", ui_input_pl, &params);
}


static void http_req_handler(struct http_conn *conn,
			     const struct http_msg *msg, void *arg)
{
	(void)arg;

	if (0 == pl_strcasecmp(&msg->path, "/")) {

		http_creply(conn, 200, "OK",
			    "text/html;charset=UTF-8",
			    "%H", html_print_cmd, msg);
	}
	else {
		http_ereply(conn, 404, "Not Found");
	}
}


static int module_init(void)
{
	struct sa laddr;
	int err;

	if (conf_get_sa(conf_cur(), "http_listen", &laddr)) {
		sa_set_str(&laddr, "127.0.0.1", 8000);
	}

	err = http_listen(&httpsock, &laddr, http_req_handler, NULL);
	if (err)
		return err;

	info("httpd: listening on %J\n", &laddr);

	return 0;
}


static int module_close(void)
{
	httpsock = mem_deref(httpsock);

	return 0;
}


EXPORT_SYM const struct mod_export DECL_EXPORTS(httpd) = {
	"httpd",
	"application",
	module_init,
	module_close,
};
