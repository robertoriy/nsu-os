#define CHECK_FUNCTION(function, action_on_error) do \
        { \
            int return_value = (function); \
            if (EXIT_SUCCESS != return_value) { \
                fprintf(stderr, "Runtime error: %s returned %d at %s:%d\n", #function, return_value, __FILE__, __LINE__); \
                action_on_error; \
            } \
        } while (0)

#define CHECK_THREAD_ARGUMENTS(return_value, action_on_error) do \
        { \
            if ((void*)EXIT_FAILURE == return_value) { \
                fprintf(stderr, "Runtime error: Thread have got wrong arguments. Error at %s:%d\n", __FILE__, __LINE__); \
                action_on_error; \
            } \
        } while (0)

#define CHECK_NULL(value, action_on_error) do\
        { \
            if (NULL == value) { \
                fprintf(stderr, "Runtime error: Unexpected NULL occured at %s:%d\n", __FILE__, __LINE__); \
                action_on_error; \
            } \
        } while (0)
