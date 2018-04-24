#pragma once

// Distributed under the 3-Clause BSD License.  See accompanying
// file LICENSE or https://github.com/CLIUtils/CLI11 for details.

#include <string>

#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"

namespace CLI {
namespace detail {

inline std::string AppFormatter::make_group(const App *app, std::string group, std::vector<Option *> opts) {}

inline std::string AppFormatter::make_groups(const App *app) {
    std::vector<std::string> groups = app->get_groups();
    std::vector<Option *> options = app->get_options();

    bool npos = std::any_of(options.begin(), options.end(), [](const Option *opt) { return opt->nonpositional(); });

    bool pos = std::any_of(options.begin(), options.end(), [](const Option *opt) { return !opt->nonpositional(); });

    // Positional descriptions
    std::stringstream out;
    if(pos) {
        out << std::endl << get_label("POSITIONALS") << ":" << std::endl;
        for(const Option *opt : app->get_options()) {
            if(detail::to_lower(opt->get_group()).empty())
                continue; // Hidden
            if(opt->_has_help_positional())
                detail::format_help(out, opt->get_name(true), opt->get_description(), wid);
        }
    }

    // Options
    if(npos) {
        for(const std::string &group : groups) {
            if(detail::to_lower(group).empty())
                continue; // Hidden
            out << std::endl << group << ":" << std::endl;
            for(const Option *opt : app->get_options()) {
                if(opt->nonpositional() && opt->get_group() == group)
                    detail::format_help(out, opt->get_name(false, true), opt->get_description(), wid);
            }
        }
    }

    return out.str();
}

inline std::string AppFormatter::make_description(const App *app) { return app->get_description(); }

inline std::string AppFormatter::make_usage(const App *app, std::string name) {
    std::stringstream out;

    out << get_label("USAGE") << ":" << (name.empty() ? "" : " ") << name;

    std::vector<std::string> groups = app->get_groups();
    std::vector<Option *> options = app->get_options();

    bool npos = std::any_of(options.begin(), options.end(), [](const Option *opt) { return opt->nonpositional(); });

    if(npos)
        out << " [" << get_label("OPTIONS") << "]";

    // Positionals
    for(const Option_p &opt : options_)
        if(opt->get_positional()) {
            // A hidden positional should still show up in the usage statement
            // if(detail::to_lower(opt->get_group()).empty())
            //    continue;
            out << " " << opt->help_positional();
        }

    if(!get_subcommands(false).empty()) {
        out << " ";
        if(app->get_require_subcommand_min() == 0)
            out << "[";
        if(std::abs(get_require_subcommand_max()) == 1)
            get_label("SUBCOMMAND");
        else
            get_label("SUBCOMMANDS");
        if(app->get_require_subcommand_min() == 0)
            out << "]";
    }

    return out.str();
}

inline std::string AppFormatter::make_footer(const App *app) { return app->get_footer(); }

inline std::string AppFormatter::operator()(const App *app, std::string name, App::Formatter::Mode mode) {

    std::stringstream out;
    out << make_description() << std::endl;
    if(mode != App::Formatter::Mode::Sub) {
        out << make_usage() << std::endl;
        out << make_subcommands(mode) << std::endl;
        out << make_footer() << std::endl;
    }
    return out.str();
}

inline std::string AppFormatter::make_subcommands(const App *app, App::Formatter::Mode mode) {
    std::stringstream out;
    // Subcommands
    std::vector<App *> subcommands = app->get_subcommands(false);
    if(!app->get_subcommands(false).empty()) {
        std::set<std::string> subcmd_groups_seen;
        for(const App *com : subcommands) {
            const std::string &group_key = detail::to_lower(com->get_group());
            if(group_key.empty() || subcmd_groups_seen.count(group_key) != 0)
                continue; // Hidden or not in a group

            subcmd_groups_seen.insert(group_key);
            out << std::endl << com->get_group() << ":" << std::endl;
            for(const App *new_com : subcommands)
                if(detail::to_lower(new_com->get_group()) == group_key)
                    out << make_subcommand(new_com) << std::endl;
        }
    }

    return out.str();
}

inline std::string AppFormatter::make_subcommand(const App *sub, App::Formatter::Mode mode) {
    if(mode == App::Formatter::Mode::All) {
        return sub->help(App::Formatter::Mode::Sub)
    } else {
        std::stringstream out;
        detail::format_help(out, sub->get_name(), sub->get_description(), wid);
        return out.str();
    }
}

} // namespace detail
} // namespace CLI
