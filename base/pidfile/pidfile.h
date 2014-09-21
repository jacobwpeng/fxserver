/*
 * =====================================================================================
 *
 *       Filename:  pidfile.h
 *        Created:  08/10/14 09:51:25
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __PIDFILE_H__
#define  __PIDFILE_H__

#include <boost/noncopyable.hpp>

namespace fx
{
    namespace base
    {
        class PidFile : boost::noncopyable
        {
            public:
                ~PidFile();
                static PidFile * Instance();
                //for signal handle
                void DeletePidFile();
            private:
                PidFile(const char * prog_name);
                void CheckIfAlreadyExists() const;
                void CreatePidFile() const;
                static void ErrorOutputAndExit(const char * fmt, ...);

            private:
                bool deleted_;
                char full_path_[512];
                    
        };
    }
}

#endif   /* ----- #ifndef __PIDFILE_H__  ----- */
