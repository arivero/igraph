/* -*- mode: C -*-  */
/* 
   IGraph library.
   Copyright (C) 2003, 2004, 2005  Gabor Csardi <csardi@rmki.kfki.hu>
   MTA RMKI, Konkoly-Thege Miklos st. 29-33, Budapest 1121, Hungary
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include "types.h"
#include "memory.h"
#include "random.h"

#include <assert.h>
#include <string.h> 		/* memcpy & co. */
#include <stdlib.h>

/**
 * \ingroup dqueue
 * \brief Initialized a double ended queue.
 */

int dqueue_init (dqueue_t* q, long int size) {
	if (size <= 0 ) { size=1; }
	q->stor_begin=Calloc(size, real_t);
	q->stor_end=q->stor_begin + size;
	q->begin=q->stor_begin;
	q->end=NULL;
	
	return 0;
}

/**
 * \ingroup dqueue
 * \brief Destroys a double ended queue.
 */

int dqueue_destroy (dqueue_t* q) {
	assert( q->stor_begin != NULL);
	Free(q->stor_begin);
	return 0;
}

/**
 * \ingroup dqueue
 * \brief Decides whether the queue is empty.
 */

int dqueue_empty (dqueue_t* q) {
	return q->end == NULL;
}

/**
 * \ingroup dqueue
 * \brief Removes all elements from the queue.
 */

int dqueue_clear   (dqueue_t* q) {

	q->begin=q->stor_begin;
	q->end=NULL;
	return 0;
}

/**
 * \ingroup dqueue
 * \brief Checks whether the queue is full.
 *
 * If a queue is full the next dqueue_push() operation will allocate
 * more memory.
 */

int dqueue_full (dqueue_t* q) {
	return q->begin == q->end;
}

/**
 * \ingroup dqueue
 * \brief Returns the number of elements in the queue.
 */

long int dqueue_size (dqueue_t* q) {
	if (q->end==NULL) {
		return 0;
	} else if (q->begin < q->end) {
		return q->end - q->begin;
	} else {
		return q->stor_end - q->begin + q->end - q->stor_begin;
	}
}

/**
 * \ingroup dqueue
 * \brief Returns the element at the head of the queue.
 */

real_t dqueue_head (dqueue_t* q) {
	assert( q->end != NULL );
	return *(q->begin);
}

/**
 * \ingroup dqueue
 * \brief Returns the element at the tail of the queue.
 */

real_t dqueue_back (dqueue_t* q) {
	assert( q->end != NULL );
	return *(q->end-1);
}

/**
 * \ingroup dqueue
 * \brief Returns and removes the element at the head of the queue.
 */

real_t dqueue_pop (dqueue_t* q) {
	real_t tmp=*(q->begin);
	assert( q->end != NULL );
	(q->begin)++;
	if (q->begin==q->stor_end) {
		q->begin=q->stor_begin;
	}
	if (q->begin==q->end) {
		q->end=NULL;
	}

	return tmp;
}

/**
 * \ingroup dqueue
 * \brief Returns and removes the element at the tail of the queue.
 */

real_t dqueue_pop_back (dqueue_t* q) {
	real_t tmp;
	assert( q->end != NULL );
	if (q->end != q->stor_begin) {
		tmp=*((q->end)-1);
		q->end = (q->end)-1;
	} else {
		tmp=*((q->stor_end)-1);
		q->end = (q->stor_end)-1;
	}
	if (q->begin==q->end) {
		q->end=NULL;
	}
	
	return tmp;
}

/**
 * \ingroup dqueue
 * \brief Appends an element to the back of the queue.
 */

int dqueue_push (dqueue_t* q, real_t elem) {
	if (q->begin != q->end) {
		/* not full */
		if (q->end==NULL) {
			q->end=q->begin;
		}			
		*(q->end) = elem;
		(q->end)++;
		if (q->end==q->stor_end) {
			q->end=q->stor_begin;
		}
	} else {
		/* full, allocate more storage */
		
		real_t *bigger=NULL, *old=q->stor_begin;

		bigger=Calloc( 2*(q->stor_end - q->stor_begin)+1, real_t );

		if (q->stor_end - q->begin) {
			memcpy(bigger, q->begin, 
			       (q->stor_end - q->begin) * sizeof(real_t));
		}
		if (q->end - q->stor_begin > 0) {
			memcpy(bigger + (q->stor_end - q->begin),
			       q->stor_begin, (q->end - q->stor_begin) * sizeof(real_t));
		}
		
		q->end       =bigger + (q->stor_end - q->stor_begin);
		q->stor_end  =bigger + 2*(q->stor_end - q->stor_begin) + 1;
		q->stor_begin=bigger;
		q->begin     =bigger;
				
		*(q->end) = elem;
		(q->end)++;
		if (q->end==q->stor_end) {
			q->end=q->stor_begin;
		}

		Free(old);
	}
	
	return 0;
}

/**
 * \ingroup vector
 * \brief Initializes a vector object.
 * 
 * Every vector needs to be initialized before it can be used, and
 * there are a number initialization functions, the others are
 * vector_init_copy(), vector_init_seq() and vector_copy().
 * 
 * Every vector object initialized by this function should be
 * destroyed (ie. the memory allocated for it should be freed) when it
 * is not needed any more, the vector_destroy() function is
 * responsible for this.
 * @param v Pointer to a not yet initialized vector object.
 * @param size The size of the vector.
 * @return error code.
 * 
 * Time complexity: operating system dependent.
 */

int vector_init      (vector_t* v, int long size) {	
        long int alloc_size= size > 0 ? size : 1;
	assert(v != NULL);
	if (size < 0) { size=0; }
	v->stor_begin=Calloc(alloc_size, real_t);
	v->stor_end=v->stor_begin + alloc_size;
	v->end=v->stor_begin+size;

	return 0;
}

/**
 * \ingroup vector
 * \brief Destroys a vector object
 *
 * All vectors initialized by vector_init() should be properly
 * destroyed by this function. A destroyed vector needs to be
 * reinitialized by vector_init(), vector_init_copy() or
 * vector_as_vector() before using it again.
 * @param v Pointer to the (previously initialized) vector object to
 *        destroy. 
 * @return Error code.
 * 
 * Time complexity: operating system dependent.
 */

int vector_destroy   (vector_t* v) {
	assert(v != NULL);
	assert(v->stor_begin != NULL);

	Free(v->stor_begin);
	v->stor_begin = NULL;
	
	return 0;
}

/**
 * \ingroup vector
 * \brief Reserves memory for a vector
 * 
 * \a igraph vectors are flexible, they can grow and shrink. Growing
 * however occasionally needs the data in the vector to be copyed.
 * In order to avoid you can call this function to reserve space for
 * future growth of the vector. 
 * 
 * Note that this function does <em>not</em> change the size of the
 * vector, let us see a small example to clarify things: if you
 * reserve space for 100 elements and the size of your
 * vector was (and still is) 60, then you can surely add additional 40
 * elements to your vector before it will be copied.
 * @param v The vector object.
 * @param size The new <em>allocated</em> size of the vector.
 * @return Error code.
 *
 * Time complexity: operating system dependent, should be around
 * <code>O(n)</code>, <code>n</code> is the new allocated size of the
 * vector.
 */

int vector_reserve   (vector_t* v, long int size) {
	long int actual_size=vector_size(v);
	assert(v != NULL);
	
	if (size <= vector_size(v)) { return 0; }

	v->stor_begin=Realloc(v->stor_begin, size, real_t);
	v->stor_end=v->stor_begin + size;
	v->end=v->stor_begin+actual_size;
	
	return 0;
}

/**
 * \ingroup vector
 * \brief Decides whether the size of the vector is zero.
 *
 * @param v The vector object.
 * @return Non-zero number if the size of the vector is not zero and
 *         zero otherwise.
 * 
 * Time complexity: <code>O(1)</code>.
 */

int vector_empty     (vector_t* v) {
	assert(v != NULL);
	assert(v->stor_begin != NULL);
	
	return v->stor_begin == v->end;
}

/**
 * \ingroup vector
 * \brief Gives the size (=length) of the vector.
 * 
 * @param v The vector object
 * @return The size of the vector.
 *
 * Time complexity: <code>O(1)</code>. 
 */

long int vector_size      (vector_t* v) {
	assert(v != NULL);
	
	return v->end - v->stor_begin;
}

/**
 * \ingroup vector
 * \brief Removes all elements from a vector.
 * 
 * This function simply sets the size of the vector to zero, it does
 * not free any allocated memory. For that you have to call
 * vector_destroy().
 * @param v The vector object.
 * @return Error code.
 * 
 * Time complexity: <code>O(1)</code>.
 */

int vector_clear     (vector_t* v) {
	assert(v != NULL);
	
	v->end = v->stor_begin;
	
	return 0;
}

/**
 * \ingroup vector
 * \brief Appends one elements to a vector.
 * 
 * This function resizes the vector to be one element longer and
 * sets the very last element in the vector to <code>e</code>.
 * @param v The vector object.
 * @param e The element to append to the vector.
 * @return Error code.
 * 
 * Time complexity: operating system dependent. What is important that
 * it can be assumed that <code>n</code> subsequent calls to this
 * function has time complexity <code>O(n)</code>, even if there
 * hadn't been any space reserved for the new elements by
 * vector_reserve(). This is 
 * implemented by a trick similar to the C++ <code>vector</code>
 * class: each time more memory is allocated for a vector, the size of
 * the additionally allocated memory is the same as the vector's
 * current length.
 */

int vector_push_back (vector_t* v, real_t e) {
	assert(v != NULL);
	
	/* full, allocate more storage */
	if (v->stor_end == v->end) {
		long int new_size = vector_size(v) * 2;
		if (new_size == 0) { new_size = 1; }
		vector_reserve(v, new_size);
	}
	
	*(v->end) = e;
	v->end += 1;
	
	return 0;
}

/**
 * \ingroup internal
 */

real_t vector_e         (vector_t* v, long int pos) {
	assert(v != NULL);
	
	return * (v->stor_begin + pos);
}

/**
 * \ingroup internal
 */

real_t*vector_e_ptr  (vector_t* v, long int pos) {
  return v->stor_begin+pos;
}

/**
 * \ingroup internal
 */

int vector_set       (vector_t* v, long int pos, real_t value) {
	assert(v != NULL);
	
	*(v->stor_begin + pos) = value;
	return 0;
}

/**
 * \ingroup internal
 */

int vector_add       (vector_t* v, long int pos, real_t value) {
	assert(v != NULL);
	
	*(v->stor_begin + pos) += value;
	return 0;
}

/**
 * \ingroup vector
 * \brief Sets each element in the vector to zero.
 * 
 * @param v The vector object.
 * @return Error code.
 *
 * Time complexity: <code>O(n)</code>, the size of the vector.
 */

int vector_null      (vector_t* v) {
	if (vector_size(v)>0) {
		memset(v->stor_begin, 0, sizeof(real_t)*vector_size(v));
	}
	return 0;
}

/**
 * \ingroup internal
 */

int vector_replace_first(vector_t* v, real_t old, real_t newe) {
  long int i;
  assert( v != NULL);
  for (i=0; i<vector_size(v); i++) {
    if (vector_e(v, i)==old) {
      vector_set(v, i, newe);
      return 0;
    }
  }
  return 1;
}

/**
 * \ingroup internal
 */

long int vector_find(vector_t* v, real_t elem) {
  long int i, s;
  assert(v!=NULL);
  s=vector_size(v);
  for (i=0; i<s; i++) {
    if (vector_e(v, i)==elem) {
      return i;
    }
  }
  return -1;
}

/**
 * \ingroup vector
 * \brief Returns the last element in a vector.
 *
 * It is an error to call this function on an empty vector.
 * @param v The vector object.
 * @return The last element.
 * 
 * Time complexity: <code>O(1)</code>.
 */

real_t vector_tail(vector_t *v) {
  return *((v->end)-1);
}

/**
 * \ingroup internal
 */

real_t vector_pop_back(vector_t* v) {
  real_t tmp;
  assert(v!=NULL);
  assert(v->end != v->stor_begin);
  tmp=vector_e(v, vector_size(v)-1);
  v->end -= 1;
  return tmp;
}

/**
 * \ingroup internal
 */

int vector_change(vector_t* v, long int pos1, long int pos2) {
  real_t tmp;
  assert(v!=NULL);
  tmp=vector_e(v, pos1);
  vector_set(v, pos1, vector_e(v, pos2));
  vector_set(v, pos2, tmp);
  return 0;
}

/**
 * \ingroup internal
 */

int vector_order(vector_t* v, vector_t* res, integer_t nodes) {
  long int edges=vector_size(v);
  long int *ptr=Calloc(nodes+1, long int);
  long int *rad=Calloc(edges, long int);
  long int i, j;
  memset(ptr, 0, sizeof(long int)*nodes+1);
  vector_resize(res, edges);
  
  for (i=0; i<edges; i++) {
    long int radix=v->stor_begin[i];
    if (ptr[radix]!=0) {
      rad[i]=ptr[radix];
    }
    ptr[radix]=i+1;
  }
  
  j=0;
  for (i=0; i<nodes+1; i++) {
    if (ptr[i] != 0) {
      long int next=ptr[i]-1;
      res->stor_begin[j++]=next;
      while (rad[next] != 0) {
	next=rad[next]-1;
	res->stor_begin[j++]=next;
      }
    }
  }
  
  Free(ptr);
  Free(rad);
  return 0;
}

/**
 * \ingroup internal
 */

int vector_sort_cmp(const void *a, const void *b) {
  const real_t *da = (const real_t *) a;
  const real_t *db = (const real_t *) b;

  return (*da > *db) - (*da < *db);
}

/**
 * \ingroup vector
 * \brief Sorts the elements of the vector into ascending order.
 * 
 * This function uses the built-in sort function of the C library.
 * @param v Pointer to an initialized vector object.
 * @return Error code.
 *
 * Time complexity: should be <code>O(nlogn)</code> for <code>n</code>
 * elements.
 */

int vector_sort(vector_t *v) {
  qsort(v->stor_begin, vector_size(v), sizeof(real_t), vector_sort_cmp);
  return  0;
}

/**
 * \ingroup vector
 * \brief Resize the vector.
 *
 * Note that this function does not free any memory, just sets the
 * size of the vector to the given one. It can on the other hand 
 * allocate more memory if the new size is larger than the previous
 * one. In this case the newly appeared elements in the vectors are
 * <em>not</em> set to zero, they are uninitialized.
 * @param v The vector object
 * @param newsize The new size of the vector.
 * @return Error code.
 * 
 * Time complexity: <code>O(1)</code> if the new size is smaller,
 * operating system dependent if it is larger. In the latter case it
 * is usually around <code>O(n)</code>, <code>n</code> is the new size
 * of the vector.
 */

int vector_resize(vector_t* v, long int newsize) {
  vector_reserve(v, newsize);
  v->end = v->stor_begin+newsize;
  return 0;
}

/**
 * \ingroup vector
 * \brief Gives the maximum element of the vector.
 *
 * If the size of the vector is zero, an arbitrary number is
 * returned.
 * @param v The vector object.
 * @return The maximum element.
 *
 * Time complexity: <code>O(n)</code>, <code>n</code> is the size of
 * the vector. 
 */

real_t vector_max(vector_t* v) {
  real_t max=*(v->stor_begin);
  real_t *ptr=v->stor_begin+1;
  while (ptr < v->end) {
    if ((*ptr) > max) {
      max=*ptr;
    }
    ptr++;
  }
  return max;
}

/**
 * \ingroup internal
 */

vector_t vector_as_vector(real_t* data, long int length) {
  vector_t v;
  v.stor_begin=data;
  v.stor_end=v.end=data+length;
  
  return v;
}

/**
 * \ingroup vector
 * \brief Initializes a vector from an ordinary C array.
 * 
 * @param v Pointer to an uninitialized vector object.
 * @param data A regular C array.
 * @param length The length of the C array.
 * @return Error code.
 * 
 * Time complexity: operating system specific, usually
 * <code>O(length)</code>.
 */

int vector_init_copy(vector_t *v, real_t *data, long int length) {
  v->stor_begin=Calloc(length, real_t);
  v->stor_end=v->stor_begin+length;
  v->end=v->stor_end;
  memcpy(v->stor_begin, data, length*sizeof(real_t));
  
  return 0;
}

/**
 * \ingroup vector
 * \brief Copies the contents of a vector to a C array.
 * 
 * The C array should have sufficient length.
 * @param v The vector object.
 * @param to The C array.
 * @return Error code.
 * 
 * Time complexity: <code>O(n)</code>, <code>n</code> is the size of
 * the vector.
 */

int vector_copy_to(vector_t *v, real_t* to) {
  if (v->end != v->stor_begin) {
    memcpy(to, v->stor_begin, sizeof(real_t) * (v->end - v->stor_begin));
  }
  return 0;
}

/**
 * \ingroup vector
 * \brief Initializes a vector from another vector object.
 * 
 * The contents of the existing vector object will be copied to
 * the new one.
 * @param to Pointer to a not yet initialized vector object.
 * @param from The original vector object to copy.
 * @return Error code.
 * 
 * Time complexity: operating system dependent, usually
 * <code>O(n)</code>, <code>n</code> is the size of the vector.
 */

int vector_copy(vector_t *to, vector_t *from) {
  to->stor_begin=Calloc(vector_size(from), real_t);
  to->stor_end=to->stor_begin+vector_size(from);
  to->end=to->stor_end;
  memcpy(to->stor_begin, from->stor_begin, vector_size(from)*sizeof(real_t));
  
  return 0;
}

/**
 * \ingroup vector
 * \brief Calculates the sum of the elements in the vector.
 *
 * For the empty vector 0.0 is returned.
 * @param v The vector object.
 * @return The sum of the elements.
 * 
 * 
 * Time complexity: <code>O(n)</code>, the size of the vector.
 */

real_t vector_sum(vector_t *v) {
  real_t res=0;
  real_t *p;
  for (p=v->stor_begin; p<v->end; p++) {
    res += *p;
  }
  return res;
}

/**
 * \ingroup vector
 * \brief Calculates the product of the elements in the vector.
 * 
 * For the empty vector one (1) is returned.
 * @param v The vector object.
 * @return The product of the elements.
 * 
 * Time complexity: <code>O(n)</code>, the size of the vector.
 */

real_t vector_prod(vector_t *v) {
  real_t res=1;
  real_t *p;
  for (p=v->stor_begin; p<v->end; p++) {
    res *= *p;
  }
  return res;
}

/**
 * \ingroup vector
 * \brief Initializes a vector with a sequence.
 * 
 * The vector will contain the numbers <code>from</code>,
 * <code>from+1</code>, ..., <code>to</code>.
 * @param v Pointer to an uninitialized vector object.
 * @param from The lower limit in the sequence (inclusive).
 * @param to The upper limit in the sequence (inclusive).
 * @return Error code.
 *
 * Time complexity: <code>O(n)</code>, the number of elements in the
 * vector. 
 */

int vector_init_seq(vector_t *v, real_t from, real_t to) {
  real_t *p;
  vector_init(v, to-from+1);
  for (p=v->stor_begin; p<v->end; p++) {
    *p = from++;
  }
  
  return 0;
}

/**
 * \ingroup internal
 */

int vector_remove_section(vector_t *v, long int from, long int to) {
  memmove(v->stor_begin+from, v->stor_begin+to,
	  sizeof(real_t)*(v->end-v->stor_begin-to));
  v->end -= (to-from);
  return 0;
}

/**
 * \ingroup internal
 */

int vector_remove(vector_t *v, long int elem) {
  return vector_remove_section(v, elem, elem+1);
}

/**
 * \ingroup internal
 */

int vector_move_interval(vector_t *v, long int begin, long int end, 
			 long int to) {
  memcpy(v->stor_begin+to, v->stor_begin+begin, 
	 sizeof(real_t)*(end-begin));

  return 0;
}

/**
 * \ingroup internal
 */

int vector_permdelete(vector_t *v, long int *index, long int nremove) {
  long int i;
  for (i=0; i<vector_size(v); i++) {
    VECTOR(*v)[ index[i]-1 ] = VECTOR(*v)[i];
  }
  vector_resize(v, vector_size(v)-nremove);
  
  return 0;
}

/**
 * \ingroup internal
 */

int vector_remove_negidx(vector_t *v, vector_t *neg, long int nremove) {
  long int i, idx=0;
  for (i=0; i<vector_size(v); i++) {
    VECTOR(*v)[idx++] = VECTOR(*v)[i];
  }
  vector_resize(v, vector_size(v)-nremove);
  
  return 0;
}

/**
 * \ingroup matrix
 * \brief Initializes a matrix.
 * 
 * Every matrix needs to be initialized before using it, this is done
 * by calling this function. A matrix has to be destroyed if it is not
 * needed any more, see matrix_destroy().
 * @param m Pointer to a not yet initialized matrix object to be
 *        initialized. 
 * @param nrow The number of rows in the matrix.
 * @param ncol The number of columns in the matrix.
 * @return Error code.
 *
 * Time complexity: ususally <code>O(n)</code>, <code>n</code> is the
 * number of elements in the matrix.
 */
int matrix_init(matrix_t *m, long int nrow, long int ncol) {
  vector_init(&m->data, nrow*ncol);
  m->nrow=nrow;
  m->ncol=ncol;
  return 0;
}

/** 
 * \ingroup matrix
 * \brief Destroys a matrix object.
 * 
 * This function frees all the memory allocated for a matrix
 * object. The destroyed object needs to be reinitialized before using
 * it again.
 * @param m The matrix to destroy.
 * @return Error code.
 *
 * Time complexity: operating system dependent.
 */ 

int matrix_destroy(matrix_t *m) {
  vector_destroy(&m->data);
  return 0;
}

/**
 * \ingroup matrix
 * \brief Resizes a matrix.
 *
 * This function resizes a matrix by adding more elements to it.
 * The matrix contains arbitrary data after resizing it.
 * Ie. after calling this function you cannot expect that element
 * <code>(i,j)</code> in the matrix remains the same as before. 
 * @param m Pointer to an already initialized matrix object.
 * @param nrow The number of rows in the resized matrix.
 * @param ncol The number of columns in the resized matrix.
 * @return Error code.
 * 
 * Time complexity: <code>O(1)</code> if the matrix gets smaller,
 * usually <code>O(n)</code> if it gets larger, <code>n</code> is the
 * number of elements in the resized matrix.
 */

int matrix_resize(matrix_t *m, long int nrow, long int ncol) {
  vector_resize(&m->data, nrow*ncol);
  m->nrow=nrow;
  m->ncol=ncol;
  return 0;
}

/**
 * \ingroup matrix
 * \brief The number of elements in a matrix.
 * 
 * @param m Pointer to an initialized matrix object.
 * @return The size of the matrix.
 *
 * Time complexity: <code>O(1)</code>.
 */

long int matrix_size(matrix_t *m) {
  return (m->nrow) * (m->ncol);
}

/**
 * \ingroup matrix
 * \brief The number of rows in a matrix.
 * 
 * @param m Pointer to an initialized matrix object.
 * @return The number of rows in the matrix.
 * 
 * Time complexity: <code>O(1)</code>.
 */

long int matrix_nrow(matrix_t *m) {
  return m->nrow;
}

/**
 * \ingroup matrix
 * \brief The number of columns in a matrix.
 * 
 * @param m Pointer to an initialized matrix object.
 * @return The number of columns in the matrix.
 * 
 * Time complexity: <code>O(1)</code>.
 */

long int matrix_ncol(matrix_t *m) {
  return m->ncol;
}

/** 
 * \ingroup matrix
 * \brief Copies a matrix to a regular C array.
 *
 * The matrix is copied columnwise, as this is the format most
 * programs and languages use.
 * The C array should be of sufficient size, there are (of course) not
 * range checks done.
 * @param m Pointer to an initialized matrix object.
 * @param to Pointer to a C array, the place to copy the data to.
 * @return Error code.
 *
 * Time complexity: <code>O(n)</code>, <code>n</code> is the number of 
 * elements in the matrix.
 */

int matrix_copy_to(matrix_t *m, real_t *to) {
  vector_copy_to(&m->data, to);
  return 0;
}

/** 
 * \ingroup matrix
 * \brief Sets all element in a matrix to zero.
 * 
 * @param m Pointer to an initialized matrix object.
 * @return Error code.
 * 
 * Time complexity: <code>O(n)</code>, <code>n</code> is the number of 
 * elements in the matrix.
 */

int matrix_null(matrix_t *m) {
  vector_null(&m->data);
  return 0;
}

/**
 * \ingroup internal
 */

int matrix_add_cols(matrix_t *m, long int n) {
  matrix_resize(m, m->nrow, m->ncol+n);
  return 0;
}

/**
 * \ingroup internal
 */

int matrix_add_rows(matrix_t *m, long int n) {
  long int i;
  vector_resize(&m->data, (m->ncol)*(m->nrow+n));
  for (i=m->ncol-1; i>=0; i--) {
    vector_move_interval(&m->data, (m->nrow)*i, (m->nrow)*(i+1),
			 (m->nrow+n)*i);
  }
  m->nrow += n;
  return 0;
}

/**
 * \ingroup internal
 */

int matrix_remove_col(matrix_t *m, long int col) {
  vector_remove_section(&m->data, (m->nrow)*col, (m->nrow)*(col+1));
  m->ncol--;
  return 0;
}

/**
 * \ingroup internal
 */

int matrix_permdelete_rows(matrix_t *m, long int *index, long int nremove) {
  long int i, j;
  for (i=0; i<m->ncol; i++) {
    for (j=0; j<m->nrow; j++) {
      if (index[j] != 0) {
	MATRIX(*m, index[j]-1, i) = MATRIX(*m, j, i);
      }
    }
  }
  matrix_resize(m, m->nrow-nremove, m->ncol);

  return 0;
}

/**
 * \ingroup internal
 */

int matrix_delete_rows_neg(matrix_t *m, vector_t *neg, long int nremove) {
  long int i, j, idx=0;
  for (i=0; i<m->ncol; i++) {
    for (j=0; j<m->nrow; j++) {
      if (VECTOR(*neg)[j] >= 0) {
	MATRIX(*m, idx++, i) = MATRIX(*m, j, i);
      } 
    }
    idx=0;
  }
  matrix_resize(m, m->nrow-nremove, m->ncol);

  return 0;
}

/**
 * \ingroup internal
 */

int matrix_copy(matrix_t *to, matrix_t *from) {
  vector_copy(&to->data, &from->data);
  to->nrow = from->nrow;
  to->ncol = from->ncol;
  
  return 0;
}


/**
 * \ingroup stack
 * \brief Initializes a stack.
 */

int igraph_stack_init       (igraph_stack_t* s, long int size) {
        long int alloc_size= size > 0 ? size : 1;
	assert (s != NULL);
	if (size < 0) { size=0; }
	s->stor_begin=Calloc(alloc_size, real_t);
	s->stor_end=s->stor_begin + alloc_size;
	s->end=s->stor_begin;
	
	return 0;
}

/**
 * \ingroup stack
 * \brief Destroys a stack object.
 */

int igraph_stack_destroy    (igraph_stack_t* s) {
	assert (s != NULL);
	assert (s->stor_begin != NULL);
	Free(s->stor_begin);
	s->stor_begin=NULL;
	
	return 0;
}

/**
 * \ingroup stack
 * \brief Reserves memory for a stack object.
 */

int igraph_stack_reserve    (igraph_stack_t* s, long int size) {
  long int actual_size=igraph_stack_size(s);
  assert(s != NULL);
  
  if (size <= igraph_stack_size(s)) { return 0; }
  
  s->stor_begin=Realloc(s->stor_begin, size, real_t);
  s->stor_end=s->stor_begin + size;
  s->end=s->stor_begin+actual_size;
  
  return 0;
}

/**
 * \ingroup stack
 * \brief Decides whether a stack object is empty.
 */

int igraph_stack_empty      (igraph_stack_t* s) {
	assert (s != NULL);
	assert (s->stor_begin != NULL);
	assert (s->end != NULL);
	
	return s->stor_begin == s->end;
}

/**
 * \ingroup stack
 * \brief Returns the number of elements in a stack.
 */

long int igraph_stack_size       (igraph_stack_t* s) {
	assert (s != NULL);
	assert (s->stor_begin != NULL);
	
	return s->end - s->stor_begin;
}

/**
 * \ingroup stack
 * \brief Removes all elements from a stack.
 */

int igraph_stack_clear      (igraph_stack_t* s) {
	assert (s != NULL);
	assert (s->stor_begin != NULL);
	s->end = s->stor_begin;
	
	return 0;
}

/**
 * \ingroup stack
 * \brief Places an element on the top of a stack.
 */

int igraph_stack_push       (igraph_stack_t* s, real_t elem) {
	assert (s != NULL);
	assert (s->stor_begin != NULL);
	if (s->end == s->stor_end) {
		/* full, allocate more storage */
		
	        real_t *bigger=NULL, *old=s->stor_begin;
		
		bigger = Calloc(2*igraph_stack_size(s)+1, real_t);
		
		memcpy(bigger, s->stor_begin, 
		       igraph_stack_size(s)*sizeof(real_t));

		s->end        = bigger + (s->stor_end - s->stor_begin);
		s->stor_end   = bigger + 2*(s->stor_end - s->stor_begin)+1;
		s->stor_begin = bigger;
		
		*(s->end) = elem;
		(s->end) += 1;

		Free(old);
	} else {
		*(s->end) = elem;
		(s->end) += 1;
	}
	return 0;
}

/**
 * \ingroup stack
 * \brief Removes and returns an element from the top of a stack.
 */

real_t igraph_stack_pop        (igraph_stack_t* s) {

	assert (s != NULL);
	assert (s->stor_begin != NULL);
	assert (s->end != NULL);
	assert (s->end != s->stor_begin);
		
	(s->end)--;
	
	return *(s->end);
}

/**
 * \ingroup multiset
 * \brief Initializes a multiset object.
 */

int multiset_init    (multiset_t* m, long int size) {
  size= size > 0 ? size : 1;
  assert(m != NULL);
  m->stor_begin=Calloc(size, real_t);
  m->stor_end=m->stor_begin + size;
  m->end=m->stor_begin;
  
  return 0;
}

/**
 * \ingroup multiset
 * \brief Destroys a multiset object.
 */

int multiset_destroy (multiset_t* m) {
  assert(m != NULL);
  assert(m->stor_begin != NULL);
  
  Free(m->stor_begin);
  m->stor_begin = NULL;
  
  return 0;
}

/**
 * \ingroup multiset
 * \brief Reserves memory for a multiset object.
 */

int multiset_reserve (multiset_t* m, long int size) {
  long int actual_size;
  
  assert(m != NULL);
  assert(m->stor_begin != NULL);

  actual_size=multiset_size(m);
  if (size < actual_size) { return 0; }
  
  m->stor_begin=Realloc(m->stor_begin, size, real_t);
  m->stor_end=m->stor_begin + size;
  m->end=m->stor_begin+actual_size;
  
  return 0;
}

/**
 * \ingroup multiset
 * \brief Adds an elements to a multiset object.
 */

int multiset_add     (multiset_t* m, real_t elem) {
  assert(m != NULL);
  
  if (m->stor_end == m->end) {
    long int new_size=multiset_size(m) * 2;
    if (new_size==0) { new_size=1; }
    multiset_reserve(m, new_size);
  }

  *(m->end) = elem;
  m->end += 1;
  
  return 0;
}

/**
 * \ingroup multiset
 * \brief Selects an element from a multiset object.
 */

real_t multiset_choose  (multiset_t* m) {
  assert(m != NULL);
  assert(m->end != m->stor_begin);
  
  return *(m->stor_begin);
}

/**
 * \ingroup multiset
 * \brief Removes and returns an element from a multiset object.
 */

real_t multiset_choose_remove (multiset_t* m) {
  real_t tmp;
  assert(m != NULL);
  assert(m->end != m->stor_begin);
  
  tmp= *(m->stor_begin);
  *(m->stor_begin)=*((m->end)-1);
  m->end -= 1;

  return tmp;
}

/**
 * \ingroup multiset
 * \brief Gives the number of element in a multiset object.
 */

long int multiset_size(multiset_t* m) {
  assert(m != NULL);
  
  return m->end - m->stor_begin;
}

/**
 * \ingroup multiset
 * \brief Removes an instance of an element from a multiset object.
 */

int multiset_remove (multiset_t* m, real_t elem) {
  real_t *p;
  assert(m != NULL);

  for (p=m->stor_begin; p<m->end; p++) {
    if (*p == elem) {
      *p=*((m->end)-1);
      m->end -= 1;
      return 0;
    }
  }
  return 1;
}

/**
 * \ingroup multiset
 * \brief Removes all instances of an element from a multiset object.
 */

int multiset_remove_all (multiset_t* m, real_t elem) {
  real_t *p;
  assert(m != NULL);
  
  for (p=m->stor_begin; p<m->end; ) {
    if (*p==elem) {
      *p=*((m->end)-1);
      m->end -= 1;
    } else {
      p++;
    }
  }
    return 0;
}

/**
 * \ingroup multiset
 * \brief Returns the C array of all the elements in a multiset.
 *
 * You should consider the returnes array as read-only.
 */

real_t* multiset_get_vector(multiset_t* m) {
  assert (m != NULL);
  
  return m->stor_begin;  
}
     
/**
 * \ingroup multiset
 * \brief Selects a random element from a multiset object.
 */

real_t multiset_choose_random(multiset_t* m) {
  long int rnd;
  
  assert(m != NULL);
  
  rnd=RNG_INTEGER(0, multiset_size(m)-1);

  return *((m->stor_begin) + rnd);
}

/**
 * \ingroup multiset
 * \brief Selects and removes a random element from a multiset object.
 */

real_t multiset_choose_remove_random(multiset_t* m) {
  long int rnd;
  real_t tmp;
  
  assert(m != NULL);
  assert(m->stor_begin != m->end);
  
  rnd=RNG_INTEGER(0, multiset_size(m)-1);
  tmp=*((m->stor_begin) + rnd);

  *((m->stor_begin)+rnd) = *((m->end)-1);
  m->end -= 1;

  return tmp;
}

/**
 * \ingroup multiset
 * \brief Removes all elements in a multiset object.
 */

int multiset_clear   (multiset_t* m) {
  assert(m != NULL);
  
  m->end = m->stor_begin;

  return 0;
}
  

/**
 * \ingroup multiset
 * \brief Counts the number of instances of an element in a multiset
 * object. 
 */

long int multiset_count(multiset_t* m, real_t elem) {
  real_t *p;
  long int c;
  assert(m != NULL);
  
  c=0;
  for (p=m->stor_begin; p<m->end; p++) {
    if (*p == elem) {
      c++;
    }
  }
  
  return c;
}

/**
 * \ingroup multiset
 * \brief Counts the number of instances different from an element in
 * a multiset object.
 */

long int multiset_count_different(multiset_t* m, real_t elem) {
  real_t *p;
  long int c;
  assert(m != NULL);
  
  c=0;
  for (p=m->stor_begin; p<m->end; p++) {
    if (*p != elem) {
      c++;
    }
  }
  
  return c;
}

/**
 * \ingroup multiset
 * \brief Selects a random element which is different than a given one
 * in the multiset object.
 */

real_t multiset_choose_random_different(multiset_t* m, real_t elem) {
  long int all, s;
  real_t *p;
  
  assert(m != NULL);
  
  all=multiset_count_different(m, elem);
  if (all == 0) {
    return elem;
  }
  s=RNG_INTEGER(0, all-1);
  for (p=m->stor_begin; p<m->end; p++) {
    if (*p==elem) { 
      continue; 
    } else if (s==0) {
      return *p;
    } else {
      s--;
    }
  }
  return 0;
}

#define PARENT(x)     (((x)+1)/2-1)
#define LEFTCHILD(x)  (((x)+1)*2-1)
#define RIGHTCHILD(x) (((x)+1)*2)
  
/**
 * \ingroup heap
 * \brief Initializes an empty heap object.
 */

int heap_init           (heap_t* h, long int alloc_size) {
  if (alloc_size <= 0 ) { alloc_size=1; }
  h->stor_begin=Calloc(alloc_size, real_t);
  h->stor_end=h->stor_begin + alloc_size;
  h->end=h->stor_begin;
  h->destroy=1;
  
  return 0;  
}

/**
 * \ingroup heap
 * \brief Initializes a heap object from an array, the heap is also
 * built of course.
 */

int heap_init_array     (heap_t *h, real_t* data, long int len) {
  h->stor_begin=Calloc(len, real_t);
  h->stor_end=h->stor_begin+len;
  h->end=h->stor_end;
  h->destroy=1;

  memcpy(h->stor_begin, data, len*sizeof(real_t));

  heap_i_build (h->stor_begin, h->end-h->stor_begin, 0);
  
  return 0;
}

/**
 * \ingroup heap
 * \brief Destroys an initialized heap object.
 */

int heap_destroy        (heap_t* h) {  
  if (h->destroy) {
    assert( h->stor_begin != NULL);
    Free(h->stor_begin);
  }
  return 0;
}

/**
 * \ingroup heap
 * \brief Decides whether a heap object is empty.
 */

int heap_empty          (heap_t* h) {
  return h->stor_begin == h->end;
}

/**
 * \ingroup heap
 * \brief Adds an element to a heap object.
 */

int heap_push           (heap_t* h, real_t elem) {
  assert(h != NULL);
	
  /* full, allocate more storage */
  if (h->stor_end == h->end) {
    long int new_size = heap_size(h) * 2;
    if (new_size == 0) { new_size = 1; }
    heap_reserve(h, new_size);
  }
	
  *(h->end) = elem;
  h->end += 1;

  /* maintain heap */
  heap_i_shift_up(h->stor_begin, heap_size(h), heap_size(h)-1);
	
  return 0;
}

/**
 * \ingroup heap
 * \brief Returns the largest element in a heap.
 */

real_t heap_max       (heap_t* h) {
  assert(h != NULL);
  assert(h->stor_begin != NULL);
  assert(h->stor_begin != h->end);
  
  return h->stor_begin[0];
}

/**
 * \ingroup heap
 * \brief Returns and removes the largest element in a heap.
 */

real_t heap_delete_max(heap_t* h) {
  real_t tmp;

  assert(h != NULL);
  assert(h->stor_begin != NULL);

  tmp=h->stor_begin[0];
  heap_i_switch(h->stor_begin, 0, heap_size(h)-1);
  h->end -= 1;
  heap_i_sink(h->stor_begin, h->end-h->stor_begin, 0);
  
  return tmp;
}

/**
 * \ingroup heap
 * \brief Gives the number of elements in a heap.
 */

long int heap_size      (heap_t* h) {
  assert (h != NULL);
  return h->end - h->stor_begin;
}

/**
 * \ingroup heap
 * \brief Allocates more memory for a heap if needed.
 */

int heap_reserve        (heap_t* h, long int size) {
  long int actual_size=heap_size(h);
  assert(h != NULL);
  
  if (size <= actual_size) { return 0; }
  
  h->stor_begin=Realloc(h->stor_begin, size, real_t);
  h->stor_end=h->stor_begin + size;
  h->end=h->stor_begin+actual_size;
  
  return 0;
}

/**
 * \ingroup heap
 * \brief Build a heap, this should not be called directly.
 */

int heap_i_build(real_t* arr, long int size, long int head) {

  if (RIGHTCHILD(head) < size) { 
    /* both subtrees */
    heap_i_build(arr, size, LEFTCHILD(head) );
    heap_i_build(arr, size, RIGHTCHILD(head));
    heap_i_sink(arr, size, head);
  } else if (LEFTCHILD(head) < size) {
    /* only left */
    heap_i_build(arr, size, LEFTCHILD(head));
    heap_i_sink(arr, size, head);
  } else {
    /* none */
  }
  return 0;
}

/**
 * \ingroup heap
 * \brief Shift an element upwards in a heap, this should not be
 * called directly.
 */

int heap_i_shift_up(real_t* arr, long int size, long int elem) {
  
  if (elem==0 || arr[elem] < arr[PARENT(elem)]) { 
    /* at the top */
  } else {
    heap_i_switch(arr, elem, PARENT(elem));
    heap_i_shift_up(arr, size, PARENT(elem));
  }
  return 0;
}

/**
 * \ingroup heap
 * \brief Moves an element down in a heap, this function should not be
 * called directly.
 */

int heap_i_sink(real_t* arr, long int size, long int head) {

  if (LEFTCHILD(head) >= size) { 
    /* no subtrees */
  } else if (RIGHTCHILD(head) == size ||
	     arr[LEFTCHILD(head)] >= arr[RIGHTCHILD(head)]) {
    /* sink to the left if needed */
    if (arr[head] < arr[LEFTCHILD(head)]) {
      heap_i_switch(arr, head, LEFTCHILD(head));
      heap_i_sink(arr, size, LEFTCHILD(head));
    }
  } else {
    /* sink to the right */
    if (arr[head] < arr[RIGHTCHILD(head)]) {
      heap_i_switch(arr, head, RIGHTCHILD(head));
      heap_i_sink(arr, size, RIGHTCHILD(head));
    }
  }

  return 0;
}

/**
 * \ingroup heap
 * \brief Switches two elements in a heap, this function should not be
 * called directly.
 */

int heap_i_switch(real_t* arr, long int e1, long int e2) {
  if (e1!=e2) {
    real_t tmp=arr[e1];
    arr[e1]=arr[e2];
    arr[e2]=tmp;
  }

  return 0;
}

/**
 * \ingroup indheap
 * \brief Initializes an indexed heap.
 */

int indheap_init           (indheap_t* h, long int alloc_size) {
 if (alloc_size <= 0 ) { alloc_size=1; }
  h->stor_begin=Calloc(alloc_size, real_t);
  h->stor_end=h->stor_begin + alloc_size;
  h->end=h->stor_begin;
  h->destroy=1;
  h->index_begin=Calloc(alloc_size, long int);
  
  return 0;  
}

/**
 * \ingroup indheap
 * \brief Initializes and build an indexed heap from a C array.
 */

int indheap_init_array     (indheap_t *h, real_t* data, long int len) {
  long int i;

  h->stor_begin=Calloc(len, real_t);
  h->stor_end=h->stor_begin+len;
  h->end=h->stor_end;
  h->destroy=1;
  h->index_begin=Calloc(len, long int);

  memcpy(h->stor_begin, data, len*sizeof(real_t));
  for (i=0; i<len; i++) {
    h->index_begin[i]=i+1;
  }

  indheap_i_build (h, 0);
  
  return 0;
}

/**
 * \ingroup indheap
 * \brief Destroys an initialized indexed heap. 
 */

int indheap_destroy        (indheap_t* h) {  
  if (h->destroy) {
    assert( h->stor_begin != NULL);
    Free(h->stor_begin);
    Free(h->index_begin);
  }
  return 0;
}

/**
 * \ingroup indheap
 * \brief Checks whether a heap is empty.
 */

int indheap_empty          (indheap_t* h) {
  return h->stor_begin == h->end;
}

/**
 * \ingroup indheap
 * \brief Adds an element to an indexed heap.
 */

int indheap_push           (indheap_t* h, real_t elem) {
  assert(h != NULL);
	
  /* full, allocate more storage */
  if (h->stor_end == h->end) {
    long int new_size = indheap_size(h) * 2;
    if (new_size == 0) { new_size = 1; }
    indheap_reserve(h, new_size);
  }
	
  *(h->end) = elem;
  h->end += 1;
  *(h->index_begin+indheap_size(h)-1)=indheap_size(h)-1;

  /* maintain indheap */
  indheap_i_shift_up(h, indheap_size(h)-1);
	
  return 0;
}

/**
 * \ingroup indheap
 * \brief Returns the largest element in an indexed heap.
 */

real_t indheap_max       (indheap_t* h) {
  assert(h != NULL);
  assert(h->stor_begin != NULL);
  assert(h->stor_begin != h->end);
  
  return h->stor_begin[0];
}

/**
 * \ingroup indheap
 * \brief Removes the largest element from an indexed heap.
 */

real_t indheap_delete_max(indheap_t* h) {
  real_t tmp;

  assert(h != NULL);
  assert(h->stor_begin != NULL);

  tmp=h->stor_begin[0];
  indheap_i_switch(h, 0, indheap_size(h)-1);
  h->end -= 1;
  indheap_i_sink(h, 0);
  
  return tmp;
}

/**
 * \ingroup indheap
 * \brief Gives the number of elements in an indexed heap.
 */

long int indheap_size      (indheap_t* h) {
  assert (h != NULL);
  return h->end - h->stor_begin;
}

/**
 * \ingroup indheap
 * \brief Reserves more memory for an indexed heap.
 */

int indheap_reserve        (indheap_t* h, long int size) {
  long int actual_size=indheap_size(h);
  assert(h != NULL);
  
  if (size <= actual_size) { return 0; }
  
  h->stor_begin=Realloc(h->stor_begin, size, real_t);
  h->stor_end=h->stor_begin + size;
  h->end=h->stor_begin+actual_size;
  h->index_begin=Realloc(h->index_begin, size, long int);
  
  return 0;
}

/**
 * \ingroup indheap
 * \brief Returns the index of the largest element in an indexed heap.
 */

long int indheap_max_index(indheap_t *h) {
  return h->index_begin[0];  
}

/**
 * \ingroup indheap
 * \brief Builds an indexed heap, this function should not be called
 * directly. 
 */

int indheap_i_build(indheap_t* h, long int head) {

  long int size=indheap_size(h);
  if (RIGHTCHILD(head) < size) { 
    /* both subtrees */
    indheap_i_build(h, LEFTCHILD(head) );
    indheap_i_build(h, RIGHTCHILD(head));
    indheap_i_sink(h, head);
  } else if (LEFTCHILD(head) < size) {
    /* only left */
    indheap_i_build(h, LEFTCHILD(head));
    indheap_i_sink(h, head);
  } else {
    /* none */
  }
  return 0;
}

/**
 * \ingroup indheap
 * \brief Moves an element up in the heap, don't call this function
 * directly. 
 */

int indheap_i_shift_up(indheap_t *h, long int elem) {
  
  if (elem==0 || h->stor_begin[elem] < h->stor_begin[PARENT(elem)]) { 
    /* at the top */
  } else {
    indheap_i_switch(h, elem, PARENT(elem));
    indheap_i_shift_up(h, PARENT(elem));
  }
  return 0;
}

/**
 * \ingroup indheap
 * \brief Moves an element down in the heap, don't call this function
 * directly. 
 */

int indheap_i_sink(indheap_t* h, long int head) {

  long int size=indheap_size(h);
  if (LEFTCHILD(head) >= size) { 
    /* no subtrees */
  } else if (RIGHTCHILD(head) == size ||
	     h->stor_begin[LEFTCHILD(head)]>=h->stor_begin[RIGHTCHILD(head)]) {
    /* sink to the left if needed */
    if (h->stor_begin[head] < h->stor_begin[LEFTCHILD(head)]) {
      indheap_i_switch(h, head, LEFTCHILD(head));
      indheap_i_sink(h, LEFTCHILD(head));
    }
  } else {
    /* sink to the right */
    if (h->stor_begin[head] < h->stor_begin[RIGHTCHILD(head)]) {
      indheap_i_switch(h, head, RIGHTCHILD(head));
      indheap_i_sink(h, RIGHTCHILD(head));
    }
  }

  return 0;
}

/**
 * \ingroup indheap
 * \brief Switches two elements in a heap, don't call this function
 * directly. 
 */

int indheap_i_switch(indheap_t* h, long int e1, long int e2) {
  if (e1!=e2) {
    real_t tmp=h->stor_begin[e1];
    h->stor_begin[e1]=h->stor_begin[e2];
    h->stor_begin[e2]=tmp;
    
    tmp=h->index_begin[e1];
    h->index_begin[e1]=h->index_begin[e2];
    h->index_begin[e2]=tmp;
  }

  return 0;
}


/**
 * \ingroup doubleindheap
 * \brief Initializes an empty doubly indexed heap object.
 */

int d_indheap_init           (d_indheap_t* h, long int alloc_size) {
 if (alloc_size <= 0 ) { alloc_size=1; }
  h->stor_begin=Calloc(alloc_size, real_t);
  h->stor_end=h->stor_begin + alloc_size;
  h->end=h->stor_begin;
  h->destroy=1;
  h->index_begin=Calloc(alloc_size, long int);
  h->index2_begin=Calloc(alloc_size, long int);
  
  return 0;  
}

/**
 * \ingroup doubleindheap
 * \brief Destroys an initialized doubly indexed heap object.
 */

int d_indheap_destroy        (d_indheap_t* h) {  
  if (h->destroy) {
    assert( h->stor_begin != NULL);
    Free(h->stor_begin);
    Free(h->index_begin);
    Free(h->index2_begin);
  }
  return 0;
}

/**
 * \ingroup doubleindheap
 * \brief Decides whether a heap is empty.
 */

int d_indheap_empty          (d_indheap_t* h) {
  return h->stor_begin == h->end;
}

/**
 * \ingroup doubleindheap
 * \brief Adds an element to the heap.
 */

int d_indheap_push           (d_indheap_t* h, real_t elem, 
			      long int idx, long int idx2) {
  assert(h != NULL);
	
  /* full, allocate more storage */
  if (h->stor_end == h->end) {
    long int new_size = d_indheap_size(h) * 2;
    if (new_size == 0) { new_size = 1; }
    d_indheap_reserve(h, new_size);
  }
	
  *(h->end) = elem;
  h->end += 1;
  *(h->index_begin+d_indheap_size(h)-1)=idx ;
  *(h->index2_begin+d_indheap_size(h)-1)=idx2 ;

  /* maintain d_indheap */
  d_indheap_i_shift_up(h, d_indheap_size(h)-1);
	
  return 0;
}

/**
 * \ingroup doubleindheap
 * \brief Returns the largest element in the heap.
 */

real_t d_indheap_max       (d_indheap_t* h) {
  assert(h != NULL);
  assert(h->stor_begin != NULL);
  assert(h->stor_begin != h->end);
  
  return h->stor_begin[0];
}

/**
 * \ingroup doubleindheap
 * \brief Removes the largest element from the heap.
 */

real_t d_indheap_delete_max(d_indheap_t* h) {
  real_t tmp;

  assert(h != NULL);
  assert(h->stor_begin != NULL);

  tmp=h->stor_begin[0];
  d_indheap_i_switch(h, 0, d_indheap_size(h)-1);
  h->end -= 1;
  d_indheap_i_sink(h, 0);
  
  return tmp;
}

/**
 * \ingroup doubleindheap
 * \brief Gives the number of elements in the heap.
 */

long int d_indheap_size      (d_indheap_t* h) {
  assert (h != NULL);
  return h->end - h->stor_begin;
}

/**
 * \ingroup doubleindheap
 * \brief Allocates memory for a heap.
 */

int d_indheap_reserve        (d_indheap_t* h, long int size) {
  long int actual_size=d_indheap_size(h);
  assert(h != NULL);
  
  if (size <= actual_size) { return 0; }
  
  h->stor_begin=Realloc(h->stor_begin, size, real_t);
  h->stor_end=h->stor_begin + size;
  h->end=h->stor_begin+actual_size;
  h->index_begin=Realloc(h->index_begin, size, long int);
  h->index2_begin=Realloc(h->index2_begin, size, long int);
  
  return 0;
}

/**
 * \ingroup doubleindheap
 * \brief Gives the indices of the maximal element in the heap.
 */

int d_indheap_max_index(d_indheap_t *h, long int *idx, long int *idx2) {
  (*idx)=h->index_begin[0];
  (*idx2)=h->index2_begin[0];

  return 0;
}

/**
 * \ingroup doubleindheap
 * \brief Builds the heap, don't call it directly.
 */

int d_indheap_i_build(d_indheap_t* h, long int head) {

  long int size=d_indheap_size(h);
  if (RIGHTCHILD(head) < size) { 
    /* both subtrees */
    d_indheap_i_build(h, LEFTCHILD(head) );
    d_indheap_i_build(h, RIGHTCHILD(head));
    d_indheap_i_sink(h, head);
  } else if (LEFTCHILD(head) < size) {
    /* only left */
    d_indheap_i_build(h, LEFTCHILD(head));
    d_indheap_i_sink(h, head);
  } else {
    /* none */
  }
  return 0;
}

/**
 * \ingroup doubleindheap
 * \brief Moves an element up in the heap, don't call it directly.
 */

int d_indheap_i_shift_up(d_indheap_t *h, long int elem) {
  
  if (elem==0 || h->stor_begin[elem] < h->stor_begin[PARENT(elem)]) { 
    /* at the top */
  } else {
    d_indheap_i_switch(h, elem, PARENT(elem));
    d_indheap_i_shift_up(h, PARENT(elem));
  }
  return 0;
}

/**
 * \ingroup doubleindheap
 * \brief Moves an element down in the heap, don't call it directly.
 */

int d_indheap_i_sink(d_indheap_t* h, long int head) {

  long int size=d_indheap_size(h);
  if (LEFTCHILD(head) >= size) { 
    /* no subtrees */
  } else if (RIGHTCHILD(head) == size ||
	     h->stor_begin[LEFTCHILD(head)]>=h->stor_begin[RIGHTCHILD(head)]) {
    /* sink to the left if needed */
    if (h->stor_begin[head] < h->stor_begin[LEFTCHILD(head)]) {
      d_indheap_i_switch(h, head, LEFTCHILD(head));
      d_indheap_i_sink(h, LEFTCHILD(head));
    }
  } else {
    /* sink to the right */
    if (h->stor_begin[head] < h->stor_begin[RIGHTCHILD(head)]) {
      d_indheap_i_switch(h, head, RIGHTCHILD(head));
      d_indheap_i_sink(h, RIGHTCHILD(head));
    }
  }

  return 0;
}

/**
 * \ingroup doubleindheap
 * \brief Switches two elements in the heap, don't call it directly.
 */

int d_indheap_i_switch(d_indheap_t* h, long int e1, long int e2) {
  if (e1!=e2) {
    long int tmpi;
    real_t tmp=h->stor_begin[e1];
    h->stor_begin[e1]=h->stor_begin[e2];
    h->stor_begin[e2]=tmp;
    
    tmpi=h->index_begin[e1];
    h->index_begin[e1]=h->index_begin[e2];
    h->index_begin[e2]=tmpi;

    tmpi=h->index2_begin[e1];
    h->index2_begin[e1]=h->index2_begin[e2];
    h->index2_begin[e2]=tmpi;
  }

  return 0;
}

/**
 * \ingroup internal
 */

int vector_ptr_init      (vector_ptr_t* v, int long size) {	
        long int alloc_size= size > 0 ? size : 1;
	assert(v != NULL);
	if (size < 0) { size=0; }
	v->stor_begin=Calloc(alloc_size, void*);
	v->stor_end=v->stor_begin + alloc_size;
	v->end=v->stor_begin+size;

	return 0;
}

/**
 * \ingroup internal
 */

int vector_ptr_destroy   (vector_ptr_t* v) {
	assert(v != NULL);
	assert(v->stor_begin != NULL);

	Free(v->stor_begin);
	v->stor_begin = NULL;
	
	return 0;
}

int vector_ptr_free_all   (vector_ptr_t* v) {
  void **ptr;
  for (ptr=v->stor_begin; ptr<v->end; ptr++) {
    Free(*ptr);
  }
  return 0;
}

int vector_ptr_destroy_all   (vector_ptr_t* v) {
  vector_ptr_free_all(v);
  vector_ptr_destroy(v);
  return 0;
}

/**
 * \ingroup internal
 */

int vector_ptr_reserve   (vector_ptr_t* v, long int size) {
	long int actual_size=vector_ptr_size(v);
	assert(v != NULL);
	
	if (size <= vector_ptr_size(v)) { return 0; }

	v->stor_begin=Realloc(v->stor_begin, size, void*);
	v->stor_end=v->stor_begin + size;
	v->end=v->stor_begin+actual_size;
	
	return 0;
}

/**
 * \ingroup internal
 */

int vector_ptr_empty     (vector_ptr_t* v) {
	assert(v != NULL);
	assert(v->stor_begin != NULL);
	
	return v->stor_begin == v->end;
}

/**
 * \ingroup internal
 */

long int vector_ptr_size      (vector_ptr_t* v) {
	assert(v != NULL);
	
	return v->end - v->stor_begin;
}

/**
 * \ingroup internal
 */

int vector_ptr_clear     (vector_ptr_t* v) {
	assert(v != NULL);
	
	v->end = v->stor_begin;
	
	return 0;
}

/**
 * \ingroup internal
 */

int vector_ptr_push_back (vector_ptr_t* v, void* e) {
	assert(v != NULL);

	/* full, allocate more storage */
	if (v->stor_end == v->end) {
		long int new_size = vector_ptr_size(v) * 2;
		if (new_size == 0) { new_size = 1; }
		vector_ptr_reserve(v, new_size);
	}
	
	*(v->end) = e;
	v->end += 1;
	
	return 0;
}

/**
 * \ingroup internal
 */

void* vector_ptr_e         (vector_ptr_t* v, long int pos) {
	assert(v != NULL);
	
	return * (v->stor_begin + pos);
}

/**
 * \ingroup internal
 */

int vector_ptr_set       (vector_ptr_t* v, long int pos, void* value) {
	assert(v != NULL);
	
	*(v->stor_begin + pos) = value;
	return 0;
}

/**
 * \ingroup internal
 */

int vector_ptr_null      (vector_ptr_t* v) {
	if (vector_ptr_size(v)>0) {
		memset(v->stor_begin, 0, sizeof(void*)*vector_ptr_size(v));
	}
	return 0;
}

/**
 * \ingroup internal
 */

long int vector_ptr_find(vector_ptr_t* v, void* elem) {
  long int i, s;
  assert(v!=NULL);
  s=vector_ptr_size(v);
  for (i=0; i<s; i++) {
    if (vector_ptr_e(v, i)==elem) {
      return i;
    }
  }
  return -1;
}

/**
 * \ingroup internal
 */

int vector_ptr_change(vector_ptr_t* v, long int pos1, long int pos2) {
  void* tmp;
  assert(v!=NULL);
  tmp=vector_ptr_e(v, pos1);
  vector_ptr_set(v, pos1, vector_ptr_e(v, pos2));
  vector_ptr_set(v, pos2, tmp);
  return 0;
}

/**
 * \ingroup internal
 */

int vector_ptr_resize(vector_ptr_t* v, long int newsize) {
  vector_ptr_reserve(v, newsize);
  v->end = v->stor_begin+newsize;
  return 0;
}

/**
 * \ingroup internal
 */

vector_ptr_t vector_ptr_as_vector(void** data, long int length) {
  vector_ptr_t v;
  v.stor_begin=data;
  v.stor_end=v.end=data+length;
  
  return v;
}

/**
 * \ingroup internal
 */

int vector_ptr_init_copy(vector_ptr_t *v, void* *data, long int length) {
  v->stor_begin=Calloc(length, void*);
  v->stor_end=v->stor_begin+length;
  v->end=v->stor_end;
  memcpy(v->stor_begin, data, length*sizeof(void*));
  
  return 0;
}

/**
 * \ingroup internal
 */

int vector_ptr_copy_to(vector_ptr_t *v, void** to) {
  if (v->end != v->stor_begin) {
    memcpy(to, v->stor_begin, sizeof(void*) * (v->end - v->stor_begin));
  }
  return 0;
}

/**
 * \ingroup internal
 */

int vector_ptr_copy(vector_ptr_t *to, vector_ptr_t *from) {
  to->stor_begin=Calloc(vector_ptr_size(from), void*);
  to->stor_end=to->stor_begin+vector_ptr_size(from);
  to->end=to->stor_end;
  memcpy(to->stor_begin, from->stor_begin, vector_ptr_size(from)*sizeof(void*));
  
  return 0;
}

/**
 * \ingroup internal
 */

int vector_ptr_remove(vector_ptr_t *v, long int pos) {

  memcpy(v->stor_begin+pos, v->stor_begin+pos+1,
	 sizeof(void*)*(vector_ptr_size(v)-pos));
  v->end--;
  
  return 0;
}

/**
 * \ingroup internal
 */

int igraph_strvector_init(igraph_strvector_t *sv, long int len) {
  sv->data=Calloc(len, char*);
  sv->len=len;

  return 0;
}

/**
 * \ingroup internal
 */

int igraph_strvector_destroy(igraph_strvector_t *sv) {
  long int i;
  for (i=0; i<sv->len; i++) {
    if (sv->data[i] != 0) {
      Free(sv->data[i]);
    }
  }
  Free(sv->data);
  
  return 0;
}

/**
 * \ingroup internal
 */

int igraph_strvector_get(igraph_strvector_t *sv, long int idx, char **value) {
  if (sv->data[idx] == 0) {
    sv->data[idx] = Calloc(1, char);
  }
  *value = sv->data[idx];
  
  return 0;
}

/**
 * \ingroup internal
 */

int igraph_strvector_set(igraph_strvector_t *sv, long int idx, 
			 const char *value) {
  if (sv->data[idx] == 0) {
    sv->data[idx] = Calloc(strlen(value)+1, char);
  } else {
    sv->data[idx] = Realloc(sv->data[idx], strlen(value)+1, char);
  }
  strcpy(sv->data[idx], value);
  
  return 0;
}

/**
 * \ingroup internal
 */

int igraph_strvector_remove_section(igraph_strvector_t *v, long int from, 
				    long int to) {
  long int i;
  for (i=from; i<to; i++) {
    if (v->data[i] != 0) {
      Free(v->data[i]);
    }
  }
  for (i=0; i<v->len-to; i++) {
    v->data[from+i]=v->data[to+i];
  }

  v->data=Realloc(v->data, v->len-(to-from), char*);
  v->len -= (to-from);

  return 0;
}

/**
 * \ingroup internal
 */

int igraph_strvector_remove(igraph_strvector_t *v, long int elem) {
  return igraph_strvector_remove_section(v, elem, elem+1);
}

/**
 * \ingroup internal
 */

int igraph_strvector_move_interval(igraph_strvector_t *v, long int begin, 
				   long int end, long int to) {
  long int i;
  for (i=to; i<to+end-begin; i++) {
    if (v->data[i] != 0) {
      Free(v->data[i]);
    }
  }
  for (i=0; i<end-begin; i++) {
    v->data[to+i]=v->data[begin+i];
  }
  
  return 0;
}

/**
 * \ingroup internal
 */

int igraph_strvector_copy(igraph_strvector_t *to, igraph_strvector_t *from) {
  long int i;
  char *str;
  to->data=Calloc(from->len, char*);
  to->len=from->len;
  
  for (i=0; i<from->len; i++) {
    igraph_strvector_get(from, i, &str);
    igraph_strvector_set(to, i, str);
  }
  
  return 0;
}

/**
 * \ingroup internal
 */

int igraph_strvector_resize(igraph_strvector_t* v, long int newsize) {
  long int toadd=newsize-v->len;
  
  if (newsize < v->len) { 
    long int i;
    for (i=v->len; i<newsize; i++) {
      Free(v->data[i]);
    }
  }
  
  v->data = Realloc(v->data, newsize, char*);
  if (newsize > v->len) {
    memset(v->data+v->len, 0, toadd*sizeof(char*));
  }
  v->len = newsize;
  
  return 0;
}

/**
 * \ingroup internal
 */

long int igraph_strvector_size(igraph_strvector_t *sv) {
  return sv->len;
}

/**
 * \ingroup internal
 */

int igraph_strvector_add(igraph_strvector_t *v, const char *value) {
  long int s=igraph_strvector_size(v);
  igraph_strvector_resize(v, s+1);
  igraph_strvector_set(v, s, value);
  return 0;
}

/**
 * \ingroup internal
 */

int igraph_strvector_permdelete(igraph_strvector_t *v, long int *index, 
				long int nremove) {
  long int i;
  for (i=0; i<igraph_strvector_size(v); i++) {
    v->data[ index[i]-1 ] = v->data[i];
  }
  igraph_strvector_resize(v, igraph_strvector_size(v)-nremove);

  return 0;
}

/**
 * \ingroup internal
 */

int igraph_strvector_remove_negidx(igraph_strvector_t *v, vector_t *neg, 
				   long int nremove) {
  long int i, idx=0;
  for (i=0; i<igraph_strvector_size(v); i++) {
    v->data[idx++] = v->data[i];
  }
  igraph_strvector_resize(v, igraph_strvector_size(v)-nremove);

  return 0;
}

/**
 * \ingroup internal
 */

int igraph_strmatrix_init(igraph_strmatrix_t *sm, 
			  long int nrow, long int ncol) {
  sm->nrow=nrow;
  sm->ncol=ncol;
  igraph_strvector_init(&sm->data, nrow*ncol);
  
  return 0;
}

/**
 * \ingroup internal
 */

int igraph_strmatrix_destroy(igraph_strmatrix_t *sm) {
  igraph_strvector_destroy(&sm->data);
  
  return 0;
}

/**
 * \ingroup internal
 */

int igraph_strmatrix_get(igraph_strmatrix_t *sm, long int row, long int col,
			  char **value) {
  igraph_strvector_get(&sm->data, col*(sm->nrow)+row, value);

  return 0;
}

/**
 * \ingroup internal
 */

int igraph_strmatrix_set(igraph_strmatrix_t *sm, long int row, long int col,
			 const char *value) {
  igraph_strvector_set(&sm->data, col*(sm->nrow)+row, value);
  
  return 0;
}

/**
 * \ingroup internal
 */

int igraph_strmatrix_add_cols(igraph_strmatrix_t *sm, long int n) {
  igraph_strmatrix_resize(sm, sm->nrow, sm->ncol+n);
  return 0;
}

/**
 * \ingroup internal
 */

int igraph_strmatrix_remove_col(igraph_strmatrix_t *m, long int col) {
  igraph_strvector_remove_section(&m->data, (m->nrow)*col, (m->nrow)*(col+1));
  m->ncol--;
  return 0;
}

/**
 * \ingroup internal
 */

int igraph_strmatrix_add_rows(igraph_strmatrix_t *m, long int n) {
  long int i;
  igraph_strvector_resize(&m->data, (m->ncol)*(m->nrow+n));
  for (i=m->ncol-1; i>=0; i--) {
    igraph_strvector_move_interval(&m->data, (m->nrow)*i, (m->nrow)*(i+1),
				   (m->nrow+n)*i);
  }
  m->nrow += n;
  return 0;
}

/**
 * \ingroup internal
 */

int igraph_strmatrix_permdelete_rows(igraph_strmatrix_t *m, long int *index, 
				     long int nremove) {
  long int i, j;
  char *str;
  for (i=0; i<m->ncol; i++) {
    for (j=0; j<m->nrow; j++) {
      if (index[j] != 0) {
	igraph_strmatrix_get(m, j, i, &str);
	igraph_strmatrix_set(m, index[j]-1, i, str);
      }
    }
  }
  igraph_strmatrix_resize(m, m->nrow-nremove, m->ncol);

  return 0;
}

/**
 * \ingroup internal
 */

int igraph_strmatrix_delete_rows_neg(igraph_strmatrix_t *m, 
				     vector_t *neg, long int nremove) {
  long int i, j, idx=0;
  char *str;
  for (i=0; i<m->ncol; i++) {
    for (j=0; j<m->nrow; j++) {
      if (VECTOR(*neg)[j] >= 0) {
	igraph_strmatrix_get(m, j, i, &str);
	igraph_strmatrix_set(m, idx++, i, str);
      } 
    }
    idx=0;
  }
  igraph_strmatrix_resize(m, m->nrow-nremove, m->ncol);

  return 0;
}

/**
 * \ingroup internal
 */

int igraph_strmatrix_copy(igraph_strmatrix_t *to, igraph_strmatrix_t *from) {
  igraph_strvector_copy(&to->data, &from->data);
  to->nrow = from->nrow;
  to->ncol = from->ncol;
  return 0;
}

/**
 * \ingroup internal
 */

int igraph_strmatrix_resize(igraph_strmatrix_t *sm, 
			    long int nrow, long int ncol) {
  igraph_strvector_resize(&sm->data, nrow*ncol);
  sm->nrow=nrow;
  sm->ncol=ncol;

  return 0;
}

/**
 * \ingroup internal
 */

long int igraph_strmatrix_nrow(igraph_strmatrix_t *sm) {
  return sm->nrow;
}

/**
 * \ingroup internal
 */

long int igraph_strmatrix_ncol(igraph_strmatrix_t *sm) {
  return sm->ncol;
}



/**********************************************************
 * Testing purposes, vector                               *
 *********************************************************/

/* int print_vector(vector_t *v) { */
/*   long int i; */
/*   for (i=0; i<vector_size(v); i++) { */
/*     printf("%f ", VECTOR(*v)[i]); */
/*   } */
/*   printf("\n"); */
  
/*   return 0; */
/* } */

/* int main() { */
  
/*   vector_t v, order; */
/*   long int i; */
/*   vector_init(&v, 10); */
/*   vector_null(&v); */

/*   /\* indexing *\/ */
/*   for (i=0; i<vector_size(&v); i++) { */
/*     VECTOR(v)[i]=i+1; */
/*   } */
/*   print_vector(&v);   */
  
/*   /\* resize *\/ */
/*   vector_resize(&v, 12); */
/*   VECTOR(v)[10]=100; */
/*   VECTOR(v)[11]=1; */
/*   print_vector(&v); */
  
/*   /\* order *\/ */
/*   vector_init(&order,0); */
/*   vector_order(&v, &order, 100); */
/*   print_vector(&order); */

/*   return 0; */
/* } */

/**********************************************************
 * Testing purposes, attribute list                       *
 *********************************************************/

/* #include <stdio.h> */

/* int print_vector(vector_t *v) { */
/*   long int i; */
/*   printf("%p %p %p ", v->stor_begin, v->end, v->stor_end); */
/*   for (i=0; i<vector_size(v); i++) { */
/*     printf("%f ", VECTOR(*v)[i]); */
/*   } */
/*   printf("\n"); */
  
/*   return 0; */
/* } */

/* int print_matrix(matrix_t *m) { */
/*   long int i,j; */
/*   for (i=0; i<m->nrow; i++) { */
/*     for (j=0; j<m->ncol; j++) { */
/*       printf("%f ", MATRIX(*m, i, j)); */
/*     } */
/*     printf("\n"); */
/*   } */
  
/*   return 0; */
/* } */

/* int print_attribute_list(igraph_attribute_list_t *al) { */
/*   char *ptr; */
/*   printf("strings: %li\n", al->nstr); */
/*   printf("internal:"); */
/*   for (ptr=al->sa_begin; ptr<al->sa_end; ptr++) { */
/*     printf("%i ", (int) *ptr); */
/*   } */
/*   printf("\n"); */
/*   printf("sa_begin: %p, sa_end: %p\n", */
/* 	 al->sa_begin, al->sa_end); */
/*   print_matrix(&al->numattrs); */
/*   return 0; */
/* } */

/* int main() { */
  
/*   igraph_attribute_list_t al; */
/*   real_t value; */
  
/*   /\* Init *\/ */
/*   printf("------------------- INIT --------------------\n"); */
/*   igraph_attribute_list_init(&al, 1); */
/*   print_attribute_list(&al); */
  
/*   /\* Add *\/ */
/*   printf("------------------- ADD --------------------\n"); */
/*   igraph_attribute_list_add(&al, "teszt"); */
/*   print_attribute_list(&al); */

/*   igraph_attribute_list_add(&al, "teszt_izebize"); */
/*   print_attribute_list(&al); */

/*   /\* Get *\/ */
/*   printf("------------------- GET --------------------\n"); */
/*   igraph_attribute_list_get(&al, "teszt", 0, &value); */
/*   printf("data: %f\n", value); */
/*   igraph_attribute_list_get(&al, "teszt_izebize", 0, &value); */
/*   printf("data: %f\n", value); */

/*   /\* Set *\/ */
/*   printf("------------------- SET --------------------\n"); */
/*   igraph_attribute_list_set(&al, "teszt", 0, 10); */
/*   print_attribute_list(&al);   */
/*   igraph_attribute_list_get(&al, "teszt", 0, &value); */
/*   printf("data: %f\n", value); */

/*   /\* Remove *\/ */
/*   printf("------------------- REMOVE --------------------\n"); */
/*   igraph_attribute_list_remove(&al, "teszt"); */
/*   print_attribute_list(&al); */

/*   igraph_attribute_list_add(&al, "mize"); */
/*   print_attribute_list(&al); */

/*   igraph_attribute_list_remove(&al, "mize"); */
/*   print_attribute_list(&al); */

/*   /\* Destroy *\/ */
/*   printf("------------------- DESTROY --------------------\n"); */
/*   igraph_attribute_list_destroy(&al); */

/*   return 0; */
/* } */
