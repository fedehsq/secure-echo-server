#define handle_error(S, M)\
    if ((S) == -1) {\
        fprintf(stderr, " Line %d ", __LINE__);\
        perror(M);\
        exit(EXIT_FAILURE);\
    }

#define handle_null(S, M)\
    if ((S) == NULL) {\
        fprintf(stderr, " Line %d ", __LINE__);\
        perror(M);\
        exit(EXIT_FAILURE);\
    }

#define handle_null_ssl(S)\
    if ((S) == NULL) {\
        fprintf(stderr, " Line %d ", __LINE__);\
        ERR_print_errors_fp(stderr);\
        exit(EXIT_FAILURE);\
    }

#define handle_negative_ssl(S)\
    if ((S) <= 0) {\
        fprintf(stderr, " Line %d ", __LINE__);\
        ERR_print_errors_fp(stderr);\
        exit(EXIT_FAILURE);\
    }

#define handle_zero(S, M) \
    if ((S) == 0) { \
        fprintf(stderr, " Line %d ", __LINE__); \
        perror(M); \
        exit(EXIT_FAILURE); \
    }
