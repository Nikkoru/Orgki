#include "Context.hpp"
#include <format>
namespace Orgki {
template<class ...Args>
void Context::PrintToBuffer(std::string_view fmt, Args&&... elms) {
    std::format_args fmtElms = std::make_format_args();
    std::string fmtOutput{};

    if constexpr (sizeof...(elms) > 0) {
        fmtElms = std::make_format_args(elms...);
        fmtOutput = std::vformat(fmt, fmtElms);
    }

    buffer << fmtOutput;
}

template<class ...Args>
void Context::PrintLnToBuffer(std::string_view fmt, Args&&... elms) {
    buffer << '\n';
    PrintToBuffer(fmt, elms...);
}
}
