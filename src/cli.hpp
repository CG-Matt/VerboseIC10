#pragma once

namespace CLI
{
    namespace Args
    {
        /**
         * Parsed command-line options for one compiler invocation.
         */
        struct Data
        {
            const char* input   = nullptr;
            const char* output  = nullptr;
            bool show_help      = false;
            bool show_bindings  = false;
            bool show_version   = false;
        };

        /**
         * Parses CLI arguments into `args_dest`.
         * Returns false if the arguments are invalid.
         */
        bool Parse(Data& args_dest, int arg_count, char** args);
    }

    /**
     * Prints command-line usage information to stdout.
     */
    void ShowUsage(void);

    /**
     * Prints the current version of the compiler to stdout.
     */
    void ShowVersion(void);
};
