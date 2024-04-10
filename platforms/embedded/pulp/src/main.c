#include <stdio.h>
#include <stdbool.h>
#include "wasm3.h"
#include "extra/test_add.wasm.h"

#define FATAL(func, msg) {      \
  printf("Fatal: " func ": ");  \
  printf(msg); printf("\n");    \
  return false; }

bool run_wasm()
{
    M3Result result = m3Err_none;

    uint8_t* wasm = (uint8_t*)test_add_wasm;
    size_t fsize = test_add_wasm_len;

    m3_PrintM3Info();

    printf("Loading WebAssembly...\n");

    IM3Environment env = m3_NewEnvironment ();
    if (!env) FATAL("m3_NewEnvironment", "failed");

    IM3Runtime runtime = m3_NewRuntime (env, 1024, NULL);
    if (!runtime) FATAL("m3_NewRuntime", "failed");

    IM3Module module;
    result = m3_ParseModule (env, &module, wasm, fsize);
    if (result) FATAL("m3_ParseModule", result);

    printf("Loading module...\n");

    result = m3_LoadModule (runtime, module);
    if (result) FATAL("m3_LoadModule", result);

    printf("Finding function...\n");

    IM3Function f;
    result = m3_FindFunction (&f, runtime, "add");
    if (result) FATAL("m3_FindFunction", result);

    printf("Running...\n");

    result = m3_CallV (f, 11, 222);
    if (result) FATAL("m3_Call", result);

    printf("Getting results...\n");

    uint32_t value = 0;
    result = m3_GetResultsV (f, &value);
    if (result) FATAL("m3_GetResults", result);

    printf("Result: %d\n", value);

    return true;
}

int main() {
    printf("\nWasm3 v" M3_VERSION " on Control-PULP (" M3_ARCH "), build " __DATE__ " " __TIME__ "\n");
    if (run_wasm()) {
        printf("SUCCESS\n");
    } else {
        printf("FAIL\n");
    }
    return 0;
}
