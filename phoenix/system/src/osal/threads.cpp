#include <pthread.h>
#include <unistd.h>
//#include <hresult.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/resource.h>

/* mutex */
typedef struct tag_posix_mutex_t {
	int				type;
	pthread_mutex_t	handle;
	int				lockCount;
} posix_mutex_t;

//HRESULT osal_MutexCreate(osal_mutex_t* pMutexID)
//{
//}

//HRESULT osal_MutexLock(osal_mutex_t* pMutexID)
