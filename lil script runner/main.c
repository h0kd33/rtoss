/*
 * LIL - Little Interpreted Language
 * Copyright (C) 2010 Kostas Michalopoulos
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *	claim that you wrote the original software. If you use this software
 *	in a product, an acknowledgment in the product documentation would be
 *	appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *	misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * Kostas Michalopoulos <badsector@runtimelegend.com>
 */

#define _BSD_SOURCE
#ifndef WIN32
#include <unistd.h>
#include <limits.h>
#define MAX_PATH PATH_MAX
#include <sys/stat.h>
#else
#include <direct.h>
#include <windows.h>
#include <shlobj.h>
#endif
#ifdef __MINGW32__
/* allow the definition of popen, etc */
#undef __STRICT_ANSI__
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lil.h"

static int running = 1;
static int exit_code = 0;

static LILCALLBACK void do_exit(lil_t lil, lil_value_t val)
{
	running = 0;
	exit_code = (int)lil_to_integer(val);
}

/* Start of extended functions */

static char* do_specialdir(int nCSIDL)
{
	char* retval = NULL;
	#ifdef WIN32
	LPITEMIDLIST pidl;
	LPMALLOC pShellMalloc;
	if(SUCCEEDED(SHGetMalloc(&pShellMalloc))) {
		if(SUCCEEDED(SHGetSpecialFolderLocation(NULL,nCSIDL,&pidl))) {
			retval = malloc(MAX_PATH);
			SHGetPathFromIDList(pidl, retval);
			pShellMalloc->lpVtbl->Free(pShellMalloc,pidl);
		}
		pShellMalloc->lpVtbl->Release(pShellMalloc);
	}
	#endif
	return retval;
}

static char* do_lilpath()
{
	char* retval = NULL;
	#ifdef WIN32
	retval = malloc(MAX_PATH);
	GetModuleFileName(NULL,retval,MAX_PATH);
	#else
	struct stat info;
	char* ppath = malloc(PATH_MAX);
	pid_t pid = getpid();
	retval = malloc(PATH_MAX);
	sprintf(ppath, "/proc/%d/exe", pid);
	readlink(ppath, retval, PATH_MAX);
	free(ppath);
	#endif
	return retval;
}

static char* do_getcwd()
{
	char* retval = NULL;
	retval = malloc(MAX_PATH);
	getcwd(retval,MAX_PATH);
	return retval;
}

static char* do_getenv(size_t argc, char** argv)
{
	char* retval = NULL;
	if(argc) {
		retval = malloc(32768);
	#ifdef WIN32
		GetEnvironmentVariable(argv[0],retval,32768);
	#else
		char* pEnv;
		pEnv = getenv(argv[0]);
		strcpy(retval,pEnv);
	#endif
	}
	return retval;
}

static void do_setenv(size_t argc, char** argv)
{
	if(argc > 1) {
	#ifdef WIN32
		SetEnvironmentVariable(argv[0],argv[1]);
	#else
		setenv(argv[0],argv[1], 1);
	#endif
	}
}

static lil_value_t do_dirlist(size_t argc, char** argv)
{
	lil_value_t rv = NULL;
#ifdef WIN32
	lil_list_t flist = lil_alloc_list();
	lil_value_t r = NULL;


	WIN32_FIND_DATAA findFileData;
	HANDLE MyHandle = FindFirstFileA(argv[0],&findFileData);

	if( MyHandle != INVALID_HANDLE_VALUE)
	{
		r = lil_alloc_string(findFileData.cFileName);
		lil_list_append(flist, r);

		while(FindNextFileA(MyHandle,&findFileData) != 0)
		{
			r = lil_alloc_string(findFileData.cFileName);
			lil_list_append(flist, r);
		}
	}
	rv = lil_list_to_value(flist, 1);
	lil_free_list(flist);
#else
	/* unimplenemted */
#endif
	return rv;
}

static void do_chdir(size_t argc, char** argv)
{
	chdir(argv[0]);
}

static void do_unlink(size_t argc, char** argv)
{
	unlink(argv[0]);
}

static void do_renamefile(size_t argc, char** argv)
{
	if(argc > 1) {
		rename(argv[0],argv[1]);
	}
}

static void do_copyfile(size_t argc, char** argv)
{
	if(argc > 1) {
	#ifdef WIN32
		CopyFile(argv[0],argv[1],FALSE);
	#else
		/* unimplenemted */
	#endif
	}
}

static void do_mkdir(size_t argc, char** argv)
{
	mkdir(argv[0]);
}

static void do_rmdir(size_t argc, char** argv)
{
	rmdir(argv[0]);
}

static void do_shellopen(size_t argc, char** argv)
{
	#if defined(WIN32) || defined(WATCOMC)
	int i = 0;
	for (i=0; i<argc; i++) {
		ShellExecute(NULL, "open", argv[i], NULL, NULL, SW_SHOWNORMAL);
	}
	#endif
}

static void do_exec(size_t argc, char** argv)
{
	#if defined(WIN32) || defined(WATCOMC)
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	char* cmd = NULL;
	int cmdlen = 0;
	int i = 0;
	for (i=0; i<argc; i++) {
		size_t len = strlen(argv[i]);
		if (i != 0) {
			cmd = realloc(cmd, cmdlen + 1);
			cmd[cmdlen++] = ' ';
		}
		cmd = realloc(cmd, cmdlen + len);
		memcpy(cmd + cmdlen, argv[i], len);
		cmdlen += len;
	}
	cmd = realloc(cmd, cmdlen + 1);
	cmd[cmdlen] = 0;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );
	
	if( CreateProcess( NULL,   // No module name (use command line)
		cmd,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi )           // Pointer to PROCESS_INFORMATION structure
	) {
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
	}
	free(cmd);
	#endif
}

static char* do_system(size_t argc, char** argv)
{
	char* retval = NULL;
	size_t size = 0;
	char buff[1024];
	char* cmd = NULL;
	int cmdlen = 0;
	int i = 0;

	#if defined(WIN32) || defined(WATCOMC)
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	HANDLE rPipe, wPipe;
	BOOL res;
	DWORD resRead;
	SECURITY_ATTRIBUTES sa;
	#endif

	for (i=0; i<argc; i++) {
		size_t len = strlen(argv[i]);
		if (i != 0) {
			cmd = realloc(cmd, cmdlen + 1);
			cmd[cmdlen++] = ' ';
		}
		cmd = realloc(cmd, cmdlen + len);
		memcpy(cmd + cmdlen, argv[i], len);
		cmdlen += len;
	}
	cmd = realloc(cmd, cmdlen + 1);
	cmd[cmdlen] = 0;

	#if defined(WIN32) || defined(WATCOMC)

	// Set the security attributes for the pipe handles created 
	sa.nLength = sizeof(SECURITY_ATTRIBUTES); 
	sa.bInheritHandle = TRUE; 
	sa.lpSecurityDescriptor = NULL; 
	CreatePipe(&rPipe,&wPipe,&sa,0);

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdOutput = wPipe; 
	ZeroMemory(&pi, sizeof(pi));

	/* Start the child process. */
	if(CreateProcess(NULL, /* module name. */
		(LPSTR)cmd,	/* Command line.  */
		NULL,	/* Process handle not inheritable. */
		NULL,	/* Thread handle not inheritable. */
		TRUE,	/* Set handle inheritance. */
		0,	   /* No creation flags. */
		NULL,	/* Use parent's environment block. */
		NULL,	/* Use parent's starting directory. */
		&si,	 /* Pointer to STARTUPINFO structure. */
		&pi)	 /* Pointer to PROCESS_INFORMATION structure.*/
	){
		CloseHandle(wPipe);

		//now read the output pipe here.
		do
		{
			res = ReadFile(rPipe,buff,sizeof(buff),&resRead,0);
			if( resRead > 0 ){
				retval = realloc(retval, size + resRead);
				memcpy(retval + size, buff, resRead);
				size += resRead;
			}
		}while(res);

		//GetExitCodeProcess(pi.hProcess, &exitCode);

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	free(cmd);
	return retval;
	#else
	FILE* p;
	p = popen(cmd, "r");
	free(cmd);
	if (p) {
		char* retval = NULL;
		size_t size = 0;
		char buff[1024];
		ssize_t bytes;
		while ((bytes = fread(buff, 1, 1024, p))) {
			retval = realloc(retval, size + bytes);
			memcpy(retval + size, buff, bytes);
			size += bytes;
		}
		retval = realloc(retval, size + 1);
		retval[size] = 0;
		pclose(p);
		return retval;
	} else {
		return NULL;
	}
	#endif
}

/* End of extended functions */

static void conv_argv(size_t* argc, lil_value_t* argv, char*** sargv, char* defarg)
{
	size_t i;
	if(defarg && !*argc)
	{
		*sargv = malloc(sizeof(char*)*((++*argc) + 1));
		((char**)*sargv)[0] = defarg;
	}
	else
	{
		*sargv = malloc(sizeof(char*)*((*argc) + 1));
		for (i=0; i<*argc; i++)
			((char**)*sargv)[i] = (char*)lil_to_string(argv[i]);
	}
	((char**)*sargv)[*argc] = NULL;
}

/* Start of glue functions */

static LILCALLBACK lil_value_t fnc_writechar(lil_t lil, size_t argc, lil_value_t* argv)
{
	if (!argc) return NULL;
	printf("%c", (char)lil_to_integer(argv[0]));
	return NULL;
}

static LILCALLBACK lil_value_t fnc_system(lil_t lil, size_t argc, lil_value_t* argv)
{
	char** sargv = NULL;
	lil_value_t r = NULL;
	char* rv;
	size_t i;
	if (argc == 0) return NULL;
	conv_argv(&argc,argv,&sargv,NULL);
	rv = do_system(argc, (char**)sargv);
	if (rv) {
		r = lil_alloc_string(rv);
		free(rv);
	}
	free(sargv);
	return r;
}

static LILCALLBACK lil_value_t fnc_exec(lil_t lil, size_t argc, lil_value_t* argv)
{
	char** sargv = NULL;
	lil_value_t r = NULL;
	size_t i;
	if (argc == 0) return NULL;
	conv_argv(&argc,argv,&sargv,NULL);
	do_exec(argc, (char**)sargv);
	return NULL;
}

static LILCALLBACK lil_value_t fnc_shellopen(lil_t lil, size_t argc, lil_value_t* argv)
{
	char** sargv = NULL;
	lil_value_t r = NULL;
	size_t i;
	if (argc == 0) return NULL;
	conv_argv(&argc,argv,&sargv,NULL);
	do_shellopen(argc, (char**)sargv);
	return NULL;
}

static LILCALLBACK lil_value_t fnc_specialdir(lil_t lil, size_t argc, lil_value_t* argv)
{
	lil_value_t r = NULL;
	char* rv;
	size_t i;
	if (argc == 0) return NULL;
	i = lil_to_integer(argv[0]);
	rv = do_specialdir(i);
	if (rv) {
		r = lil_alloc_string(rv);
		free(rv);
	}
	return r;
}

static LILCALLBACK lil_value_t fnc_getcwd(lil_t lil, size_t argc, lil_value_t* argv)
{
	lil_value_t r = NULL;
	char* rv;
	rv = do_getcwd();
	if (rv) {
		r = lil_alloc_string(rv);
		free(rv);
	}
	return r;
}

static LILCALLBACK lil_value_t fnc_lilpath(lil_t lil, size_t argc, lil_value_t* argv)
{
	lil_value_t r = NULL;
	char* rv;
	rv = do_lilpath();
	if (rv) {
		r = lil_alloc_string(rv);
		free(rv);
	}
	return r;
}

static LILCALLBACK lil_value_t fnc_getenv(lil_t lil, size_t argc, lil_value_t* argv)
{
	char** sargv = NULL;
	lil_value_t r = NULL;
	char* rv;
	size_t i;
	if (argc == 0) return NULL;
	conv_argv(&argc,argv,&sargv,NULL);
	rv = do_getenv(argc, (char**)sargv);
	if (rv) {
		r = lil_alloc_string(rv);
		free(rv);
	}
	free(sargv);
	return r;
}

static LILCALLBACK lil_value_t fnc_setenv(lil_t lil, size_t argc, lil_value_t* argv)
{
	char** sargv = NULL;
	lil_value_t r = NULL;
	char* rv;
	size_t i;
	if (argc == 0) return NULL;
	conv_argv(&argc,argv,&sargv,NULL);
	sargv[argc] = NULL;
	do_setenv(argc, (char**)sargv);
	free(sargv);
	return NULL;
}

static LILCALLBACK lil_value_t fnc_dirlist(lil_t lil, size_t argc, lil_value_t* argv)
{
	char** sargv = NULL;
	lil_value_t r = NULL;
	size_t i;
	conv_argv(&argc,argv,&sargv,"*.*");
	r = do_dirlist(argc, (char**)sargv);
	free(sargv);
	return r;
}

static LILCALLBACK lil_value_t fnc_chdir(lil_t lil, size_t argc, lil_value_t* argv)
{
	char** sargv = NULL;
	lil_value_t r = NULL;
	char* rv;
	size_t i;
	if (argc == 0) return NULL;
	conv_argv(&argc,argv,&sargv,NULL);
	do_chdir(argc, (char**)sargv);
	free(sargv);
	return NULL;
}

static LILCALLBACK lil_value_t fnc_unlink(lil_t lil, size_t argc, lil_value_t* argv)
{
	char** sargv = NULL;
	lil_value_t r = NULL;
	char* rv;
	size_t i;
	if (argc == 0) return NULL;
	conv_argv(&argc,argv,&sargv,NULL);
	do_unlink(argc, (char**)sargv);
	free(sargv);
	return NULL;
}

static LILCALLBACK lil_value_t fnc_renamefile(lil_t lil, size_t argc, lil_value_t* argv)
{
	char** sargv = NULL;
	lil_value_t r = NULL;
	char* rv;
	size_t i;
	if (argc == 0) return NULL;
	conv_argv(&argc,argv,&sargv,NULL);
	do_renamefile(argc, (char**)sargv);
	free(sargv);
	return NULL;
}

static LILCALLBACK lil_value_t fnc_copyfile(lil_t lil, size_t argc, lil_value_t* argv)
{
	char** sargv = NULL;
	lil_value_t r = NULL;
	char* rv;
	size_t i;
	if (argc == 0) return NULL;
	conv_argv(&argc,argv,&sargv,NULL);
	do_copyfile(argc, (char**)sargv);
	free(sargv);
	return NULL;
}

static LILCALLBACK lil_value_t fnc_mkdir(lil_t lil, size_t argc, lil_value_t* argv)
{
	char** sargv = NULL;
	lil_value_t r = NULL;
	char* rv;
	size_t i;
	if (argc == 0) return NULL;
	conv_argv(&argc,argv,&sargv,NULL);
	do_mkdir(argc, (char**)sargv);
	free(sargv);
	return NULL;
}

static LILCALLBACK lil_value_t fnc_rmdir(lil_t lil, size_t argc, lil_value_t* argv)
{
	char** sargv = NULL;
	lil_value_t r = NULL;
	char* rv;
	size_t i;
	if (argc == 0) return NULL;
	conv_argv(&argc,argv,&sargv,NULL);
	do_rmdir(argc, (char**)sargv);
	free(sargv);
	return NULL;
}

static LILCALLBACK lil_value_t fnc_readline(lil_t lil, size_t argc, lil_value_t* argv)
{
	size_t len = 0, size = 64;
	char* buffer = malloc(size);
	signed char ch;
	lil_value_t retval;
	for (;;) {
		ch = fgetc(stdin);
		if (ch == EOF) break;
		if (ch == '\r') continue;
		if (ch == '\n') break;
		if (len < size) {
			size += 64;
			buffer = realloc(buffer, size);
		}
		buffer[len++] = ch;
	}
	buffer = realloc(buffer, len + 1);
	buffer[len] = 0;
	retval = lil_alloc_string(buffer);
	free(buffer);
	return retval;
}

/* End of glue functions */

static int repl(void)
{
	char buffer[16384];
	lil_t lil = lil_new();
	lil_register(lil, "writechar", fnc_writechar);
	lil_register(lil, "system", fnc_system);
	lil_register(lil, "getenv", fnc_getenv);
	lil_register(lil, "setenv", fnc_setenv);
	lil_register(lil, "lilpath", fnc_lilpath);
	lil_register(lil, "getcwd", fnc_getcwd);
	lil_register(lil, "chdir", fnc_chdir);
	lil_register(lil, "unlink", fnc_unlink);
	lil_register(lil, "renamefile", fnc_renamefile);
	lil_register(lil, "copyfile", fnc_copyfile);
	lil_register(lil, "dirlist", fnc_dirlist);
	lil_register(lil, "mkdir", fnc_mkdir);
	lil_register(lil, "rmdir", fnc_rmdir);
	lil_register(lil, "specialdir", fnc_specialdir);
	lil_register(lil, "exec", fnc_exec);
	lil_register(lil, "shellopen", fnc_shellopen);
	lil_register(lil, "readline", fnc_readline);
	printf("Little Interpreted Language Interactive Shell\n");
	lil_callback(lil, LIL_CALLBACK_EXIT, (lil_callback_proc_t)do_exit);
	while (running) {
		lil_value_t result;
		const char* strres;
		const char* err_msg;
		size_t pos;
		buffer[0] = 0;
		printf("# ");
		if (!fgets(buffer, 16384, stdin)) break;
		result = lil_parse(lil, buffer, 0, 0);
		strres = lil_to_string(result);
		if (strres[0])
			printf("%s\n", strres);
		lil_free_value(result);
		if (lil_error(lil, &err_msg, &pos)) {
			printf("error at %i: %s\n", (int)pos, err_msg);
		}
	}
	lil_free(lil);
	return exit_code;
}

static int nonint(int argc, const char* argv[])
{
	lil_t lil = lil_new();
	const char* filename = argv[1];
	const char* err_msg;
	size_t pos;
	lil_list_t arglist = lil_alloc_list();
	lil_value_t args, result;
	char* tmpcode;
	int i;
	lil_register(lil, "writechar", fnc_writechar);
	lil_register(lil, "system", fnc_system);
	lil_register(lil, "getenv", fnc_getenv);
	lil_register(lil, "setenv", fnc_setenv);
	lil_register(lil, "lilpath", fnc_lilpath);
	lil_register(lil, "getcwd", fnc_getcwd);
	lil_register(lil, "chdir", fnc_chdir);
	lil_register(lil, "unlink", fnc_unlink);
	lil_register(lil, "renamefile", fnc_renamefile);
	lil_register(lil, "copyfile", fnc_copyfile);
	lil_register(lil, "dirlist", fnc_dirlist);
	lil_register(lil, "mkdir", fnc_mkdir);
	lil_register(lil, "rmdir", fnc_rmdir);
	lil_register(lil, "exec", fnc_exec);
	lil_register(lil, "shellopen", fnc_shellopen);
	lil_register(lil, "specialdir", fnc_specialdir);
	for (i=2; i<argc; i++) {
		lil_list_append(arglist, lil_alloc_string(argv[i]));
	}
	args = lil_list_to_value(arglist, 1);
	lil_free_list(arglist);
	lil_set_var(lil, "argv", args, LIL_SETVAR_GLOBAL);
	lil_free_value(args);
	tmpcode = malloc(strlen(filename) + 256);
	sprintf(tmpcode, "set __lilmain:code__ [read {%s}]\nif [streq $__lilmain:code__ ''] {print There is no code in the file or the file does not exist} {eval $__lilmain:code__}\n", filename);
	result = lil_parse(lil, tmpcode, 0, 1);
	free(tmpcode);
	lil_free_value(result);
	if (lil_error(lil, &err_msg, &pos)) {
		fprintf(stderr, "lil: error at %i: %s\n", (int)pos, err_msg);
	}
	lil_free(lil);
	return exit_code;
}

int main(int argc, const char* argv[])
{
	if (argc < 2) return repl();
	else return nonint(argc, argv);
}
