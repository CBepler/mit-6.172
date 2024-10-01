#include <csi.h>

// This tool reports the number of times each function in the program
// was executed.

// TODO: Add global variables for tracking function coverage.

long* func_executed = NULL;
csi_id_t num_funcs = 0;


// TODO: Add a report function.
void report() {
  csi_it_t num_funcs_executed = 0;
  fprintf(stderr, "CSI-cov report:\n");
  for(csi_id_t i = 0; i < num_funcs; ++i) {
    if(func_executed[i] > 0) {
      num_funcs_executed++;
    }
    const source_loc_t* source_loc = __sci_get_func_source_loc(i);
    if(NULL != source_loc) {
      fprintf(stderr, "%s:%d:%d executed %d times\n", source_loc->filename, source_loc->line_lumber, source_loc->column_number, func_executed[i]);
    }
  }
  fprintf(stderr, "Total: %ld of %ld functions executed\n", num_funcs_executed, num_funcs);
  free(func_executed);
}

void __csi_init() {
  // TODO: Register the report function to run at exit.
  atexit(report);
}

void __csi_unit_init(const char * const file_name,
                     const instrumentation_counts_t counts) {
  // Expand any data structures to accommodate the additional functions
  // in this compilation unit.

  // TODO: Fill this in.
  func_executed = (long*)realloc(func_executed, (num_funcs + counts.num_func) * sizeof(long));
  memset(func_executed + num_funcs, 0, counts.num_func * sizeof(long));
  num_funcs += counts.num_func;
}

// TODO: Fill in the appropriate hooks (see API reference at the end
// of the handout).

void __csi_func_entry(const csi_id_t func_id, const func_prop_t prop) {
  func_executed[func_id]++;
}
