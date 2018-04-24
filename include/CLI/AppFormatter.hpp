#pragma once

// Distributed under the 3-Clause BSD License.  See accompanying
// file LICENSE or https://github.com/CLIUtils/CLI11 for details.

#include <string>

#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"

namespace CLI {
namespace detail {

inline std::string AppFormatter::make_group(std::string group,
                                            std::vector<const Option *> opts,
                                            detail::OptionFormatter::Mode mode) const {
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
    std::vector<const Option *> positionals =
        app->get_options([](const Option *opt) { return !opt->get_group().empty() && opt->has_help_positional(); });

    if(!positionals.empty())
        make_group(get_label("POSITIONALS"), positionals, detail::OptionFormatter::Mode::Positional);

    // Options
    for(const std::string &group : groups) {
        std::vector<const Option *> grouped_items = app->get_options([&group](const Option *opt) {
            return !opt->get_group().empty() && opt->nonpositional() && opt->get_group() == group;
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

    // Print an Options badge if any options exist
    std::vector<const Option *> non_pos_options =
        app->get_options([](const Option *opt) { return opt->nonpositional(); });
    if(!non_pos_options.empty())
        out << " [" << get_label("OPTIONS") << "]";

    // Positionals need to be listed here
    std::vector<const Option *> positionals =
        app->get_options([](const Option *opt) { return opt->has_help_positional(); });

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
    if(!app->get_subcommands({}).empty()) {
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
    if(mode == App::Formatter::Mode::Normal) {
        out << make_usage(app, name) << std::endl;
        out << make_groups(app) << std::endl;
        out << make_subcommands(app, mode);
        out << make_footer(app) << std::endl;
    } else if(mode == App::Formatter::Mode::Sub) {
        out << make_subcommand(app, mode);
    } else if(mode == App::Formatter::Mode::All) {
        std::vector<const App *> subs = app->get_subcommands({});
        std::vector<std::string> sub_details(subs.size());
        std::transform(subs.begin(), subs.end(), sub_details.begin(), [](const App *sub) {
            return sub->help(sub->get_name(), App::Formatter::Mode::Sub);
        });
        out << detail::join(sub_details, "\n");
    }

    return out.str();
}

inline std::string AppFormatter::make_subcommands(const App *app, App::Formatter::Mode mode) const {
    std::stringstream out;

    std::vector<const App *> subcommands = app->get_subcommands({});

    if(!subcommands.empty()) {

        // Make a list in definition order of the groups seen
        std::vector<std::string> subcmd_groups_seen;
        for(const App *com : subcommands) {
            std::string group_key = detail::to_lower(com->get_group());
            if(!group_key.empty() &&
               std::find(subcmd_groups_seen.begin(), subcmd_groups_seen.end(), group_key) != subcmd_groups_seen.end())
                subcmd_groups_seen.push_back(group_key);
        }

        // For each group, filter out and print subcommands
        for(const std::string &group : subcmd_groups_seen) {
            out << std::endl << group << ":" << std::endl;
            std::vector<const App *> subcommands_group =
                app->get_subcommands([&group](const App *app) { return detail::to_lower(app->get_group()) == group; });
            for(const App *new_com : subcommands_group)
                out << detail::join(make_subcommand(new_com, mode));
        }
    }

    return out.str();
}

inline std::string AppFormatter::make_subcommand(const App *sub, App::Formatter::Mode mode) const {
    if(mode == App::Formatter::Mode::All) {
        return sub->help(sub->get_name(), AppFormatter::Mode::Sub);
    } else {
        std::stringstream out;
        detail::format_help(out, sub->get_name(), sub->get_description(), column_width_);
        return out.str();
    }
}

} // namespace detail
} // namespace CLI
