/************************************************************************
*
*  Module:       UsbIoBuf.cpp
*  Long name:    CIoBuf class
*  Description:  implementation of an USB I/O buffer and buffer pool
*                used by CUsbIoPipe::Read and CUsbIoPipe::Write
*
*  Runtime Env.: Win32, Part of UsbioLib
*  Author(s):    Guenter Hildebrandt, Udo Eberhardt
*  Company:      Thesycon GmbH, Ilmenau
************************************************************************/

#include "stdAfx.h"
#include "Iobuf.h"


// CIoBuf
CIoBuf::CIoBuf()
{
   Init();
} 


CIoBuf::CIoBuf(void *pBuffer, int nBuffersize)
{
   Init();
   m_pBufferMem = pBuffer;
   m_nBufferSize = nBuffersize;
   
} 

CIoBuf::CIoBuf(int nBuffersize)
{
   Init();
   if ( nBuffersize != 0 ) 
   {
      m_pBufferMem = new char[nBuffersize];
      m_nBufferSize = nBuffersize;
      m_bBufferMemAllocated = true;
   }
   m_nTransferdSize = 0;
} 

CIoBuf::~CIoBuf()
{
   if ( m_bBufferMemAllocated && m_pBufferMem!=0 ) 
   {
      delete [] m_pBufferMem;
      m_pBufferMem = 0;
   }
   
}

void CIoBuf::Init()
{
   m_bBufferMemAllocated = false;
   m_pBufferMem = 0;
   m_nBufferSize = 0;   
   m_nTransferdSize = 0;   
} 

//--------------------------------------------------------------------------
// CIoBufPool
CIoBufPool::CIoBufPool()
{
   m_nNumberOfBuffers = 0;
   m_pchBufferMemory = 0;
   m_pocBufArray = 0;
   
   InitializeCriticalSection(&CritSect);
}


CIoBufPool::~CIoBufPool()
{
   Free();
   DeleteCriticalSection(&CritSect);
}

int CIoBufPool::Allocate(int nSizeOfBuffer, int nNumberOfBuffers)
{
   int i;
   bool succ = false;
   int nSize;
   char *pMem;
   m_nNumberOfBuffers = 0;
   
   EnterCriticalSection(&CritSect);
   
   if ( m_pocBufArray!=0 )
   {
      // is already allocated
   }
   else 
   {     
      m_pocBufArray = new CIoBuf[nNumberOfBuffers];
      if ( m_pocBufArray==0 ) 
      {
	 // failed
      }
      else
      {
	 nSize = nSizeOfBuffer * nNumberOfBuffers;
	 // alloc buffer memory
	 m_pchBufferMemory = new char[nSize];
	 if ( m_pchBufferMemory==0 ) 
	 {
	    // failed
	    delete [] m_pocBufArray;
	    m_pocBufArray = 0;
	 } 
	 else 
	 {        
	    pMem = m_pchBufferMemory;
	     // init all buffer descriptors
	    for ( i=0; i < nNumberOfBuffers; i++ ) 
	    {
	       m_pocBufArray[i].m_pBufferMem = pMem + ( i * nSizeOfBuffer);
	       m_pocBufArray[i].m_nBufferSize = nSizeOfBuffer;
	       m_pocBufArray[i].bEmpty = TRUE;
	       if( i > 0  )
		  m_pocBufArray[i - 1].Next = &m_pocBufArray[ i ];

	    }	    
	    m_pocBufArray[ i - 1 ].Next = &m_pocBufArray[ 0 ];
	    // success
	    m_nNumberOfBuffers = nNumberOfBuffers;
	    succ = true;
	    
	 }
      }
   }
   Reset();

   LeaveCriticalSection(&CritSect);
   
   return succ;
}

void CIoBufPool::Reset()
{
   EnterCriticalSection(&CritSect);
   for ( int i=0; i < m_nNumberOfBuffers; i++ ) 
      m_pocBufArray[i].bEmpty = TRUE;
   m_pocEmptyBuff = &m_pocBufArray[0];
   m_pocFillBuff = NULL;
   LeaveCriticalSection(&CritSect);
}

void CIoBufPool::Free()
{
   EnterCriticalSection(&CritSect);   
   
   /*if( m_hEmptyEvent )
      CloseHandle( m_hEmptyEvent );
   if( m_hFillEvent )
      CloseHandle( m_hFillEvent );
*/
   if (m_pocBufArray != 0) 
   {
      delete []m_pocBufArray;
      m_pocBufArray = 0;
   }
   
   if (m_pchBufferMemory != 0) 
   {
      delete [] m_pchBufferMemory;
      m_pchBufferMemory = 0;
   }
   
   m_nNumberOfBuffers = 0;
   
   LeaveCriticalSection(&CritSect);
}

CIoBuf* CIoBufPool::GetEmptyBuf()
{    
   EnterCriticalSection(&CritSect);
   
   CIoBuf *pocIoBuf = NULL;
   
   if( m_pocEmptyBuff ->bEmpty )
      pocIoBuf = m_pocEmptyBuff;

   LeaveCriticalSection(&CritSect);

   return pocIoBuf;
}

void CIoBufPool::ReleaseEmptyBuf()
{    
   EnterCriticalSection(&CritSect);
   
   CIoBuf *pocIoBuf = NULL;
   
   m_pocEmptyBuff ->bEmpty = FALSE; // Deliver
   if ( !m_pocFillBuff )
      m_pocFillBuff = m_pocEmptyBuff;
   m_pocEmptyBuff = m_pocEmptyBuff -> Next;   

   LeaveCriticalSection(&CritSect);
}

CIoBuf* CIoBufPool::GetFilledBuf()
{
   EnterCriticalSection(&CritSect);
   
   CIoBuf *pocIoBuf = NULL;
   
   if( m_pocFillBuff )
   {
      if( !m_pocFillBuff ->bEmpty )
      	 pocIoBuf = m_pocFillBuff;	 
   }
   LeaveCriticalSection(&CritSect);

   return pocIoBuf;
}

void CIoBufPool::ReleaseFilledBuf()
{
   EnterCriticalSection(&CritSect);
   
   if ( m_pocFillBuff )
   {
      m_pocFillBuff ->bEmpty = TRUE;
      m_pocFillBuff = m_pocFillBuff -> Next;
   }
   LeaveCriticalSection(&CritSect);
}
