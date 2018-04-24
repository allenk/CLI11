#pragma once

// Distributed under the 3-Clause BSD License.  See accompanying
// file LICENSE or https://github.com/CLIUtils/CLI11 for details.

#include <string>

#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"

namespace CLI {
namespace detail {

inline std::string
AppFormatter::make_group(std::string group, std::vector<Option *> opts, detail::OptionFormatter::Mode mode) const {
    std::stringstream out;
    out << std::endl << group << ":" << std::endl;
    for(const Option *opt : opts) {
        opt->help(mode);
    }

    return out.str();
}

inline std::string AppFormatter::make_groups(const App *app) const {
    std::stringstream out;
    std::vector<std::string> groups = app->get_groups();
    std::vector<Option *> options = app->get_options();

    // Hide empty options
    options.erase(std::remove_if(options.begin(), options.end(), [](Option *opt) { return opt->get_group().empty(); }),
                  options.end());

    // Positional descriptions
    std::vector<Option *> positionals;
    std::copy_if(options.begin(), options.end(), std::back_inserter(positionals), [](const Option *opt) {
        return opt->has_help_positional();
    });

    if(!positionals.empty())
        make_group(get_label("POSITIONALS"), positionals, detail::OptionFormatter::Mode::Positional);

    // Options
    for(const std::string &group : groups) {
        std::vector<Option *> grouped_items;
        std::copy_if(options.begin(), options.end(), std::back_inserter(grouped_items), [&group](const Option *opt) {
            return opt->nonpositional() && opt->get_group() == group;
        });

        if(!grouped_items.empty())
            make_group(group, grouped_items, detail::OptionFormatter::Mode::Optional);
    }

    return out.str();
}

inline std::string AppFormatter::make_description(const App *app) const { return app->get_description(); }

inline std::string AppFormatter::make_usage(const App *app, std::string name) const {
    std::stringstream out;

    out << get_label("USAGE") << ":" << (name.empty() ? "" : " ") << name;

    std::vector<std::string> groups = app->get_groups();
    std::vector<Option *> options = app->get_options();

    // Print an Options badge if any options exist
    bool non_pos_options =
        std::any_of(options.begin(), options.end(), [](const Option *opt) { return opt->nonpositional(); });
    if(non_pos_options)
        out << " [" << get_label("OPTIONS") << "]";

    // Positionals need to be listed here
    std::vector<Option *> positionals;
    std::copy_if(options.begin(), options.end(), std::back_inserter(positionals), [](const Option *opt) {
        return opt->has_help_positional();
    });

    // Print out positionals if any are left
    if(!positionals.empty()) {
        // Convert to help names
        std::vector<std::string> positional_names(positionals.size());
        std::transform(positionals.begin(), positionals.end(), positional_names.begin(), [](const Option *opt) {
            return opt->help(detail::OptionFormatter::Mode::Usage);
        });

        out << " " << detail::join(positional_names, " ");
    }

    // Add a marker if subcommands are expected or optional
    if(!app->get_subcommands(false).empty()) {
        out << " " << (app->get_require_subcommand_min() == 0 ? "[" : "")
            << get_label(app->get_require_subcommand_max() == 1 ? "SUBCOMMAND" : "SUBCOMMANDS")
            << (app->get_require_subcommand_min() == 0 ? "]" : "");
    }

    return out.str();
}

inline std::string AppFormatter::make_footer(const App *app) const { return app->get_footer(); }

inline std::string AppFormatter::operator()(const App *app, std::string name, App::Formatter::Mode mode) const {

    std::stringstream out;
    out << make_description(app) << std::endl;
    if(mode != App::Formatter::Mode::Sub) {
        out << make_usage(app, name) << std::endl;
        out << make_groups(app) << std::endl;
        out << make_subcommands(app, mode);
        out << make_footer(app) << std::endl;
    } else {
        std::vector<App *> subs = app->get_subcommands(false);
        std::vector<std::string> sub_details(subs.size());
        std::transform(subs.begin(), subs.end(), sub_details.begin(), [](App *sub) {
            return sub->help(App::Formatter::Mode::Sub);
        });
        out << detail::join(sub_details, "\n");
    }

    return out.str();
}

inline std::string AppFormatter::make_subcommands(const App *app, App::Formatter::Mode mode) const {
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

inline std::string AppFormatter::make_subcommand(const App *sub, App::Formatter::Mode mode) const {
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
