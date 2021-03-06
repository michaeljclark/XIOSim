/* Implementation of temporary replacement statistics library layer. */

#include "core_const.h"
#include "expression.h"
#include "statistic.h"
#include "stat_database.h"

#include "stats.h"

using namespace xiosim::stats;

const size_t STAT_NAME_MAX_LEN = 64;
const size_t DESC_MAX_LEN = 128;
const char* core_stat_fmt = "c%d.%s";     // c[num].[stat_name]
const char* comp_stat_fmt = "c%d.%s.%s";  // c[num].[comp_name].[stat_name]
const char* llc_stat_fmt = "%s.c%d.%s";   // [cache_name].c[num].[stat_name]
const char* nocore_comp_stat_fmt = "%s.%s";  // [comp_name].[stat_name]

//**************************************************//
//                    Helpers                       //
// *************************************************//

void create_comp_stat_name(int core_id,
                           const char* comp_name,
                           const char* stat_name,
                           char* full_name,
                           bool is_llc) {
    if (core_id == xiosim::INVALID_CORE)
        snprintf(full_name, STAT_NAME_MAX_LEN, nocore_comp_stat_fmt, comp_name, stat_name);
    else if (is_llc)
        snprintf(full_name, STAT_NAME_MAX_LEN, llc_stat_fmt, comp_name, core_id, stat_name);
    else
        snprintf(full_name, STAT_NAME_MAX_LEN, comp_stat_fmt, core_id, comp_name, stat_name);
}

//**************************************************//
//                    Scalars                       //
// *************************************************//

// These all return references so they can be directly used in an expression
// without an additional deference.

template <typename T>
Statistic<T>& reg_stat_helper(StatsDatabase* sdb,
                              int print_me,
                              const char* name,
                              const char* desc,
                              T* var,
                              T init_val,
                              int scale_me,
                              const char* format) {
    Statistic<T>* stat = sdb->add_statistic(name, desc, var, init_val);
    if (format)
        stat->set_output_fmt(format);
    if (!print_me)
        stat->set_printed(false);
    if (!scale_me)
        stat->set_scaled(false);
    return *stat;
}

Statistic<int>& stat_reg_int(StatsDatabase* sdb,
                             int print_me,
                             const char* name,
                             const char* desc,
                             int* var,
                             int init_val,
                             int scale_me,
                             const char* format) {
    return reg_stat_helper<int>(sdb, print_me, name, desc, var, init_val, scale_me, format);
}

Statistic<int>& stat_reg_core_int(StatsDatabase* sdb,
                                  int print_me,
                                  int core_id,
                                  const char* name,
                                  const char* desc,
                                  int* var,
                                  int init_val,
                                  int scale_me,
                                  const char* format) {
    char full_stat_name[STAT_NAME_MAX_LEN];
    snprintf(full_stat_name, STAT_NAME_MAX_LEN, core_stat_fmt, core_id, name);
    return stat_reg_int(sdb, print_me, name, desc, var, init_val, scale_me, format);
}

/* Integer stat of a component that belongs to a core or is associated with a core.
 *
 * Possible components are caches (both private caches and shared caches) and
 * predictors.
 *
 * The statistic name is formatted as c[num].[comp_name].[stat_name].
 */
Statistic<int>& stat_reg_comp_int(StatsDatabase* sdb,
                                  int print_me,
                                  int core_id,
                                  const char* comp_name,
                                  const char* stat_name,
                                  const char* desc,
                                  int* var,
                                  int init_val,
                                  int scale_me,
                                  const char* format) {
    char full_stat_name[STAT_NAME_MAX_LEN];
    char full_desc[DESC_MAX_LEN];
    create_comp_stat_name(core_id, comp_name, stat_name, full_stat_name);
    snprintf(full_desc, DESC_MAX_LEN, desc, comp_name);
    return stat_reg_int(sdb, print_me, full_stat_name, full_desc, var, init_val, scale_me, format);
}

Statistic<unsigned int>& stat_reg_uint(StatsDatabase* sdb,
                                       int print_me,
                                       const char* name,
                                       const char* desc,
                                       unsigned int* var,
                                       unsigned int init_val,
                                       int scale_me,
                                       const char* format) {
    return reg_stat_helper<unsigned int>(
            sdb, print_me, name, desc, var, init_val, scale_me, format);
}

Statistic<uint64_t>& stat_reg_qword(StatsDatabase* sdb,
                                   int print_me,
                                   const char* name,
                                   const char* desc,
                                   uint64_t* var,
                                   uint64_t init_val,
                                   int scale_me,
                                   const char* format) {
    return reg_stat_helper<uint64_t>(sdb, print_me, name, desc, var, init_val, scale_me, format);
}

Statistic<uint64_t>& stat_reg_core_qword(StatsDatabase* sdb,
                                   int print_me,
                                   int core_id,
                                   const char* name,
                                   const char* desc,
                                   uint64_t* var,
                                   uint64_t init_val,
                                   int scale_me,
                                   const char* format) {
    char full_stat_name[STAT_NAME_MAX_LEN];
    snprintf(full_stat_name, STAT_NAME_MAX_LEN, core_stat_fmt, core_id, name);
    return stat_reg_qword(sdb, print_me, full_stat_name, desc, var, init_val, scale_me, format);
}

Statistic<int64_t>& stat_reg_sqword(StatsDatabase* sdb,
                                     int print_me,
                                     const char* name,
                                     const char* desc,
                                     int64_t* var,
                                     int64_t init_val,
                                     int scale_me,
                                     const char* format) {
    return reg_stat_helper<int64_t>(sdb, print_me, name, desc, var, init_val, scale_me, format);
}

Statistic<int64_t>& stat_reg_core_sqword(StatsDatabase* sdb,
                                          int print_me,
                                          int core_id,
                                          const char* name,
                                          const char* desc,
                                          int64_t* var,
                                          int64_t init_val,
                                          int scale_me,
                                          const char* format) {
    char full_stat_name[STAT_NAME_MAX_LEN];
    snprintf(full_stat_name, STAT_NAME_MAX_LEN, core_stat_fmt, core_id, name);
    return stat_reg_sqword(sdb, print_me, full_stat_name, desc, var, init_val, scale_me, format);
}

/* sqword stats of a component that belongs to a core or is associated with a core.
 *
 * Possible components are caches (both private caches and shared caches) and
 * predictors.
 *
 * For non last-level caches and predictors:
 *    Formatted stat name = c[core_id].[comp_name].[stat_name].
 *    Formatted description = @desc, where @desc is assumed to contain a single
 *        string formatting replacement character (e.g. "num misses in %s.")
 *        where @comp_name (the name of the cache or predictor) will be
 *        substituted.
 *
 * For the LLC, is_llc should be set to true.
 *    Formatted stat name = [comp_name].c[core_id].[stat_name]
 *    Formatted description = @desc, where @desc is assumed to contain an
 *        integer formatting replacement character AND a string replacement
 *        character (e.g. "num misses for core %d in cache %s.") where @core_id
 *        and @comp_name will be substituted, respectively.
 *
 * The difference in behavior is because LLCs do not belong to a core, so they
 * should not be labeled as a subcategory of one.
 */
Statistic<int64_t>& stat_reg_comp_sqword(StatsDatabase* sdb,
                                           int print_me,
                                           int core_id,
                                           const char* comp_name,
                                           const char* stat_name,
                                           const char* desc,
                                           int64_t* var,
                                           int64_t init_val,
                                           int scale_me,
                                           const char* format,
                                           bool is_llc) {
    char full_stat_name[STAT_NAME_MAX_LEN];
    char full_desc[DESC_MAX_LEN];
    if (is_llc) {
        snprintf(full_stat_name, STAT_NAME_MAX_LEN, llc_stat_fmt, comp_name, core_id, stat_name);
        snprintf(full_desc, DESC_MAX_LEN, desc, core_id, comp_name);
    } else {
        create_comp_stat_name(core_id, comp_name, stat_name, full_stat_name, is_llc);
        snprintf(full_desc, DESC_MAX_LEN, desc, comp_name);
    }
    return stat_reg_sqword(sdb, print_me, full_stat_name, full_desc, var, init_val, scale_me,
                           format);
}

Statistic<float>& stat_reg_float(StatsDatabase* sdb,
                                 int print_me,
                                 const char* name,
                                 const char* desc,
                                 float* var,
                                 float init_val,
                                 int scale_me,
                                 const char* format) {
    return reg_stat_helper<float>(sdb, print_me, name, desc, var, init_val, scale_me, format);
}

Statistic<double>& stat_reg_double(StatsDatabase* sdb,
                                   int print_me,
                                   const char* name,
                                   const char* desc,
                                   double* var,
                                   double init_val,
                                   int scale_me,
                                   const char* format) {
    return reg_stat_helper<double>(sdb, print_me, name, desc, var, init_val, scale_me, format);
}

Statistic<double>& stat_reg_comp_double(StatsDatabase* sdb,
                                        int print_me,
                                        int core_id,
                                        const char* comp_name,
                                        const char* stat_name,
                                        const char* desc,
                                        double* var,
                                        double init_val,
                                        int scale_me,
                                        const char* format) {
    char full_stat_name[STAT_NAME_MAX_LEN];
    char full_desc[DESC_MAX_LEN];
    create_comp_stat_name(core_id, comp_name, stat_name, full_stat_name);
    snprintf(full_desc, DESC_MAX_LEN, desc, comp_name);
    return stat_reg_double(
            sdb, print_me, full_stat_name, full_desc, var, init_val, scale_me, format);
}

// Distributions

Distribution* stat_reg_dist(StatsDatabase* sdb,
                            const char* name,
                            const char* desc,
                            unsigned int init_val,
                            unsigned int arr_sz,
                            int pf, /* print format, use PF_* defs */
                            const char* format,
                            const char** imap,
                            int scale_me,
                            void* print_fn) /* optional user print function */ {
    auto dist = sdb->add_distribution(name, desc, init_val, arr_sz, imap);
    if (format)
        dist->set_output_fmt(format);
    if (!scale_me)
        dist->set_scaled(false);

    /* Currently not supported: print_fn_t and pf flags. */
    return dist;
}

Distribution* stat_reg_core_dist(StatsDatabase* sdb,
                                 int core_id,
                                 const char* name,
                                 const char* desc,
                                 unsigned int init_val,
                                 unsigned int arr_sz,
                                 int pf, /* print format, use PF_* defs */
                                 const char* format,
                                 const char** imap,
                                 int scale_me,
                                 void* print_fn) /* optional user print function */ {
    char full_stat_name[STAT_NAME_MAX_LEN];
    snprintf(full_stat_name, STAT_NAME_MAX_LEN, core_stat_fmt, core_id, name);
    return stat_reg_dist(sdb, full_stat_name, desc, init_val, arr_sz, pf,
                         format, imap, scale_me, print_fn);
}

SparseHistogram* stat_reg_sparse_hist(StatsDatabase* sdb,
                                      const char* name,
                                      const char* desc,
                                      const char* label_fmt,
                                      const char* output_fmt,
                                      bool print,
                                      bool scale) {
    auto hist = sdb->add_sparse_histogram(
            name, desc, label_fmt, output_fmt, print, scale);
    return hist;
}

SparseHistogram* stat_reg_core_sparse_hist(StatsDatabase* sdb,
                                           int core_id,
                                           const char* name,
                                           const char* desc,
                                           const char* label_fmt,
                                           const char* output_fmt,
                                           bool print,
                                           bool scale) {
    char full_stat_name[STAT_NAME_MAX_LEN];
    snprintf(full_stat_name, STAT_NAME_MAX_LEN, core_stat_fmt, core_id, name);
    return stat_reg_sparse_hist(
            sdb, full_stat_name, desc, label_fmt, output_fmt, print, scale);
}

// Register a histogram belonging to or associated with a core.
// For more details, see stat_reg_comp_sqword.
SparseHistogram* stat_reg_comp_sparse_hist(StatsDatabase* sdb,
                                           int core_id,
                                           const char* comp_name,
                                           const char* stat_name,
                                           const char* desc,
                                           const char* label_fmt,
                                           const char* output_fmt,
                                           bool print,
                                           bool scale,
                                           bool is_llc) {
    char full_stat_name[STAT_NAME_MAX_LEN];
    char full_desc[DESC_MAX_LEN];
    create_comp_stat_name(core_id, comp_name, stat_name, full_stat_name, is_llc);
    snprintf(full_desc, DESC_MAX_LEN, desc, comp_name);
    return stat_reg_sparse_hist(
            sdb, full_stat_name, full_desc, label_fmt, output_fmt, print, scale);
}

void reg_core_queue_occupancy_stats(StatsDatabase* sdb,
                                    int core_id,
                                    std::string queue_name,
                                    counter_t* occupancy,
                                    counter_t* cycles_empty,
                                    counter_t* cycles_full) {
    auto sim_cycle_st = stat_find_core_stat<tick_t>(sdb, core_id, "sim_cycle");

    std::string occupancy_st_name = queue_name + "_occupancy";
    std::string empty_st_name = queue_name + "_empty";
    std::string full_st_name = queue_name + "_full";
    std::string avg_st_name = queue_name + "_avg";
    std::string frac_empty_st_name = queue_name + "_frac_empty";
    std::string frac_full_st_name = queue_name + "_frac_full";

    std::string occupancy_st_desc = "total " + queue_name + "_occupancy";
    std::string empty_st_desc = "total cycles " + queue_name + " was empty";
    std::string full_st_desc = "total cycles " + queue_name + " was full";
    std::string avg_st_desc = "average " + queue_name + " occupancy";
    std::string frac_empty_st_desc = "fraction of cycles " + queue_name + " was empty";
    std::string frac_full_st_desc = "fraction of cycles " + queue_name + " was full";

    auto& occupancy_st = stat_reg_core_counter(
            sdb, true, core_id, occupancy_st_name.c_str(),
            occupancy_st_desc.c_str(), occupancy, 0, true, NULL);
    stat_reg_core_formula(
            sdb, true, core_id, avg_st_name.c_str(),
            avg_st_desc.c_str(), occupancy_st / *sim_cycle_st, NULL);
    if (cycles_empty) {
        auto& empty_st = stat_reg_core_counter(
                sdb, true, core_id, empty_st_name.c_str(),
                empty_st_desc.c_str(), cycles_empty, 0, true, NULL);
        stat_reg_core_formula(
                sdb, true, core_id, frac_empty_st_name.c_str(),
                frac_empty_st_desc.c_str(), empty_st / *sim_cycle_st, NULL);
    }
    if (cycles_full) {
        auto& full_st = stat_reg_core_counter(
                sdb, true, core_id, full_st_name.c_str(),
                full_st_desc.c_str(), cycles_full, 0, true, NULL);
        stat_reg_core_formula(
                sdb, true, core_id, frac_full_st_name.c_str(),
                frac_full_st_desc.c_str(), full_st / *sim_cycle_st, NULL);
    }
}

/* Add a single sample to array or sparse array distribution STAT */
void stat_add_sample(BaseStatistic* stat, unsigned int index) {
    Distribution* dist = static_cast<Distribution*>(stat);
    dist->add_samples(index, 1);
}

//**************************************************//
//                   Formulas                       //
// *************************************************//

Formula* stat_reg_formula(StatsDatabase* sdb,
                          int print_me,
                          const char* name,
                          const char* desc,
                          const Expression& expression,
                          const char* format) {
    Formula* formula = sdb->add_formula(name, desc, expression);
    if (format)
        formula->set_output_fmt(format);
    if (!print_me)
        formula->set_printed(false);
    return formula;
}

Formula* stat_reg_formula(StatsDatabase* sdb, Formula& formula) {
    return sdb->add_formula(formula);
}

Formula* stat_reg_core_formula(StatsDatabase* sdb,
                               int print_me,
                               int core_id,
                               const char* name,
                               const char* desc,
                               const Expression& expression,
                               const char* format) {
    char full_stat_name[STAT_NAME_MAX_LEN];
    snprintf(full_stat_name, STAT_NAME_MAX_LEN, core_stat_fmt, core_id, name);

    Formula* formula = sdb->add_formula(full_stat_name, desc, expression);
    if (format)
        formula->set_output_fmt(format);
    return formula;
}

Formula* stat_reg_comp_formula(StatsDatabase* sdb,
                                int print_me,
                                int core_id,
                                const char* comp_name,
                                const char* stat_name,
                                const char* desc,
                                const Expression& expression,
                                const char* format,
                                bool is_llc) {
    char full_stat_name[STAT_NAME_MAX_LEN];
    char full_desc[DESC_MAX_LEN];
    create_comp_stat_name(core_id, comp_name, stat_name, full_stat_name, is_llc);
    if (is_llc)
        snprintf(full_desc, DESC_MAX_LEN, desc, core_id, comp_name);
    else
        snprintf(full_desc, DESC_MAX_LEN, desc, comp_name);

    Formula* formula = sdb->add_formula(full_stat_name, full_desc, expression);
    if (format)
        formula->set_output_fmt(format);
    return formula;
}

//**************************************************//
//                    Strings                       //
// *************************************************//

Statistic<const char*>& stat_reg_string(StatsDatabase* sdb,
                                                       const char* name,
                                                       const char* desc,
                                                       const char* var,
                                                       const char* format) {

    Statistic<const char*>* stat = sdb->add_statistic(name, desc, var, format);
    return *stat;
}

Statistic<const char*>& stat_reg_comp_string(StatsDatabase* sdb,
                                             int core_id,
                                             const char* comp_name,
                                             const char* stat_name,
                                             const char* desc,
                                             const char* var,
                                             const char* format) {
    char full_stat_name[STAT_NAME_MAX_LEN];
    char full_desc[DESC_MAX_LEN];
    create_comp_stat_name(core_id, comp_name, stat_name, full_stat_name);
    snprintf(full_desc, DESC_MAX_LEN, desc, comp_name);
    Statistic<const char*>* stat = sdb->add_statistic(full_stat_name, full_desc, var, format);
    return *stat;
}

Statistic<const char*>& stat_reg_note(StatsDatabase* sdb, const char* note) {
    Statistic<const char*>* stat = sdb->add_statistic("", "", note);
    return *stat;
}

//**************************************************//
//                   Utilities                      //
// *************************************************//

StatsDatabase* stat_new() { return new StatsDatabase(); }

void stat_print_stats(StatsDatabase* sdb, FILE* fd) { sdb->print_all_stats(fd); }

void stat_print_stat(BaseStatistic* stat, FILE* fd) { stat->print_value(fd); }

Distribution* stat_find_dist(StatsDatabase* sdb, const char* stat_name) {
    return static_cast<Distribution*>(sdb->get_statistic(stat_name));
}

Formula* stat_find_core_formula(StatsDatabase* sdb, int core_id, const char* stat_name) {
    char full_stat_name[STAT_NAME_MAX_LEN];
    snprintf(full_stat_name, STAT_NAME_MAX_LEN, core_stat_fmt, core_id, stat_name);
    return static_cast<Formula*>(sdb->get_statistic(full_stat_name));
}

Distribution* stat_find_core_dist(StatsDatabase* sdb, int core_id, const char* stat_name) {
    char full_stat_name[STAT_NAME_MAX_LEN];
    snprintf(full_stat_name, STAT_NAME_MAX_LEN, core_stat_fmt, core_id, stat_name);
    return static_cast<Distribution*>(sdb->get_statistic(full_stat_name));
}

SparseHistogram* stat_find_sparse_hist(StatsDatabase* sdb, const char* stat_name) {
    return static_cast<SparseHistogram*>(sdb->get_statistic(stat_name));
}

SparseHistogram* stat_find_core_sparse_hist(
        StatsDatabase* sdb, int core_id, const char* stat_name) {
    char full_stat_name[STAT_NAME_MAX_LEN];
    snprintf(full_stat_name, STAT_NAME_MAX_LEN, core_stat_fmt, core_id, stat_name);
    return static_cast<SparseHistogram*>(sdb->get_statistic(full_stat_name));
}

void stat_save_stats(StatsDatabase* sdb) { sdb->save_stats_values(); }

void stat_save_stats_delta(StatsDatabase* sdb) { sdb->save_stats_deltas(); }

void stat_scale_stats(StatsDatabase* sdb) { sdb->scale_all_stats(); }

void stat_scale_stat(BaseStatistic* stat, double weight) { stat->scale_value(weight); }
