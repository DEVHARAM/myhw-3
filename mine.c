#include <sys/types.h>
#include <limits.h>

#include "run.h"
#include "util.h"

void *base = 0;

p_meta find_meta(p_meta *last, size_t size) {
	p_meta index = base;
	p_meta result = base;

	switch (fit_flag) {
	case FIRST_FIT:
	{
		//FIRST FIT CODE
		while(index){
			if(index->free==1){
				if(index->size>=size){
					result=index;
					index->free=0;
					break;
					
				}
			}
		}
	}
	break;

	case BEST_FIT:
	{
		//BEST_FIT CODE
	}
	break;

	case WORST_FIT:
	{
		//WORST_FIT CODE
	}
	break;

	}
	return result;
}

void *m_malloc(size_t size) {

}

void m_free(void *ptr) {

}

void*
m_realloc(void* ptr, size_t size)
{

}
