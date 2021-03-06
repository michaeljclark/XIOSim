#ifndef ZESTO_COMMIT_INCLUDED
#define ZESTO_COMMIT_INCLUDED

/* zesto-commit.h - Zesto commit stage class
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
 * 4. Zesto is distributed freely for commercial and non-commercial use.
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
 */

#include <memory>

class core_commit_t
{
  enum commit_stall_t {CSTALL_NONE,      /* no stall */
                       CSTALL_NOT_READY, /* oldest inst not done (no uops finished) */
                       CSTALL_PARTIAL,   /* oldest inst not done (but some uops finished) */
                       CSTALL_EMPTY,     /* ROB is empty, nothing to commit */
                       CSTALL_JECLEAR_INFLIGHT, /* Mop is done, but its jeclear hasn't been handled yet */
                       CSTALL_MAX_BRANCHES, /* exceeded maximum number of branches committed per cycle */
                       CSTALL_num
                     };

  public:

  core_commit_t(void);
  virtual ~core_commit_t();
  virtual void reg_stats(xiosim::stats::StatsDatabase* sdb) = 0;
  virtual void update_occupancy(void) = 0;

  virtual void step(void) = 0;
  virtual void IO_step(void) = 0;
  virtual void recover(const struct Mop_t * const Mop) = 0;
  virtual void recover(void) = 0;

  /* ROB management functions
     ROB_available() returns true if a ROB entry is available
     ROB_empty() returns true when there are no uops in the ROB
     ROB_insert() inserts a uop into the ROB
     ROB_fuse_insert() adds a fused uop body to the uop head
       (and any previously fuse_inserted uops) to an already
       allocated ROB entry (i.e., that alloc'd to the head) */
  virtual bool ROB_available(void) = 0;
  virtual bool ROB_empty(void) = 0;
  virtual bool pipe_empty(void) = 0;
  virtual void ROB_insert(struct uop_t * const uop) = 0;
  virtual void ROB_fuse_insert(struct uop_t * const uop) = 0;

  virtual bool pre_commit_available(void) = 0;
  virtual void pre_commit_insert(struct uop_t * const uop) = 0;
  virtual void pre_commit_fused_insert(struct uop_t * const uop) = 0;
  virtual void pre_commit_step(void) = 0;
  virtual void pre_commit_recover(struct Mop_t * const Mop) = 0;

  /* Squash a uop -- invalidate all actions and deallocate it from all exec buffers.
   * Also, clean up exec idep/odep pointers.
   * XXX: Different pipeline styles call this in different orders: DPM (young -> old),
   * IO_DPM (old -> young). */
  virtual void squash_uop(struct uop_t * const uop) = 0;

  bool deadlocked;
  static const int deadlock_threshold = 50000;

  protected:

  struct core_t * core;
};

std::unique_ptr<class core_commit_t> commit_create(const char * commit_opt_string, struct core_t * core);

#endif /* ZESTO_COMMIT_INCLUDED */
