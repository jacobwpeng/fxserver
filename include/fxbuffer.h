/*
 * =====================================================================================
 *       Filename:  fxbuffer.h
 *        Created:  15:27:32 Mar 10, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  Buffer class used for convenient
 *
 * =====================================================================================
 */

#ifndef  __FXBUFFER_H__
#define  __FXBUFFER_H__

class FXBuffer
{
    public:
        FXBuffer(size_t reserved_size = BUFFER_INCREASE_STEP);
        ~FXBuffer();

        void Append(const char * addr, size_t len);

        void Clear();
        void Shrink();
        void Swap(FXBuffer & other);

        /*-----------------------------------------------------------------------------
         *  Getter
         *-----------------------------------------------------------------------------*/
        char * Begin();
        size_t Capacity() const;
        const char * Begin() const;
        const char * Read() const;
        size_t BytesToRead() const;

        std::string ReadAndClear();

    private:
        static const size_t BUFFER_INCREASE_STEP = 1 << 20;
        void EnsureSpace(size_t append_len);

        std::vector<char> buf_;
        size_t read_index_;
        size_t write_index_;
};

#endif   /* ----- #ifndef __FXBUFFER_H__----- */
