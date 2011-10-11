/* 
 * Fluffy - looking for regions of interest when running under ILDJIT.
 * Copyright, Svilen Kanev, 2011
 */

#include "feeder.h"

#include <vector>

//XXX: This is single-threaded for now. Move to TLS for multithreading.

std::vector<ADDRINT> start_counts;
std::vector<ADDRINT> stop_counts;

std::vector<ADDRINT> curr_start_counts;
std::vector<ADDRINT> curr_stop_counts;

std::vector<ADDRINT> slice_weights_times_1000;

/* ========================================================================== */
VOID FLUFFY_Init()
{
    if (KnobFluffy.Value().empty())
        return;

    ifstream annotation_file(KnobFluffy.Value().c_str());

    ADDRINT start, end, id;
    std::string command;
    char ch;
    while (true)
    {
        annotation_file >> id >> command;
        if (annotation_file.eof())
            break;
        if (command == "Start")
        {
            do { annotation_file >> ch; } while (ch != ':');
            annotation_file >> start;
            start_counts.push_back(start);
        }
        else if (command == "End")
        {
            do { annotation_file >> ch; } while (ch != ':');
            annotation_file >> end;
            stop_counts.push_back(end);
        }
        else
            ASSERTX(false);
    }
    ASSERTX(start_counts.size() == stop_counts.size());
    ADDRINT npoints = start_counts.size();

//XXX: Change protocol so we don't need to do that and this is in annotation file
    ifstream weight_file("weights");
    ASSERTX(!weight_file.fail());
    double weight;
    int jnk;
    for(ADDRINT i=0; i<npoints; i++) {
        weight_file >> weight >> jnk;
        slice_weights_times_1000.push_back((ADDRINT)(weight*1000*100));
    }

    for(ADDRINT i=0; i<npoints; i++) {
#ifdef ZESTO_PIN_DBG
        cerr << start_counts [i] << " " << stop_counts[i] << endl;
#endif
        curr_start_counts.push_back(0);
        curr_stop_counts.push_back(0);
    }
}

/* ========================================================================== */
VOID FLUFFY_StartInsn(THREADID tid, ADDRINT pc, ADDRINT phase)
{
    if ((curr_start_counts[phase]++) == start_counts[phase]) {
        PPointHandler(CONTROL_START, NULL, NULL, (VOID*)pc, tid);
    }
}

/* ========================================================================== */
VOID FLUFFY_StopInsn(THREADID tid, ADDRINT pc, ADDRINT phase)
{
    if ((curr_stop_counts[phase]++) == stop_counts[phase]) {
        PPointHandler(CONTROL_STOP, NULL, NULL, (VOID*)pc, tid);

        thread_state_t* tstate = get_tls(tid);
        tstate->slice_num++;
        tstate->slice_length = 0; //XXX: fix icount failing with all ildjit threads
        tstate->slice_weight_times_1000 = slice_weights_times_1000[phase];
    }
}
