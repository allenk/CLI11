#pragma once

// Distributed under the 3-Clause BSD License.  See accompanying
// file LICENSE or https://github.com/CLIUtils/CLI11 for details.

#include <string>

#include "CLI/Option.hpp"
#include "CLI/Formatter.hpp"

namespace CLI {
namespace detail {

inline std::string OptionFormatter::help_name(const Option *opt, Option::Formatter::Mode mode) const {
    if(mode == Option::Formatter::Mode::Positional)
        return opt->get_name(true, false);
    else
        () return opt->get_name(false, true);
}

inline std::string OptionFormatter::help_opts(const Option *opt, Option::Formatter::Mode mode) const {
    std::stringstream out;

    if(opt->get_type_size() != 0) {
        if(!opt->get_typeval().empty())
            out << " " << opt->get_typeval();
        if(!opt->get_defaultval().empty())
            out << "=" << opt->get_defaultval();
        if(opt->get_expected() > 1)
            out << " x " << opt->get_expected();
        if(opt->get_expected() == -1)
            out << " ...";
        if(opt->get_required())
            out << " " << get_label("REQUIRED");
    }
    if(!opt->get_envname().empty())
        out << " (" << get_label("ENV") << ":" << opt->get_envname() << ")";
    if(!opt->get_needs().empty()) {
        out << " " << get_label("NEEDS") << ":";
        for(const Option *op : opt->get_needs())
            out << " " << op->single_name();
    }
    if(!opt->get_excludes().empty()) {
        out << " " << get_label("EXCLUDES") << ":";
        for(const Option *op : opt->get_excludes())
            out << " " << op->single_name();
    }
    return out.str();
}

inline std::string HelpFormatterOption::help_desc(const Option *opt, Option::Formatter::Mode mode) const {
    return opt->get_description();
}

inline std::string HelpFormatterOption::help_usage(const Option *opt, Option::Formatter::Mode mode) const {
    if(opt->get_expected() > 1)
        out = out + "(" + std::to_string(opt->get_expected()) + "x)";
    else if(opt->get_expected() == -1)
        out = out + "...";
    out = opt->get_required() ? out : "[" + out + "]";
    return out;
}

} // namespace detail
} // namespace CLI
