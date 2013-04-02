/* loader.c - program loader routines
 *
 * Copyright � 2009 by Gabriel H. Loh and the Georgia Tech Research Corporation
 * Atlanta, GA  30332-0415
 * All Rights Reserved.
 * 
 * THIS IS A LEGAL DOCUMENT BY DOWNLOADING ZESTO, YOU ARE AGREEING TO THESE
 * TERMS AND CONDITIONS.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * 
 * NOTE: Portions of this release are directly derived from the SimpleScalar
 * Toolset (property of SimpleScalar LLC), and as such, those portions are
 * bound by the corresponding legal terms and conditions.  All source files
 * derived directly or in part from the SimpleScalar Toolset bear the original
 * user agreement.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the Georgia Tech Research Corporation nor the names of
 * its contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 * 
 * 4. Zesto is distributed freely for commercial and non-commercial use.  Note,
 * however, that the portions derived from the SimpleScalar Toolset are bound
 * by the terms and agreements set forth by SimpleScalar, LLC.  In particular:
 * 
 *   "Nonprofit and noncommercial use is encouraged. SimpleScalar may be
 *   downloaded, compiled, executed, copied, and modified solely for nonprofit,
 *   educational, noncommercial research, and noncommercial scholarship
 *   purposes provided that this notice in its entirety accompanies all copies.
 *   Copies of the modified software can be delivered to persons who use it
 *   solely for nonprofit, educational, noncommercial research, and
 *   noncommercial scholarship purposes provided that this notice in its
 *   entirety accompanies all copies."
 * 
 * User is responsible for reading and adhering to the terms set forth by
 * SimpleScalar, LLC where appropriate.
 * 
 * 5. No nonprofit user may place any restrictions on the use of this software,
 * including as modified by the user, by any other authorized user.
 * 
 * 6. Noncommercial and nonprofit users may distribute copies of Zesto in
 * compiled or executable form as set forth in Section 2, provided that either:
 * (A) it is accompanied by the corresponding machine-readable source code, or
 * (B) it is accompanied by a written offer, with no time limit, to give anyone
 * a machine-readable copy of the corresponding source code in return for
 * reimbursement of the cost of distribution. This written offer must permit
 * verbatim duplication by anyone, or (C) it is distributed by someone who
 * received only the executable form, and is accompanied by a copy of the
 * written offer of source code.
 * 
 * 7. Zesto was developed by Gabriel H. Loh, Ph.D.  US Mail: 266 Ferst Drive,
 * Georgia Institute of Technology, Atlanta, GA 30332-0765
 * SimpleScalar � Tool Suite
 * � 1994-2003 Todd M. Austin, Ph.D. and SimpleScalar, LLC
 * All Rights Reserved.
 * 
 * THIS IS A LEGAL DOCUMENT BY DOWNLOADING SIMPLESCALAR, YOU ARE AGREEING TO
 * THESE TERMS AND CONDITIONS.
 * 
 * No portion of this work may be used by any commercial entity, or for any
 * commercial purpose, without the prior, written permission of SimpleScalar,
 * LLC (info@simplescalar.com). Nonprofit and noncommercial use is permitted as
 * described below.
 * 
 * 1. SimpleScalar is provided AS IS, with no warranty of any kind, express or
 * implied. The user of the program accepts full responsibility for the
 * application of the program and the use of any results.
 * 
 * 2. Nonprofit and noncommercial use is encouraged.  SimpleScalar may be
 * downloaded, compiled, executed, copied, and modified solely for nonprofit,
 * educational, noncommercial research, and noncommercial scholarship purposes
 * provided that this notice in its entirety accompanies all copies. Copies of
 * the modified software can be delivered to persons who use it solely for
 * nonprofit, educational, noncommercial research, and noncommercial
 * scholarship purposes provided that this notice in its entirety accompanies
 * all copies.
 * 
 * 3. ALL COMMERCIAL USE, AND ALL USE BY FOR PROFIT ENTITIES, IS EXPRESSLY
 * PROHIBITED WITHOUT A LICENSE FROM SIMPLESCALAR, LLC (info@simplescalar.com).
 * 
 * 4. No nonprofit user may place any restrictions on the use of this software,
 * including as modified by the user, by any other authorized user.
 * 
 * 5. Noncommercial and nonprofit users may distribute copies of SimpleScalar
 * in compiled or executable form as set forth in Section 2, provided that
 * either: (A) it is accompanied by the corresponding machine-readable source
 * code, or (B) it is accompanied by a written offer, with no time limit, to
 * give anyone a machine-readable copy of the corresponding source code in
 * return for reimbursement of the cost of distribution. This written offer
 * must permit verbatim duplication by anyone, or (C) it is distributed by
 * someone who received only the executable form, and is accompanied by a copy
 * of the written offer of source code.
 * 
 * 6. SimpleScalar was developed by Todd M. Austin, Ph.D. The tool suite is
 * currently maintained by SimpleScalar LLC (info@simplescalar.com). US Mail:
 * 2395 Timbercrest Court, Ann Arbor, MI 48105.
 * 
 * Copyright � 1994-2003 by Todd M. Austin, Ph.D. and SimpleScalar, LLC.
 */

#include "loader.h"

  /* register simulator-specific statistics */
  void
ld_reg_stats(struct thread_t * thread, struct stat_sdb_t *sdb)	/* stats data base */
{
  char buf[128];

  stat_reg_note(sdb,"\n#### PROGRAM/TEXT STATS ####");
  sprintf(buf,"c%d.loader.text_base",thread->id);
  stat_reg_addr(sdb, TRUE, buf, "program text (code) segment base", &thread->loader.text_base, thread->loader.text_base, FALSE, "  0x%08p");
  sprintf(buf,"c%d.loader.text_bound",thread->id);
  stat_reg_addr(sdb, TRUE, buf, "program text (code) segment bound", &thread->loader.text_bound, thread->loader.text_bound, FALSE, "  0x%08p");
  sprintf(buf,"c%d.loader.text_size",thread->id);
  stat_reg_uint(sdb, TRUE, buf, "program text (code) size in bytes", &thread->loader.text_size, thread->loader.text_size, FALSE, NULL);
  sprintf(buf,"c%d.loader.data_base",thread->id);
  stat_reg_addr(sdb, TRUE, buf, "program initialized data segment base", &thread->loader.data_base, thread->loader.data_base, FALSE, "  0x%08p");
  sprintf(buf,"c%d.loader.data_bound",thread->id);
  stat_reg_addr(sdb, TRUE, buf, "program initialized data segment bound", &thread->loader.data_bound, thread->loader.data_bound, FALSE, "  0x%08p");
  sprintf(buf,"c%d.loader.data_size",thread->id);
  stat_reg_uint(sdb, TRUE, buf, "program init'ed `.data' and uninit'ed `.bss' size in bytes", &thread->loader.data_size, thread->loader.data_size, FALSE, NULL);
  sprintf(buf,"c%d.loader.stack_base",thread->id);
  stat_reg_addr(sdb, TRUE, buf, "program stack segment base (highest address in stack)", &thread->loader.stack_base, thread->loader.stack_base, FALSE, "  0x%08p");
#if 0 /* FIXME: broken... */
  stat_reg_addr(sdb, TRUE, "ld_stack_min", "program stack segment lowest address", &ld_stack_min, ld_stack_min, "0x%08p");
#endif
  sprintf(buf,"c%d.loader.stack_size",thread->id);
  stat_reg_uint(sdb, TRUE, buf, "program initial stack size", &thread->loader.stack_size, thread->loader.stack_size, FALSE, NULL);
  sprintf(buf,"c%d.loader.prog_entry",thread->id);
  stat_reg_addr(sdb, TRUE, buf, "program entry point (initial PC)", &thread->loader.prog_entry, thread->loader.prog_entry, FALSE, "  0x%08p");
  sprintf(buf,"c%d.loader.environ_base",thread->id);
  stat_reg_addr(sdb, TRUE, buf, "program environment base address address", &thread->loader.environ_base, thread->loader.environ_base, FALSE, "  0x%08p");
  sprintf(buf,"c%d.loader.target_big_endian",thread->id);
  stat_reg_int(sdb, TRUE, buf, "target executable endian-ness, non-zero if big endian", &thread->loader.target_big_endian, thread->loader.target_big_endian, FALSE, NULL);
}
