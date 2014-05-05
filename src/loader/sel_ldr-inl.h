/*
 * Copyright (c) 2011 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
/*
 * Copyright (c) 2012, LiteStack, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * DO NOT INCLUDE EXCEPT FROM sel_ldr.h
 * THERE CANNOT BE ANY MULTIPLE INCLUSION GUARDS IN ORDER FOR
 * sel_ldr-inl.c TO WORK.
 */

/*
 * Routines to translate addresses between user and "system" or
 * service runtime addresses.  the *Addr* versions will return
 * kNaClBadAddress if the user address is outside of the user address
 * space, e.g., if the input addresses for *UserToSys* is outside of
 * (1 << ADDR_BITS), and correspondingly for *SysToUser* if the
 * input system address does not correspond to a user address.
 * Generally, the *Addr* versions are used when the addresses come
 * from untrusted usre code, and kNaClBadAddress would translate to an
 * EINVAL return from a syscall.  The *Range code ensures that the
 * entire address range is in the user address space.
 *
 * Note that just because an address is within the address space, it
 * doesn't mean that it is safe to acceess the memory: the page may be
 * protected against access.
 *
 * The non-*Addr* versions abort the program rather than return an
 * error indication.
 *
 * 0 is not a good error indicator, since 0 is a valid user address
 */

#include "src/main/zlog.h"
#include "src/loader/userspace.h"

/* d'b: no checks, just does the work */
static INLINE uintptr_t NaClUserToSysAddrNullOkay(uintptr_t uaddr)
{
  return uaddr + MEM_START;
}

static INLINE uintptr_t NaClUserToSys(uintptr_t uaddr)
{
  ZLOGFAIL(0 == uaddr || ((uintptr_t) 1U << ADDR_BITS) <= uaddr, EFAULT,
      "uaddr 0x%08lx, addr space %d bits", uaddr, ADDR_BITS);
  return uaddr + MEM_START;
}

static INLINE uintptr_t NaClSysToUser(uintptr_t sysaddr)
{
  ZLOGFAIL(MEM_START + ((uintptr_t) 1U << ADDR_BITS) <= sysaddr
      || sysaddr < MEM_START, EFAULT,
      "sysaddr 0x%08lx, mem_start 0x%08lx, addr space %d bits",
      sysaddr, MEM_START, ADDR_BITS);
  return sysaddr - MEM_START;
}

static INLINE uintptr_t NaClSandboxCodeAddr(uintptr_t addr)
{
  return (((addr & ~(((uintptr_t)NACL_INSTR_BLOCK_SIZE) - 1))
           & ((((uintptr_t) 1) << 32) - 1)) + MEM_START);
}
