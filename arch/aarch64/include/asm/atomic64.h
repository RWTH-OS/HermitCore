/*
 * Copyright (c) 2017, Stefan Lankes, RWTH Aachen University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the University nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @author Stefan Lankes
 * @file arch/arm64/include/asm/atomic.h
 * @brief Functions for atomic operations
 *
 * This file defines functions for atomic operations on int64 variables
 * which will be used in locking-mechanisms.
 */

#ifndef __ARCH_ATOMIC64_H__
#define __ARCH_ATOMIC64_H__

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Standard-datatype for atomic operations
 *
 * It just consists of an int64_t variable internally, marked as volatile.
 */
typedef struct { volatile int64_t counter; } atomic_int64_t;

/** @brief Atomic test and set operation for int64 vars.
 *
 * This function will atomically exchange the value of an atomic variable and
 * return its old value. Is used in locking-operations.\n
 * \n
 * Intel manuals: If a memory operand is referenced, the processor's locking
 * protocol is automatically implemented for the duration of the exchange
 * operation, regardless of the presence or absence of the LOCK prefix.
 *
 * @param d Pointer to the atomic_int_64_t with the value you want to exchange
 * @param v the value you want the var test for
 *
 * @return The old value of the atomic_int_64_t var before exchange
 */
inline static int64_t atomic_int64_test_and_set(atomic_int64_t* d, int64_t v)
{
	asm volatile(
		"%=:\n\t"
		"ldxr x0, %0\n\t"
		"ldr x1, %1\n\t"
		"str x0, %1\n\t"
		"stxr w1, x1, %0\n\t"
		"cbnz w1, %=b"
		: "+Q"(d->counter), "+m"(v)
		:
		: "memory", "x0", "x1");
	return v;
}

/** @brief Atomic addition of values to atomic_int64_t vars
 *
 * This function lets you add values in an atomic operation
 *
 * @param d Pointer to the atomit_int64_t var you want do add a value to
 * @param i The value you want to increment by
 *
 * @return The mathematical result
 */
inline static int64_t atomic_int64_add(atomic_int64_t *d, int64_t i)
{
	asm volatile(
		"ldr x1, %1\n\t"
		"%=:\n\t"
		"ldxr x0, %0\n\t"
		"add x0, x0, x1\n\t"
		"stxr w1, x0, %0\n\t"
		"cbnz w1, %=b\n\t"
		"str x0, %1"
		: "+Q"(d->counter), "+m"(i)
		:
		: "memory", "x0", "x1");
	return i;
}

/** @brief Atomic subtraction of values from atomic_int64_t vars
 *
 * This function lets you subtract values in an atomic operation.\n
 * This function is just for convenience. It uses atomic_int64_add(d, -i)
 *
 * @param d Pointer to the atomic_int64_t var you want to subtract from
 * @param i The value you want to subtract by
 *
 * @return The mathematical result
 */
inline static int64_t atomic_int64_sub(atomic_int64_t *d, int64_t i)
{
        return atomic_int64_add(d, -i);
}

/** @brief Atomic increment by one
 *
 * The atomic_int64_t var will be atomically incremented by one.\n
 *
 * @param d The atomic_int64_t var you want to increment
 */
inline static int64_t atomic_int64_inc(atomic_int64_t* d) {
	return atomic_int64_add(d, 1);
}

/** @brief Atomic decrement by one
 *
 * The atomic_int64_t var will be atomically decremented by one.\n
 *
 * @param d The atomic_int64_t var you want to decrement
 */
inline static int64_t atomic_int64_dec(atomic_int64_t* d) {
	return atomic_int64_add(d, -1);
}

/** @brief Read out an atomic_int64_t var
 *
 * This function is for convenience: It looks into the atomic_int64_t struct
 * and returns the internal value for you.
 *
 * @param d Pointer to the atomic_int64_t var you want to read out
 * @return It's number value
 */
inline static int64_t atomic_int64_read(atomic_int64_t *d) {
	return d->counter;
}

/** @brief Set the value of an atomic_int64_t var
 *
 * This function is for convenience: It sets the internal value of
 * an atomic_int64_t var for you.
 *
 * @param d Pointer to the atomic_int64_t var you want to set
 * @param v The value to set
 */
inline static void atomic_int64_set(atomic_int64_t *d, int64_t v) {
	atomic_int64_test_and_set(d, v);
}

#ifdef __cplusplus
}
#endif

#endif
