#ifndef ZESTO_MC_INCLUDED
#define ZESTO_MC_INCLUDED

/* zesto-uncore.h - Zesto uncore wrapper class
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

#include "statistic.h"
#include "stat_database.h"
#include "zesto-cache.h"

/* memory controller */
class MC_t
{
 protected:

  struct MC_action_t {
    bool valid;
    void * op;
    seq_t action_id;
    md_paddr_t addr;
    int linesize;
    int MSHR_bank;
    int MSHR_index;
    enum cache_command cmd;
    struct cache_t * prev_cp;
    /* final call back on service-complete */
    void (*cb)(void * op); /* NULL = invalid/empty entry */
    /* extract action_id; varies depending on whether we're dealing with a uop or IFQ entry */
    seq_t (*get_action_id)(void *);
    tick_t when_enqueued; /* cycle this transaction arrived in the MC */
    tick_t when_started;  /* cycle the MC sent this transaction to the DRAM */
    tick_t when_finished; /* cycle the dram access completes */
    tick_t when_returned; /* cycle the MC returned this request to the CPU */
  };

  counter_t total_accesses;
  counter_t total_dram_cycles;
  counter_t total_service_cycles;

  void init(void);

  public:
  MC_t() {};
  virtual ~MC_t() {};
  virtual bool enqueuable(md_paddr_t addr = 0) = 0;
  virtual void enqueue(struct cache_t * const prev_cp,
                       const enum cache_command cmd,
                       const md_paddr_t addr,
                       const int linesize,
                       const seq_t action_id,
                       const int MSHR_bank,
                       const int MSHR_index,
                       void * const op,
                       void (*const cb)(void *),
                       seq_t (*const get_action_id)(void *) ) = 0;
  virtual void step(void) = 0;
  virtual void reg_stats(xiosim::stats::StatsDatabase* sdb);
  virtual void reset_stats(void);
  virtual void print(FILE * const fp) = 0;
};

std::unique_ptr<MC_t> MC_from_string(const char * const opt_string);

#ifndef MC_assert
#ifdef DEBUG
#define MC_assert(cond, retval) assert(cond)
#else
#define MC_assert(cond, retval) { \
  if(!(cond)) { \
    fprintf(stderr,"fatal (%s,%d:MC): ",__FILE__,__LINE__); \
    fprintf(stderr,"%s\n",#cond); \
    return (retval); \
  } \
}
#endif
#endif

#endif /* ZESTO_MC_INCLUDED */
