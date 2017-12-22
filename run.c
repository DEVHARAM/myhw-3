#include <sys/types.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "run.h"
#include "util.h"

void *base = 0;

p_meta find_meta(p_meta *end, size_t size) {
	p_meta index = base;
	p_meta result = NULL;
	size += META_SIZE;
	int fin = -1;

	switch (fit_flag) {
	case FIRST_FIT:
	{
		while (index)
		{
			if (index->free && index->size >= size)
			{
				result = index;
				break;
			}
			index = index->next;
		}
	}
	break;

	case BEST_FIT:
	{
		while (index)
		{
			if (index->free && index->size >= size)
			{
				if ((fin == -1) || (fin > index->size)) {
					fin = index->size;
					result = index;
				}
			}
			index = index->next;
		}
	}
	break;

	case WORST_FIT:
	{

		size_t max = 0;
		while (index) {

			if (index->free == 1 && index->size >= size && index->size > max) {
				result = index;
				max = index->size;

			}
			index = index->next;

		}
	}
	break;
	}
	return result;
}

void *m_malloc(size_t size) {
	p_meta tmp;
	p_meta end=base;
	p_meta formul;
	p_meta result;
	if (size % 4) size += 4 - size % 4;
	formul = sbrk(META_SIZE + size);
	formul->size = size;
	formul->free = 0;
	tmp = find_meta(base, size);

	if(base)
	{
		while (end->next != 0&&end->next)end = end->next;
		if (!tmp)
		{
			if (end->next)
			{
				end->next = formul;
				formul->prev = end;
			}
			else
			{				
				formul->prev = end;
				formul->next = end->next;

				end->next = formul;
			}
		}
		else
		{
			int tmpsize = tmp->size - size - META_SIZE;
			tmp->free = 0;
			while (tmp->size <= size + META_SIZE);

			result = tmp + size + META_SIZE;
			result->prev = tmp;
			result->next = tmp->next;
			result->size = tmpsize;
			result->free = 1;
			
			tmp->next = result;
			tmp->size = size;
			
			return tmp->data;
		}
	}
	else
		base = formul;
	return formul->data;
}



void m_free(void *ptr) {
	p_meta tmp ;
	tmp = ptr - META_SIZE;
	tmp->free = 1;

	
	if (tmp->next) {
		if ((tmp->next)->free) {
			tmp->size += (tmp->next)->size + META_SIZE;
			tmp->next = (tmp->next)->next;
			(tmp->next)->prev = tmp;
		}
	}
	else if (tmp->prev) {
		(tmp->prev)->next = NULL;
	}
	if (tmp->prev && (tmp->prev)->free) {
		(tmp->prev)->next = tmp->next;
		(tmp->prev)->size += tmp->size + META_SIZE;
		tmp = tmp->prev;
	}
}


void* m_realloc(void* ptr, size_t size)
{
	int tmpsize = size;
	p_meta tmp = base,formul;

	if (tmp&&tmp->data != ptr) 
		tmp = tmp->next;
	if (size % 4) size = size + (4 - size % 4);

	if (tmp->data == ptr)
	{
		if (tmp->size >= size)
		{
			if (tmp->next && (tmp->size - size) >= META_SIZE) {
				formul = tmp->data + size;
				formul->next = tmp->next;
				formul->prev = tmp;
				formul->size = tmp->size - size - META_SIZE;
				formul->free = 1;
				
				tmp->next = formul;
				tmp->size = size;
				tmp->data[tmpsize] = '\0';
				return formul->data;
			}
			else {
				for (int i = tmpsize; i<tmp->size; i++)
					tmp->data[i] = '\0';
				return tmp->data;
			}
		}
		else
		{
			if (!tmp->next || tmp->next->size < size - tmp->size)
			{
				formul = m_malloc(size);
				tmp->next = formul;
				formul->prev = tmp;
				formul->size = size;
				formul->free = 0;
				for (int i = 0; i<tmp->size; i++)
					formul->data[i] = tmp->data[i];
				m_free(ptr);
				return formul->data;
			}
			else
			{
				tmp->next = tmp->next + (size - tmp->size);
				tmp->next->next = tmp->next->next;
				tmp->next->prev = tmp->next->prev;
				tmp->next->size = tmp->next->size;
				tmp->next->free = tmp->next->free;
				tmp->size = size;
				return tmp->next->data;
			}
		}
	}
	
}

