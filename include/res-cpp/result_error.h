#ifndef RESCPP_RESULT_ERROR_H
#define RESCPP_RESULT_ERROR_H

#include "res-cpp/result_error_base.h"
#include "res-cpp/formatted_error.h"

#if defined(RESCPP_RESULT_ERROR) && !defined(RESCPP_RESULT_ERROR_HEADER)
#error "RESCPP_RESULT_ERROR_HEADER also needs to be defined"
#endif

#ifndef RESCPP_RESULT_ERROR
namespace ResCpp {
using ResultError = FormattedError;
}
#else
namespace ResCpp {
#include RESCPP_RESULT_ERROR_HEADER
using ResultError = RESCPP_RESULT_ERROR;
static_assert(std::is_base_of_v<ResultErrorBase, ResultError>,
              "ResultError type should have ResultErrorBase as base type/interface.");
}
#endif


#endif //RESCPP_RESULT_ERROR_H
