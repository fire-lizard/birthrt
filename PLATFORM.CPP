/* ========================================================================
   PLATFORM.CPP - host-specific implementations behind PLATFORM.H.
   ======================================================================== */
#include "PLATFORM.H"

#include <string.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

#if defined(_WIN32)

char *plat_copy_path(char *dst, const char *src, unsigned long len)
{
	strncpy(dst, src, len);
	dst[len - 1] = '\0';
	return dst;
}

vector<string> PlatListFiles(const char *pattern)
{
	vector<string> names;
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(pattern, &fd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				names.push_back(fd.cFileName);
		} while (FindNextFile(hFind, &fd));
		FindClose(hFind);
	}

	return names;
}

#else	/* ---------------- Linux / macOS ---------------- */

#include <ctype.h>
#include <dirent.h>
#include <stdarg.h>
#include <time.h>

void Sleep(unsigned long ms)
{
	struct timespec ts;
	ts.tv_sec = (time_t)(ms / 1000);
	ts.tv_nsec = (long)(ms % 1000) * 1000000L;
	nanosleep(&ts, NULL);
}

unsigned long GetTickCount(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (unsigned long)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

int MessageBox(HWND, const char *text, const char *caption, unsigned)
{
	fprintf(stderr, "[%s] %s\n", caption ? caption : "Birthright", text ? text : "");
	return IDOK;
}

void OutputDebugString(const char *s)
{
	fputs(s, stderr);
}

int wsprintf(char *dst, const char *fmt, ...)
{
	va_list ap;
	int n;

	va_start(ap, fmt);
	n = vsprintf(dst, fmt, ap);
	va_end(ap);
	return n;
}

char *plat_strupr(char *s)
{
	char *p;

	for (p = s; *p; ++p)
		*p = (char)toupper((unsigned char)*p);
	return s;
}

long plat_filelength(int fd)
{
	struct stat st;

	if (fstat(fd, &st) != 0)
		return -1;
	return (long)st.st_size;
}

/* Case-insensitive lookup of one path component inside `dir`.
   ponytail: linear readdir per component. The game opens a few thousand
   files over a session, so it has never been worth a cached index; build
   one here if load times ever show up in a profile. */
static bool match_component(const string &dir, const string &want, string &found)
{
	DIR *d = opendir(dir.empty() ? "." : dir.c_str());
	struct dirent *e;

	if (!d)
		return false;

	while ((e = readdir(d)) != NULL)
	{
		if (strcasecmp(e->d_name, want.c_str()) == 0)
		{
			found = e->d_name;
			closedir(d);
			return true;
		}
	}

	closedir(d);
	return false;
}

char *plat_copy_path(char *dst, const char *src, unsigned long len)
{
	string in(src ? src : "");
	string out;
	size_t i = 0;

	/* the data files ship with Win32 separators baked into the source */
	for (size_t k = 0; k < in.size(); ++k)
		if (in[k] == '\\')
			in[k] = '/';

	if (!in.empty() && in[0] == '/')
	{
		out = "/";
		i = 1;
	}

	while (i < in.size())
	{
		size_t slash = in.find('/', i);
		string part = in.substr(i, slash == string::npos ? string::npos : slash - i);

		if (!part.empty() && part != ".")
		{
			string probe = out + part;

			if (access(probe.c_str(), F_OK) != 0)
			{
				string base = out.empty() ? string(".") : out;
				string real;

				if (match_component(base, part, real))
					part = real;
			}

			out += part;
		}

		if (slash == string::npos)
			break;

		out += "/";
		i = slash + 1;
	}

	strncpy(dst, out.c_str(), len);
	dst[len - 1] = '\0';
	return dst;
}

vector<string> PlatListFiles(const char *pattern)
{
	vector<string> names;
	string pat(pattern ? pattern : "");
	string dir, prefix, suffix;
	size_t star, slash;
	DIR *d;
	struct dirent *e;

	for (size_t k = 0; k < pat.size(); ++k)
		if (pat[k] == '\\')
			pat[k] = '/';

	slash = pat.rfind('/');
	dir = (slash == string::npos) ? "." : pat.substr(0, slash);
	pat = (slash == string::npos) ? pat : pat.substr(slash + 1);

	star = pat.find('*');
	if (star == string::npos)
	{
		prefix = pat;
	}
	else
	{
		prefix = pat.substr(0, star);
		suffix = pat.substr(star + 1);
	}

	d = opendir(dir.c_str());
	if (!d)
		return names;

	while ((e = readdir(d)) != NULL)
	{
		string name(e->d_name);
		struct stat st;
		string full = dir + "/" + name;

		if (name.size() < prefix.size() + suffix.size())
			continue;
		if (strncasecmp(name.c_str(), prefix.c_str(), prefix.size()) != 0)
			continue;
		if (!suffix.empty() &&
			strcasecmp(name.c_str() + name.size() - suffix.size(), suffix.c_str()) != 0)
			continue;
		if (stat(full.c_str(), &st) == 0 && S_ISDIR(st.st_mode))
			continue;

		names.push_back(name);
	}

	closedir(d);
	return names;
}

#endif	/* _WIN32 */
