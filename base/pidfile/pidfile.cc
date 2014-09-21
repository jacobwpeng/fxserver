/*
 * =====================================================================================
 *
 *       Filename:  pidfile.cc
 *        Created:  08/10/14 09:50:55
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "pidfile.h"
#include <cstdio>
#include <cstdarg>

extern char *program_invocation_short_name;

fx::base::PidFile * fx::base::PidFile::Instance()
{
    static PidFile pidfile(program_invocation_short_name);
    return &pidfile;
}

fx::base::PidFile::PidFile(const char * prog_name)
    :deleted_(false)
{
    char * cwd = get_current_dir_name();
    snprintf(full_path_, sizeof(full_path_), "%s/%s.pid", cwd, prog_name);
    free(cwd);

    CheckIfAlreadyExists();
    CreatePidFile();
}

fx::base::PidFile::~PidFile()
{
    if (not deleted_) DeletePidFile();
}

void fx::base::PidFile::CheckIfAlreadyExists() const
{
    if (::access(full_path_, F_OK) == 0)
    {
        PidFile::ErrorOutputAndExit("%s already exists.\n", full_path_);
    }
}

void fx::base::PidFile::CreatePidFile() const
{
    FILE * fp = fopen(full_path_, "w");
    if (fp == NULL)
    {
        PidFile::ErrorOutputAndExit("cannot open %s for writing.\n", full_path_);
    }
    pid_t pid = getpid();
    fprintf(fp, "%d", pid);
    fclose(fp);
}

void fx::base::PidFile::DeletePidFile()
{
    int ret = unlink(full_path_);
    if (ret != 0)
    {
        perror("delete pidfile failed.");
    }
    deleted_ = true;
}

void fx::base::PidFile::ErrorOutputAndExit(const char * fmt, ...)
{
    char buf[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    fputs(buf, stderr);
    va_end(args);
    exit(-1);
}
