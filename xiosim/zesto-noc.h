#ifndef ZESTO_NOC_INCLUDED
#define ZESTO_NOC_INCLUDED

#include <memory>
#include <string>

struct bus_t {
  std::string name;
  int width; /* in bytes tranferrable per cycle */
  const tick_t * clock; /* The sim_cycle used to drive this bus */
  int ratio; /* number of ^ clock cycles per bus cycle */
  tick_t when_available;

  struct {
    counter_t accesses;
    double utilization; /* cumulative cycles in use */
    double prefetch_utilization; /* cumulative prefetch cycles in use */
  } stat;
};

std::unique_ptr<struct bus_t> bus_create(
    const char * const name,
    const int width,
    const tick_t * clock,
    const int ratio);

void bus_reg_stats(
    xiosim::stats::StatsDatabase* sdb,
    struct core_t * const core,
    struct bus_t * const bus);

int bus_free(const struct bus_t * const bus);

void bus_use(
    struct bus_t * const bus,
    const int transfer_size,
    const int prefetch);

#endif /* ZESTO_NOC */
