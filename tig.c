#include <fcntl.h>
static void
argv_from_env(const char **argv, const char *name)
{
	char *env = argv ? getenv(name) : NULL;
	int argc = 0;

	if (env && *env)
		env = strdup(env);
	if (env && !argv_from_string(argv, &argc, env))
		die("Too many arguments in the `%s` environment variable", name);
}

	IO_BG,			/* Execute command in the background. */
	IO_FG,			/* Execute command with same std{in,out,err}. */
	enum io_type type;	/* The requested type of pipe. */
	const char *dir;	/* Directory from which to execute. */
	FILE *pid;		/* Pipe for reading or writing. */
	int pipe;		/* Pipe end for reading or writing. */
	const char *argv[SIZEOF_ARG];	/* Shell command arguments. */
	char *buf;		/* Read buffer. */
	size_t bufsize;		/* Buffer content size. */
	char *bufpos;		/* Current buffer position. */
	unsigned int eof:1;	/* Has end of file been reached. */
	io->pipe = -1;
	io->pid = NULL;
	io->buf = io->bufpos = NULL;
	io->bufalloc = io->bufsize = 0;
	io->eof = 0;
init_io(struct io *io, const char *dir, enum io_type type)
	io->dir = dir;
init_io_rd(struct io *io, const char *argv[], const char *dir,
		enum format_flags flags)
	init_io(io, dir, IO_RD);
	return format_argv(io->argv, argv, flags);
}

static bool
io_open(struct io *io, const char *name)
{
	init_io(io, NULL, IO_FD);
	io->pipe = *name ? open(name, O_RDONLY) : STDIN_FILENO;
	return io->pipe != -1;
		close(io->pipe);
	else if (io->type == IO_RD || io->type == IO_WR)
		pclose(io->pid);
	char buf[SIZEOF_STR * 2];
	size_t bufpos = 0;

	if (io->type == IO_FD)
		return TRUE;

	if (io->dir && *io->dir &&
	    !string_format_from(buf, &bufpos, "cd %s;", io->dir))
		return FALSE;

	if (!format_command(buf + bufpos, io->argv, FORMAT_NONE))
		return FALSE;

	if (io->type == IO_FG || io->type == IO_BG)
		return system(buf) == 0;

	io->pid = popen(buf, io->type == IO_RD ? "r" : "w");
	if (!io->pid)
		return FALSE;
	io->pipe = fileno(io->pid);
	return io->pipe != -1;
run_io(struct io *io, const char **argv, const char *dir, enum io_type type)
	init_io(io, dir, type);
	if (!format_argv(io->argv, argv, FORMAT_NONE))
		return FALSE;
static int
run_io_do(struct io *io)
	return start_io(io) && done_io(io);
}
static int
run_io_bg(const char **argv)
{
	struct io io = {};
	init_io(&io, NULL, IO_BG);
	if (!format_argv(io.argv, argv, FORMAT_NONE))
		return FALSE;
	return run_io_do(&io);
}

static bool
run_io_fg(const char **argv, const char *dir)
{
	struct io io = {};

	init_io(&io, dir, IO_FG);
	if (!format_argv(io.argv, argv, FORMAT_NONE))
		return FALSE;
	return run_io_do(&io);
}
static bool
run_io_rd(struct io *io, const char **argv, enum format_flags flags)
{
	return init_io_rd(io, argv, NULL, flags) && start_io(io);
	return io->eof;
static ssize_t
io_read(struct io *io, void *buf, size_t bufsize)
{
	do {
		ssize_t readsize = read(io->pipe, buf, bufsize);

		if (readsize < 0 && (errno == EAGAIN || errno == EINTR))
			continue;
		else if (readsize == -1)
			io->error = errno;
		else if (readsize == 0)
			io->eof = 1;
		return readsize;
	} while (1);
}

	char *eol;
	ssize_t readsize;

		io->buf = io->bufpos = malloc(BUFSIZ);
		io->bufsize = 0;
	while (TRUE) {
		if (io->bufsize > 0) {
			eol = memchr(io->bufpos, '\n', io->bufsize);
			if (eol) {
				char *line = io->bufpos;

				*eol = 0;
				io->bufpos = eol + 1;
				io->bufsize -= io->bufpos - line;
				return line;
			}
		}

		if (io_eof(io)) {
			if (io->bufsize) {
				io->bufpos[io->bufsize] = 0;
				io->bufsize = 0;
				return io->bufpos;
			}
			return NULL;
		}

		if (io->bufsize > 0 && io->bufpos > io->buf)
			memmove(io->buf, io->bufpos, io->bufsize);

		io->bufpos = io->buf;
		readsize = io_read(io, io->buf + io->bufsize, io->bufalloc - io->bufsize);
		if (io_error(io))
			return NULL;
		io->bufsize += readsize;
	}
}

static bool
io_write(struct io *io, const void *buf, size_t bufsize)
{
	size_t written = 0;

	while (!io_error(io) && written < bufsize) {
		ssize_t size;

		size = write(io->pipe, buf + written, bufsize - written);
		if (size < 0 && (errno == EAGAIN || errno == EINTR))
			continue;
		else if (size == -1)
		else
			written += size;
	return written == bufsize;
static bool
run_io_buf(const char **argv, char buf[], size_t bufsize)
{
	struct io io = {};
	bool error;

	if (!run_io_rd(&io, argv, FORMAT_NONE))
		return FALSE;

	io.buf = io.bufpos = buf;
	io.bufalloc = bufsize;
	error = !io_gets(&io) && io_error(&io);
	io.buf = NULL;

	return done_io(&io) || error;
}

static int read_properties(struct io *io, const char *separators, int (*read)(char *, size_t, char *, size_t));
parse_options(int argc, const char *argv[], const char ***run_argv)
	/* XXX: This is vulnerable to the user overriding options
	 * required for the main view parser. */
	const char *custom_argv[SIZEOF_ARG] = {
		"git", "log", "--no-color", "--pretty=raw", "--parents",
			"--topo-order", NULL
	};
	int i, j = 6;
	if (!isatty(STDIN_FILENO))
	if (subcommand) {
		custom_argv[1] = subcommand;
		j = 2;
	}
		custom_argv[j++] = opt;
		if (j >= ARRAY_SIZE(custom_argv))
	custom_argv[j] = NULL;
	*run_argv = custom_argv;
	struct io io = {};
	if (!io_open(&io, path))
	if (read_properties(&io, " \t", read_option) == ERR ||
	/* Default command arguments. */
	const char **argv;
static struct view_ops diff_ops;
#define VIEW_STR(name, env, ref, ops, map, git) \
	{ name, #env, ref, ops, map, git }
	VIEW_STR(name, TIG_##id##_CMD, ref, ops, KEYMAP_##id, git)
	VIEW_(DIFF,   "diff",   &diff_ops,   TRUE,  ref_commit),
prepare_update(struct view *view, const char *argv[], const char *dir,
	       enum format_flags flags)
	if (view->pipe)
		end_update(view, TRUE);
	return init_io_rd(&view->io, argv, dir, flags);
}
static bool
prepare_update_file(struct view *view, const char *name)
{
	if (view->pipe)
		end_update(view, TRUE);
	return io_open(&view->io, name);
}
static bool
begin_update(struct view *view, bool refresh)
{
	if (refresh) {
		if (view == VIEW(REQ_VIEW_TREE) && strcmp(view->vid, view->id))
			opt_path[0] = 0;
		if (!run_io_rd(&view->io, view->ops->argv, FORMAT_ALL))
	OPEN_PREPARED = 32,	/* Open already prepared command. */
	bool reload = !!(flags & (OPEN_RELOAD | OPEN_REFRESH | OPEN_PREPARED));
		   !begin_update(view, flags & (OPEN_REFRESH | OPEN_PREPARED))) {
open_external_viewer(const char *argv[], const char *dir)
	run_io_fg(argv, dir);
	const char *mergetool_argv[] = { "git", "mergetool", file, NULL };
	open_external_viewer(mergetool_argv, NULL);
	const char *editor_argv[] = { "vi", file, NULL };
	editor_argv[0] = editor;
	open_external_viewer(editor_argv, from_root ? opt_cdup : NULL);
	const char *argv[ARRAY_SIZE(req->argv)] = { NULL };
	if (format_argv(argv, req->argv, FORMAT_ALL))
		open_external_viewer(argv, NULL);
	free_argv(argv);
		if (!VIEW(REQ_VIEW_PAGER)->pipe && !VIEW(REQ_VIEW_PAGER)->lines) {
	const char *describe_argv[] = { "git", "describe", commit_id, NULL };
	if (run_io_buf(describe_argv, refbuf, sizeof(refbuf)))
		ref = chomp_string(refbuf);
	NULL,
static const char *log_argv[SIZEOF_ARG] = {
	"git", "log", "--no-color", "--cc", "--stat", "-n100", "%(head)", NULL
};

	log_argv,
static const char *diff_argv[SIZEOF_ARG] = {
	"git", "show", "--pretty=fuller", "--no-color", "--root",
		"--patch-with-stat", "--find-copies-harder", "-C", "%(commit)", NULL
};

static struct view_ops diff_ops = {
	"line",
	diff_argv,
	NULL,
	pager_read,
	pager_draw,
	pager_request,
	pager_grep,
	pager_select,
};
	NULL,
static const char *tree_argv[SIZEOF_ARG] = {
	"git", "ls-tree", "%(commit)", "%(directory)", NULL
};

	tree_argv,
static const char *blob_argv[SIZEOF_ARG] = {
	"git", "cat-file", "blob", "%(blob)", NULL
};

	blob_argv,
static const char *blame_head_argv[] = {
	"git", "blame", "--incremental", "--", "%(file)", NULL
};

static const char *blame_ref_argv[] = {
	"git", "blame", "--incremental", "%(ref)", "--", "%(file)", NULL
};

static const char *blame_cat_file_argv[] = {
	"git", "cat-file", "blob", "%(ref):%(file)", NULL
};

	if (*opt_ref || !io_open(&view->io, opt_file)) {
		if (!run_io_rd(&view->io, blame_cat_file_argv, FORMAT_ALL))
		const char **argv = *opt_ref ? blame_ref_argv : blame_head_argv;
		if (view->lines == 0 || !run_io_rd(&io, argv, FORMAT_ALL)) {
			struct view *diff = VIEW(REQ_VIEW_DIFF);
			const char *diff_index_argv[] = {
				"git", "diff-index", "--root", "--cached",
					"--patch-with-stat", "-C", "-M",
					"HEAD", "--", view->vid, NULL
			};

			if (!prepare_update(diff, diff_index_argv, NULL, FORMAT_DASH)) {
				report("Failed to allocate diff command");
			}
			flags |= OPEN_PREPARED;
	NULL,
status_run(struct view *view, const char *argv[], char status, enum line_type type)
	struct io io = {};
	if (!run_io(&io, argv, NULL, IO_RD))
	while (!io_eof(&io)) {
		ssize_t readsize;
		readsize = io_read(&io, buf + bufsize, sizeof(buf) - bufsize);
		if (io_error(&io))
	if (io_error(&io)) {
		done_io(&io);
	done_io(&io);
static const char *status_diff_index_argv[] = {
	"git", "diff-index", "-z", "--diff-filter=ACDMRTXB",
			     "--cached", "-M", "HEAD", NULL
};
static const char *status_diff_files_argv[] = {
	"git", "diff-files", "-z", NULL
};
static const char *status_list_other_argv[] = {
	"git", "ls-files", "-z", "--others", "--exclude-standard", NULL
};
static const char *status_list_no_head_argv[] = {
	"git", "ls-files", "-z", "--cached", "--exclude-standard", NULL
};

static const char *update_index_argv[] = {
	"git", "update-index", "-q", "--unmerged", "--refresh", NULL
};
	run_io_bg(update_index_argv);
		if (!status_run(view, status_list_no_head_argv, 'A', LINE_STAT_STAGED))
	} else if (!status_run(view, status_diff_index_argv, 0, LINE_STAT_STAGED)) {
	if (!status_run(view, status_diff_files_argv, 0, LINE_STAT_UNSTAGED) ||
	    !status_run(view, status_list_other_argv, '?', LINE_STAT_UNTRACKED))
	const char *oldpath = status ? status->old.name : NULL;
	/* Diffs for unmerged entries are empty when passing the new
	 * path, so leave it empty. */
	const char *newpath = status && status->status != 'U' ? status->new.name : NULL;
	struct view *stage = VIEW(REQ_VIEW_STAGE);
			const char *no_head_diff_argv[] = {
				"git", "diff", "--no-color", "--patch-with-stat",
					"--", "/dev/null", newpath, NULL
			};

			if (!prepare_update(stage, no_head_diff_argv, opt_cdup, FORMAT_DASH))
			const char *index_show_argv[] = {
				"git", "diff-index", "--root", "--patch-with-stat",
					"-C", "-M", "--cached", "HEAD", "--",
					oldpath, newpath, NULL
			};

			if (!prepare_update(stage, index_show_argv, opt_cdup, FORMAT_DASH))
	{
		const char *files_show_argv[] = {
			"git", "diff-files", "--root", "--patch-with-stat",
				"-C", "-M", "--", oldpath, newpath, NULL
		};

		if (!prepare_update(stage, files_show_argv, opt_cdup, FORMAT_DASH))
	}
		if (!newpath) {
		if (!prepare_update_file(stage, newpath))
			return REQ_QUIT;
	open_view(view, REQ_VIEW_STAGE, OPEN_REFRESH | split);
static bool
status_update_prepare(struct io *io, enum line_type type)
	const char *staged_argv[] = {
		"git", "update-index", "-z", "--index-info", NULL
	};
	const char *others_argv[] = {
		"git", "update-index", "-z", "--add", "--remove", "--stdin", NULL
	};
		return run_io(io, staged_argv, opt_cdup, IO_WR);
		return run_io(io, others_argv, opt_cdup, IO_WR);

		return run_io(io, others_argv, NULL, IO_WR);
		return FALSE;
status_update_write(struct io *io, struct status *status, enum line_type type)
	return io_write(io, buf, bufsize);
	struct io io = {};
	if (!status_update_prepare(&io, type))
	result = status_update_write(&io, status, type);
	done_io(&io);
	struct io io = {};
	if (!status_update_prepare(&io, line->type))
		result = status_update_write(&io, line->data, line->type);
	done_io(&io);
		const char *checkout_argv[] = {
			"git", "checkout", "--", status->old.name, NULL
		};
		if (!prompt_yesno("Are you sure you want to overwrite any changes?"))
		return run_io_fg(checkout_argv, opt_cdup);
	NULL,
stage_diff_write(struct io *io, struct line *line, struct line *end)
		if (!io_write(io, line->data, strlen(line->data)) ||
		    !io_write(io, "\n", 1))
		line++;
	const char *apply_argv[SIZEOF_ARG] = {
		"git", "apply", "--whitespace=nowarn", NULL
	};
	struct io io = {};
	int argc = 3;
	if (!revert)
		apply_argv[argc++] = "--cached";
	if (revert || stage_line_type == LINE_STAT_STAGED)
		apply_argv[argc++] = "-R";
	apply_argv[argc++] = "-";
	apply_argv[argc++] = NULL;
	if (!run_io(&io, apply_argv, opt_cdup, IO_WR))
	if (!stage_diff_write(&io, diff_hdr, chunk) ||
	    !stage_diff_write(&io, chunk, view->line + view->lines))
	done_io(&io);
	run_io_bg(update_index_argv);
		if (!prepare_update_file(view, stage_status.new.name)) {
			report("Failed to open file: %s", strerror(errno));
			return REQ_NONE;
		}
	NULL,
static const char *main_argv[SIZEOF_ARG] = {
	"git", "log", "--no-color", "--pretty=raw", "--parents",
		      "--topo-order", "%(head)", NULL
};

	main_argv,
	static char buf[SIZEOF_STR];
 * Repository properties
static int
git_properties(const char **argv, const char *separators,
	       int (*read_property)(char *, size_t, char *, size_t))
{
	struct io io = {};

	if (init_io_rd(&io, argv, NULL, FORMAT_NONE))
		return read_properties(&io, separators, read_property);
	return ERR;
}

	static const char *ls_remote_argv[SIZEOF_ARG] = {
		"git", "ls-remote", ".", NULL
	};
	static bool init = FALSE;

	if (!init) {
		argv_from_env(ls_remote_argv, "TIG_LS_REMOTE");
		init = TRUE;
	}
	return git_properties(ls_remote_argv, "\t", read_ref);
	const char *config_list_argv[] = { "git", GIT_CONFIG, "--list", NULL };

	return git_properties(config_list_argv, "=", read_repo_config_option);
		string_ncopy(opt_cdup, name, namelen);
	const char *head_argv[] = {
		"git", "symbolic-ref", "HEAD", NULL
	};
	const char *rev_parse_argv[] = {
		"git", "rev-parse", "--git-dir", "--is-inside-work-tree",
			"--show-cdup", NULL
	};
	if (run_io_buf(head_argv, opt_head, sizeof(opt_head))) {
		chomp_string(opt_head);
		if (!prefixcmp(opt_head, "refs/heads/")) {
			char *offset = opt_head + STRING_SIZE("refs/heads/");
			memmove(opt_head, offset, strlen(offset) + 1);
		}
	}
	return git_properties(rev_parse_argv, "=", read_repo_info);
read_properties(struct io *io, const char *separators,
	if (!start_io(io))
	while (state == OK && (name = io_gets(io))) {
	if (state != ERR && io_error(io))
	done_io(io);
	const char **run_argv = NULL;
	request = parse_options(argc, argv, &run_argv);
		argv_from_env(view->ops->argv, view->cmd_env);
	if (request == REQ_VIEW_PAGER || run_argv) {
		if (request == REQ_VIEW_PAGER)
			io_open(&VIEW(request)->io, "");
		else if (!prepare_update(VIEW(request), run_argv, NULL, FORMAT_NONE))
			die("Failed to format arguments");
		open_view(NULL, request, OPEN_PREPARED);
		request = REQ_NONE;
	}

			if (cmd) {
				struct view *next = VIEW(REQ_VIEW_PAGER);
				const char *argv[SIZEOF_ARG] = { "git" };
				int argc = 1;

				/* When running random commands, initially show the
				 * command in the title. However, it maybe later be
				 * overwritten if a commit line is selected. */
				string_ncopy(next->ref, cmd, strlen(cmd));

				if (!argv_from_string(argv, &argc, cmd)) {
					report("Too many arguments");
				} else if (!prepare_update(next, argv, NULL, FORMAT_DASH)) {
					report("Failed to format command");
					open_view(view, REQ_VIEW_PAGER, OPEN_PREPARED);