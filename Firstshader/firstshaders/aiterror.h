#ifndef AITERROR_H
#define AITERROR_H

#ifdef _DEBUG
#   define AIT_ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
		 abort(); \
        } \
    } while (false)
#else
#   define AIT_ASSERT(condition, message) // Do nothing
#endif


#endif AITERROR_H