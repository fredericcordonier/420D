/*
 * This file comes from Magic Lantern.
 * Thanks to g3gg0 for the idea and implementation.
 * Thanks to Sergei for porting it.
 */

#ifndef _CACHE_HACKS_H_
#define _CACHE_HACKS_H_

#include <vxworks.h>
#include <stdint.h>

#include "asm.h"

/*
 * Canon cameras appear to use the ARMv5 946E.
 * (Confirmed on: 550D, ... )
 *
 * This processor supports a range of cache sizes from no cache (0KB) or
 * 4KB to 1MB in powers of 2. Instruction(icache) and data(dcache) cache sizes
 * can be independent and can not be changed at run time.
 *
 * A cache line is 32 bytes / 8 words / 8 instructions.
 * 	byte address 	(Addr[1:0] = 2 bits)
 * 	word address 	(Addr[4:2] = 2 bits)
 * 	index 			(Addr[i+4:5] = i bits)
 * 	address TAG 	(Addr[31:i+5] = 27 - i bits)
 * Where 'i' is the size of the cache index in bits.
 *
 * There are 2^i cache lines.
 * The index bits from the address select the cache line. The tag bits from the
 * address are compared with the tag of the cache line and if the cache line is
 * valid the byte and word bits extract the data from that cache line.
 *
 * The CP15 Control Register controls cache operation:
 * bit 2  = dcache enable
 * bit 12 = icache enable
 *
 * Self modifying code and reprogramming the protection regions requires a
 * flush of the icache. Writing to CP15 register 7 flushes the
 * cache. Writing a 0 flushes the entire icache. Writing the "FlushAddress"
 * flushes that cache line. Icache automatically flushed on reset. Never needs
 * to be cleaned because it cannot be written to.
 *
 * Dcache is automatically disabled and flushed on reset.
 */

#define RET_INSTR       0xE12FFF1E      // bx lr
#define FAR_CALL_INSTR  0xE51FF004      // ldr pc, [pc,#-4]
#define LOOP_INSTR      0xEAFFFFFE      // 1: b 1b
#define NOP_INSTR       0xE1A00000      // mov r0, r0
#define MOV_R0_1_INSTR  0xE3A00001	// mov r0, 1
#define MOV_R0_0_INSTR  0xE3A00000	// mov r0, 0

#define BL_INSTR(pc,dest) \
    ( 0xEB000000 \
    | ((( ((uint32_t)dest) - ((uint32_t)pc) - 8 ) >> 2) & 0x00FFFFFF) \
    )

#define B_INSTR(pc,dest) \
    ( 0xEA000000 \
    | ((( ((uint32_t)dest) - ((uint32_t)pc) - 8 ) >> 2) & 0x00FFFFFF) \
    )

#define TYPE_DCACHE 0
#define TYPE_ICACHE 1

/* get cache size depending on cache type and processor setup (13 -> 2^13 -> 8192 -> 8KiB) */
#define CACHE_SIZE_BITS(t)          cache_get_size(t)

/* depending on cache size, INDEX has different length */
#define CACHE_INDEX_BITS(t)         (CACHE_SIZE_BITS(t)-7)
/* INDEX in tag field starts at bit 5 */
#define CACHE_INDEX_TAGOFFSET(t)    5
/* bitmask that matches the INDEX value bits */
#define CACHE_INDEX_BITMASK(t)      ((1U<<CACHE_INDEX_BITS(t)) - 1)
/* bitmask to mask out the INDEX field in a tag */
#define CACHE_INDEX_ADDRMASK(t)     (CACHE_INDEX_BITMASK(t)<<CACHE_INDEX_TAGOFFSET(t))

/* depending on cache size, TAG has different length */
#define CACHE_TAG_BITS(t)           (27-CACHE_INDEX_BITS(t))
/* TAG in tag field starts at bit 5 plus INDEX size */
#define CACHE_TAG_TAGOFFSET(t)      (5+CACHE_INDEX_BITS(t))
/* bitmask that matches the TAG value bits */
#define CACHE_TAG_BITMASK(t)        ((1U<<CACHE_TAG_BITS(t)) - 1)
/* bitmask to mask out the TAG field in a tag */
#define CACHE_TAG_ADDRMASK(t)       (CACHE_TAG_BITMASK(t)<<CACHE_TAG_TAGOFFSET(t))

/* the WORD field in tags is always 3 bits */
#define CACHE_WORD_BITS(t)          3
/* WORD in tag field starts at this bit position */
#define CACHE_WORD_TAGOFFSET(t)     2
/* bitmask that matches the WORD value bits */
#define CACHE_WORD_BITMASK(t)       ((1U<<CACHE_WORD_BITS(t)) - 1)
/* bitmask to mask out the WORD field in a tag */
#define CACHE_WORD_ADDRMASK(t)      (CACHE_WORD_BITMASK(t)<<CACHE_WORD_TAGOFFSET(t))

/* the SEGMENT field in tags is always 2 bits */
#define CACHE_SEGMENT_BITS(t)       2
/* SEGMENT in tag field starts at this bit position */
#define CACHE_SEGMENT_TAGOFFSET(t)  30
/* bitmask that matches the SEGMENT value bits */
#define CACHE_SEGMENT_BITMASK(t)    ((1U<<CACHE_SEGMENT_BITS(t)) - 1)
/* bitmask to mask out the SEGMENT field in a tag */
#define CACHE_SEGMENT_ADDRMASK(t)   (CACHE_SEGMENT_BITMASK(t)<<CACHE_SEGMENT_TAGOFFSET(t))


static inline uint32_t cli(void)
{
	uint32_t old_cpsr = 0;
	uint32_t new_cpsr = 0;
	asm volatile (
		"MRS %0, CPSR\n"
		"ORR %1, %0, #0x80\n" // set I flag to disable IRQ
		"MSR CPSR_c, %1\n"

		: "=r"(old_cpsr)
		: "r"(new_cpsr)
	);
	return old_cpsr & 0x80; // return true if the flags are set
}

static inline void sei( uint32_t old_cpsr )
{
	uint32_t new_cpsr = 0;
	asm volatile (
		"MRS %0, CPSR\n"
		"BIC %0, %0, #0x80\n"
		"ORR %0, %0, %1\n"
		"MSR CPSR_c, %0"

		:
		: "r"(new_cpsr), "r"(old_cpsr)
	);
}

/* return cache size in bits (13 -> 2^13 -> 8192 -> 8KiB) */
static uint32_t cache_get_size(uint32_t type)
{
	uint32_t cache_info = 0;

	// get cache type register
	// On a 550D: 0x0F112112. 8KB I/D Cache. 4 way set associative.
	asm volatile (
		"MRC p15, 0, %0, c0, c0, 1\n"

		: "=r"(cache_info)
	);

	/* dcache is described at bit pos 12 */
	if (type == TYPE_DCACHE) {
		cache_info >>= 12;
	}

	/* check if size is invalid, or absent flag is set */
	uint32_t size = (cache_info >> 6) & 0x0F;
	uint32_t absent = (cache_info >> 2) & 0x01;

	if((size < 3) || absent) {
		return 0;
	}

	/* return as 2^x */
	return size + 9;
}

static uint32_t cache_patch_single_word(uint32_t address, uint32_t data, uint32_t type)
{
	uint32_t cache_seg_index_word = (address & (CACHE_INDEX_ADDRMASK(type) | CACHE_WORD_ADDRMASK(type)));
	uint32_t cache_tag_index = (address & (CACHE_TAG_ADDRMASK(type) | CACHE_INDEX_ADDRMASK(type))) | 0x10;

	if(type == TYPE_ICACHE) {
		asm volatile (
			/* write index for address to write */
			"MCR p15, 3, %0, c15, c0, 0\n"
			/* set TAG at given index */
			"MCR p15, 3, %1, c15, c1, 0\n"
			/* write instruction */
			"MCR p15, 3, %2, c15, c3, 0\n"

			:
			: "r"(cache_seg_index_word), "r"(cache_tag_index), "r"(data)
		);
	} else {
		asm volatile (
			/* write index for address to write */
			"MCR p15, 3, %0, c15, c0, 0\n"
			/* set TAG at given index */
			"MCR p15, 3, %1, c15, c2, 0\n"
			/* write data */
			"MCR p15, 3, %2, c15, c4, 0\n"

			:
			: "r"(cache_seg_index_word), "r"(cache_tag_index), "r"(data));
	}

	return 1;
}

/*	fetch all the instructions in that temp_cacheline the given address is in.
	this is *required* before patching a single address.
	if you omit this, only the patched instruction will be correct, the
	other instructions around will simply be crap.
	warning - this is doing a data fetch (LDR) so DCache patches may cause
	unwanted or wanted behavior. make sure you know what you do :)

	same applies to dcache routines
*/
static void cache_fetch_line(uint32_t address, uint32_t type)
{
	uint32_t base = (address & ~0x1F);
	uint32_t temp_cacheline[8];

	// our ARM946 has 0x20 byte temp_cachelines. fetch the current line
	// thanks to unified memories, we can do LDR on instructions.
	uint32_t pos;
	for(pos = 0; pos < 8; pos++) {
		temp_cacheline[pos] = ((uint32_t *)base)[pos];
	}

	/* and nail it into locked cache */
	for(pos = 0; pos < 8; pos++) {
		cache_patch_single_word(base + pos * 4, temp_cacheline[pos], type);
	}
}

/* return the tag and content at given index (segment+index+word) */
static void cache_get_content(uint32_t segment, uint32_t index, uint32_t word, uint32_t type, uint32_t *tag, uint32_t *data)
{
	uint32_t cache_seg_index_word = 0;
	uint32_t stored_tag_index = 0;
	uint32_t stored_data = 0;

	cache_seg_index_word |= ((segment & CACHE_SEGMENT_BITMASK(type)) << CACHE_SEGMENT_TAGOFFSET(type));
	cache_seg_index_word |= ((index & CACHE_INDEX_BITMASK(type)) << CACHE_INDEX_TAGOFFSET(type));
	cache_seg_index_word |= ((word & CACHE_WORD_BITMASK(type)) << CACHE_WORD_TAGOFFSET(type));

	if(type == TYPE_ICACHE) {
		asm volatile (
			/* write index for address to write */
			"MCR p15, 3, %2, c15, c0, 0\n"
			/* get TAG at given index */
			"MRC p15, 3, %0, c15, c1, 0\n"
			/* get DATA at given index */
			"MRC p15, 3, %1, c15, c3, 0\n"

			: "=r"(stored_tag_index), "=r"(stored_data)
			: "r"(cache_seg_index_word)
		);
	} else {
		asm volatile (
			/* write index for address to write */
			"MCR p15, 3, %2, c15, c0, 0\n"
			/* get TAG at given index */
			"MRC p15, 3, %0, c15, c2, 0\n"
			/* get DATA at given index */
			"MRC p15, 3, %1, c15, c4, 0\n"

			: "=r"(stored_tag_index), "=r"(stored_data)
			: "r"(cache_seg_index_word)
		);
	}

	*tag = stored_tag_index;
	*data = stored_data;
}

/* check if given address is already used or if it is usable for patching */
static uint32_t __attribute__ ((unused)) cache_is_patchable(uint32_t address, uint32_t type)
{
	uint32_t stored_tag_index = 0;
	uint32_t stored_data = 0;
	cache_get_content(
		0,
		(address & CACHE_INDEX_ADDRMASK(type)) >> CACHE_INDEX_TAGOFFSET(type),
		(address & CACHE_WORD_ADDRMASK(type)) >> CACHE_WORD_TAGOFFSET(type),
		type,
		&stored_tag_index,
		&stored_data
	);

	/* this line is free, so can be used for patching */
	if((stored_tag_index & 0x10) == 0) {
		return 1;
	}

	/* now check if the TAG RAM content matches with what we expect and valid bit is set */
	uint32_t tag_index_mask = CACHE_TAG_ADDRMASK(type) | CACHE_INDEX_ADDRMASK(type);
	uint32_t cache_tag_index = address & tag_index_mask;

	if((stored_tag_index & tag_index_mask) == cache_tag_index) {
		/* that line is used by the right address, now check data */

		if(stored_data == *(uint32_t*)address) { // data is original, so it is patchable
			return 1;
		}

		return 2; // its already patched. so return 2
	}

	/* oh, its already used by some other patch. sorry. */
	return 0;
}

/* check if given address is already in cache */
static uint32_t cache_get_cached(uint32_t address, uint32_t type)
{
	uint32_t cache_seg_index_word = (address & (CACHE_INDEX_ADDRMASK(type) | CACHE_WORD_ADDRMASK(type)));
	uint32_t stored_tag_index = 0;

	if(type == TYPE_ICACHE) {
		asm volatile (
			/* write index for address to write */
			"MCR p15, 3, %1, c15, c0, 0\n"
			/* get TAG at given index */
			"MRC p15, 3, %0, c15, c1, 0\n"

			: "=r"(stored_tag_index)
			: "r"(cache_seg_index_word)
		);
	} else {
		asm volatile (
			/* write index for address to write */
			"MCR p15, 3, %1, c15, c0, 0\n"
			/* get TAG at given index */
			"MRC p15, 3, %0, c15, c2, 0\n"

			: "=r"(stored_tag_index)
			: "r"(cache_seg_index_word)
		);
	}

	/* now check if the TAG RAM content matches with what we expect and valid bit is set */
	uint32_t tag_index_valid_mask = CACHE_TAG_ADDRMASK(type) | CACHE_INDEX_ADDRMASK(type) | 0x10;
	uint32_t cache_tag_index = (address & tag_index_valid_mask) | 0x10;

	if((stored_tag_index & tag_index_valid_mask) == cache_tag_index) {
		return 1;
	}

	return 0;
}

static void icache_unlock(void)
{
	uint32_t old_int = cli();

	/* make sure all entries are set to invalid */
	uint32_t index;
	for(index = 0; index < (1U<<CACHE_INDEX_BITS(TYPE_ICACHE)); index++) {
		asm volatile (
			/* write index for address to write */
			"MOV R0, %0, LSL #5\n"
			"MCR p15, 3, R0, c15, c0, 0\n"
			/* set TAG at given index */
			"MCR p15, 3, R0, c15, c1, 0\n"

			:
			: "r"(index)
			: "r0"
		);
	}

	/* disable cache lockdown */
	asm volatile (
		"MOV R0, #0\n"
		"MCR p15, 0, R0, c9, c0, 1\n"

		:
		:
		: "r0"
	);

	/* and flush cache again to make sure its consistent */
	asm volatile (
		"MOV R0, #0\n"
		"MCR p15, 0, R0, c7, c5, 0\n"
		"MCR p15, 0, R0, c7, c10, 4\n"

		:
		:
		: "r0"
	);

	sei(old_int);
}

static void dcache_unlock(void)
{
	uint32_t old_int = cli();

	/* first clean and flush dcache entries */
	uint32_t segment, index;
	for(segment = 0; segment < (1U<<CACHE_SEGMENT_BITS(TYPE_DCACHE)); segment++ ) {
		for(index = 0; index < (1U<<CACHE_INDEX_BITS(TYPE_DCACHE)); index++) {
			uint32_t seg_index = (segment << CACHE_SEGMENT_TAGOFFSET(TYPE_DCACHE)) | (index << CACHE_INDEX_TAGOFFSET(TYPE_DCACHE));
			asm volatile (
				"MCR p15, 0, %0, c7, c14, 2\n"

				:
				: "r"(seg_index)
			);
		}
	}

	/* make sure all entries are set to invalid */
	for(index = 0; index < (1U<<CACHE_INDEX_BITS(TYPE_ICACHE)); index++) {
		asm volatile (
			/* write index for address to write */
			"MOV R0, %0, LSL #5\n"
			"MCR p15, 3, R0, c15, c0, 0\n"
			/* set TAG at given index */
			"MCR p15, 3, R0, c15, c2, 0\n"

			:
			: "r"(index)
			: "r0"
		);
	}

	/* disable cache lockdown */
	asm volatile (
		"MOV R0, #0\n"
		"MCR p15, 0, R0, c9, c0, 0\n"

		:
		:
		: "r0"
	);

	/* and flush cache again to make sure its consistent */
	asm volatile (
		"MOV R0, #0\n"
		"MCR p15, 0, R0, c7, c6, 0\n"
		"MCR p15, 0, R0, c7, c10, 4\n"

		:
		:
		: "r0"
	);

	sei(old_int);
}

static inline void icache_lock(void)
{
	uint32_t old_int = cli();

	/* no need to clean entries, directly flush and lock cache */
	asm volatile (
		/* flush cache pages */
		"MCR p15, 0, R0, c7, c5, 0\n"

		/* enable cache lockdown for segment 0 (of 4) */
		"MOV R0, #0x80000000\n"
		"MCR p15, 0, R0, c9, c0, 1\n"

		/* finalize lockdown */
		"MOV R0, #1\n"
		"MCR p15, 0, R0, c9, c0, 1\n"

		:
		:
		: "r0"
	);

	/* make sure all entries are set to invalid */
	uint32_t index;
	for(index = 0; index < (1U<<CACHE_INDEX_BITS(TYPE_ICACHE)); index++) {
		asm volatile (
			/* write index for address to write */
			"MOV R0, %0, LSL #5\n"
			"MCR p15, 3, R0, c15, c0, 0\n"
			/* set TAG at given index */
			"MCR p15, 3, R0, c15, c1, 0\n"

			:
			: "r"(index)
			: "r0"
		);
	}

	sei(old_int);
}

static void clean_d_cache(void);

static inline void dcache_lock(void)
{
	uint32_t old_int = cli();

	/* first clean and flush dcache entries */
	//uint32_t segment;
	uint32_t index;
	/*
	for(segment = 0; segment < (1U<<CACHE_SEGMENT_BITS(TYPE_DCACHE)); segment++ ) {
		for(index = 0; index < (1U<<CACHE_INDEX_BITS(TYPE_DCACHE)); index++) {
			uint32_t seg_index = (segment << CACHE_SEGMENT_TAGOFFSET(TYPE_DCACHE)) | (index << CACHE_INDEX_TAGOFFSET(TYPE_DCACHE));
			asm volatile (
				"MCR p15, 0, %0, c7, c14, 2\n"

				:
				: "r"(seg_index)
			);
		}
	}
	*/
	// XXX: 0xAF: I dont know what is wrong with the code above, but it hangs the camera
	// the following function clears D Cache w/o problems
	clean_d_cache();

	/* then lockdown data cache */
	asm volatile (
		/* enable cache lockdown for segment 0 (of 4) */
		"MOV R0, #0x80000000\n"
		"MCR p15, 0, R0, c9, c0, 0\n"

		/* finalize lockdown */
		"MOV R0, #1\n"
		"MCR p15, 0, R0, c9, c0, 0\n"

		:
		:
		: "r0"
	);

	/* make sure all entries are set to invalid */
	for(index = 0; index < (1U<<CACHE_INDEX_BITS(TYPE_DCACHE)); index++) {
		asm volatile (
			/* write index for address to write */
			"MOV R0, %0, LSL #5\n"
			"MCR p15, 3, R0, c15, c0, 0\n"
			/* set TAG at given index */
			"MCR p15, 3, R0, c15, c2, 0\n"

			:
			: "r"(index)
			: "r0"
		);
	}

	sei(old_int);
}

/* these are the "public" functions. please use only these if you are not sure what the others are for */

static uint32_t __attribute__ ((unused)) cache_locked(void)
{
	uint32_t status = 0;
	asm volatile (
		/* get lockdown status */
		"MRC p15, 0, %0, c9, c0, 1\n"

		: "=r"(status)
		:
		: "r0"
	);

	return status;
}

static inline void cache_lock(void)
{
	icache_lock();
	dcache_lock();
}

static void __attribute__ ((unused)) cache_unlock(void)
{
	icache_unlock();
	dcache_unlock();
}

static uint32_t cache_fake(uint32_t address, uint32_t data, uint32_t type)
{
	/* that word is already patched? return failure */
	/*
	if(!cache_is_patchable(address, type)) {
		return 0;
	}
	*/
	/* is that line not in cache yet? */
	if(!cache_get_cached(address, type)) {
		/* no, then fetch it */
		cache_fetch_line(address, type);
	}

	return cache_patch_single_word(address, data, type);
}

static inline void flush_caches( void ) {
	uint32_t reg = 0;
	asm(
		"MOV %0, #0\n"
		"MCR p15, 0, %0, c7, c5, 0\n" // entire I cache
		"MOV %0, #0\n"
		"MCR p15, 0, %0, c7, c6, 0\n" // entire D cache
		"MCR p15, 0, %0, c7, c10, 4\n" // drain write buffer

		:
		: "r"(reg)
	);
}

static inline void clean_d_cache( void ) {
	uint32_t segment = 0;
	do {
		uint32_t line = 0;
		for( ; line != 0x400 ; line += 0x20 ) {
			asm(
				"MCR p15, 0, %0, c7, c14, 2"

				:
				: "r"( line | segment )
			);
		}
	} while( segment += 0x40000000 );
}

#endif
