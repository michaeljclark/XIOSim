#ifndef __FEEDER_ZESTO__
#define __FEEDER_ZESTO__

/*
 * Molecool: Feeder to Zesto, fed itself by ILDJIT.
 * Copyright, Vijay Reddi, 2007 -- SimpleScalar feeder prototype
              Svilen Kanev, 2011
*/

#include "pin.H"
#include "instlib.H"
using namespace INSTLIB;

#include "../interface.h"
#include "../synchronization.h"

class handshake_container_t;
class BufferManager;

extern BufferManager handshake_buffer;
extern KNOB<BOOL> KnobILDJIT;
extern KNOB<string> KnobFluffy;
extern list<THREADID> thread_list;
extern XIOSIM_LOCK thread_list_lock;

#define ATOMIC_ITERATE(_list, _it, _lock) \
    for (lk_lock(&(_lock), 1), (_it) = (_list).begin(), lk_unlock(&(_lock)); \
         [&]{lk_lock(&(_lock), 1); bool res = (_it) != (_list).end(); lk_unlock(&(_lock)); return res;}();  \
         lk_lock(&(_lock), 1), (_it)++, lk_unlock(&(_lock)))

/* ========================================================================== */
/* Thread-local state for instrument threads that we need to preserve between
 * different instrumentation calls */
class thread_state_t
{
  public:
    thread_state_t(THREADID instrument_tid) {
        memzero(&fpstate_buf, sizeof(FPSTATE));

        last_syscall_number = last_syscall_arg1 = 0;
        last_syscall_arg2 = last_syscall_arg3 = 0;
        bos = -1;
        coreID = -1;
        firstIteration = false;
        lastSignalAddr = 0xdecafbad;

        memset(pc_queue, 0, PC_QUEUE_SIZE*sizeof(INT32));
        pc_queue_head = PC_QUEUE_SIZE-1;
        pc_queue_valid = false;

        ignore = true;
        ignore_all = true;

        unmatchedWaits = 0;

        num_inst = 0;

        sleep_producer = false;

        lk_init(&lock);
    }

    // Buffer to store the fpstate that the simulator may corrupt
    FPSTATE fpstate_buf;

    // Used by syscall capture code
    ADDRINT last_syscall_number;
    ADDRINT last_syscall_arg1;
    ADDRINT last_syscall_arg2;
    ADDRINT last_syscall_arg3;

    // Bottom-of-stack pointer used for shadow page table
    ADDRINT bos;

    // Which simulated core this thread runs on
    ADDRINT coreID;

    // How many instructions have been produced
    UINT64 num_inst;

    // Have we executed a wait on this thread
    BOOL firstIteration;

    // Address of the last signal executed
    ADDRINT lastSignalAddr;

    // Handling a buffer of the last PC_QUEUE_SIZE instruction pointers
    ADDRINT get_queued_pc(INT32 index) {
        return pc_queue[(pc_queue_head + index) & (PC_QUEUE_SIZE - 1)];
    }
    VOID queue_pc(ADDRINT pc) {
        pc_queue_head = (pc_queue_head - 1) & (PC_QUEUE_SIZE - 1);
        pc_queue[pc_queue_head] = pc;
        pc_queue_valid = true;
    }
    BOOL pc_queue_valid;

    XIOSIM_LOCK lock;
    // XXX: SHARED -- lock protects those
    // Is thread not instrumenting instructions ?
    BOOL ignore;
    // Similar effect to above, but produced differently for sequential code
    BOOL ignore_all;
    // Stores the ID of the wait between before and afterWait. -1 outside.
    INT32 lastWaitID;
    // flag on whether to sleep the producer buffer
    BOOL sleep_producer;

    //Ignore list of instrutcions that we don't care about
    map<ADDRINT, BOOL> ignore_list;
    // XXX: END SHARED

    INT32 unmatchedWaits;
private:
    // XXX: power of 2
    static const INT32 PC_QUEUE_SIZE = 4;
    // Latest several pc-s instrumented
    ADDRINT pc_queue[PC_QUEUE_SIZE];
    INT32 pc_queue_head;
};
thread_state_t* get_tls(THREADID tid);

/* ========================================================================== */
class sim_thread_state_t {
public:
    sim_thread_state_t() {

        is_running = true;
        // Will get cleared on first simulated instruction
        sim_stopped = true;

        lk_init(&lock);
    }

    XIOSIM_LOCK lock;
    // Signal to the simulator thread to die
    BOOL is_running;
    // Set by simulator thread once it dies
    BOOL sim_stopped;
};
sim_thread_state_t* get_sim_tls(THREADID tid);

/* ========================================================================== */
/* Execution mode allows easy querying of exactly what the pin tool is doing at
 * a given time, and also helps ensuring that certain parts of the code are run
 * in only certain modes. */
enum EXECUTION_MODE
{
    EXECUTION_MODE_FASTFORWARD,
    EXECUTION_MODE_SIMULATE,
    EXECUTION_MODE_INVALID
};
extern EXECUTION_MODE ExecMode;

struct handshake_flags_t
{
  // Did we finish dumping context
  BOOL valid;

  BOOL isFirstInsn;
  BOOL isLastInsn;
  BOOL giveCoreUp;
  BOOL killThread;
};

class handshake_container_t
{
  public:
    handshake_container_t() {
        Clear();
    }

    void Clear() {
        memzero(&handshake, sizeof(P2Z_HANDSHAKE));
        handshake.real = true;
        flags.valid = false;
        flags.isFirstInsn = false;
        flags.isLastInsn = false;
        flags.giveCoreUp = false;
        flags.killThread = false;

        mem_buffer.clear();
    }

    void CopyTo(handshake_container_t* dest) const {
        dest->flags = this->flags;
        memcpy(&dest->handshake, &this->handshake, sizeof(P2Z_HANDSHAKE));
        dest->mem_buffer = this->mem_buffer;
    }

    // Handshake information that gets passed on to Zesto
    struct P2Z_HANDSHAKE handshake;

    struct handshake_flags_t flags;

    // Memory reads and writes to be passed on to Zesto
    std::map<UINT32, UINT8> mem_buffer;
};

VOID PPointHandler(CONTROL_EVENT ev, VOID * v, CONTEXT * ctxt, VOID * ip, THREADID tid);
VOID PauseSimulation(THREADID tid);
VOID StopSimulation(BOOL kill_sim_threads);
VOID SimulatorLoop(VOID* arg);
VOID ThreadFini(THREADID threadIndex, const CONTEXT *ctxt, INT32 code, VOID *v);
VOID Fini(INT32 exitCode, VOID *v);

VOID amd_hack();
VOID doLateILDJITInstrumentation();

VOID disable_consumers();
VOID enable_consumers();
VOID disable_producers();
VOID enable_producers();

#endif /*__FEEDER_ZESTO__ */
