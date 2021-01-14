/************************************************************************
*
*  Module:       IoBuf.h
*  Long name:    CIoBuf class
*  Description:  definition of an USB I/O buffer and buffer pool
*
*  Runtime Env.: Win32, Part of UsbioLib
*  Author(s):    Guenter Hildebrandt, Udo Eberhardt, Mario Guenther
*  Company:      Thesycon GmbH, Ilmenau
************************************************************************/

#ifndef _IoBuf_h_
#define _IoBuf_h_

#define MAX_BUFF_COUNT  10
#define MAX_BUFF_SIZE  17000

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// CIoBuf
// used as a buffer descriptor
class CIoBuf
{
   friend class CIoBufPool;
   
   public:
      // standard constructor
      CIoBuf();
      
      // construct and attach an existing buffer
      CIoBuf(void* pBuffer, int nBufferSize);
      
      // construct and allocate a buffer internally
      CIoBuf(int nBufferSize);
      
      // destructor
      ~CIoBuf();
      
      // get buffer pointer, NULL if no buffer is attached
      void* Buffer() { return m_pBufferMem; }
      
      // get buffer size, in bytes
      int Size() { return m_nBufferSize; }
      
      // public member variables
   public:
      // final status of the read or write operation
      // will be set *after* the read or write operation is completed
      CIoBuf* Next;
      // implementation:
   protected:
      BOOL bEmpty;
      // init member variables
      // helper function, used by constructors
      void Init();
   public :
      
      // buffer pointer, NULL if no buffer is attached 
      void* m_pBufferMem;
      
      // buffer size, in bytes
      int m_nBufferSize;

      // buffer size, in bytes
      int m_nTransferdSize;
      
      // TRUE if buffer was allocated by constructor
      bool m_bBufferMemAllocated;  

}; // class CIoBuf

//------------------------------------------------------------------------------------------
// CIoBufPool
// implements a pool of CIoBuf objects

class CIoBufPool
{
   public:
      // standard constructor
      CIoBufPool();
      // destructor
      ~CIoBufPool();
      
      // allocate all elements of the buffer pool
      // returns TRUE for success, FALSE otherwise
      int Allocate( int SizeOfBuffer, int NumberOfBuffers );
      
      // free all elements of the buffer pool
      void Free();

      // free all elements of the buffer pool
      void Reset();
      
      // get a buffer from pool
      // returns NULL if the pool is empty
      CIoBuf* GetEmptyBuf();
      void ReleaseEmptyBuf();
      
      // put a buffer back to the pool
      CIoBuf* GetFilledBuf();
      void ReleaseFilledBuf();
      
      // returns the current number of buffers in the pool
      int  GetCount()    { return m_nNumberOfBuffers; }
      
      // implementation:
   protected:
      // protects the pool
      CRITICAL_SECTION CritSect;

            
      CIoBuf* m_pocFillBuff;
      CIoBuf* m_pocEmptyBuff;
      CIoBuf* m_pocBufArray;

      // current number of buffers in pool
      int m_nNumberOfBuffers;

      // allocated buffer memory
      char* m_pchBufferMemory;

}; // class CIoBufPool


#endif // _IoBuf_h_

/*************************** EOF **************************************/
